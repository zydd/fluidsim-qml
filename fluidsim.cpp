#include "fluidsim.h"

#include <QQuickWindow>
#include <QtOpenGL/QtOpenGL>

static const char *vertexShaderSource =
    "#version 330\n"
    "in vec2 vertex;\n"
    "out vec2 uv;\n"

    "void main() {\n"
    "   uv = vertex/vec2(2,2) + vec2(0.5);\n"
    "   gl_Position = vec4(vertex,0,1);\n"
    "}\n";

FluidRenderer::FluidRenderer(const FluidSim *parent)
    : m_item(parent)
{
    auto f = QOpenGLContext::currentContext()->functions();

    m_progFluid = linkFragment(":/fluid.frag");
    m_progDisp = linkFragment(":/display.frag");
    m_progInit = linkFragment(":/init.frag");

    m_vao = new QOpenGLVertexArrayObject;
    if (m_vao->create())
        m_vao->bind();

    m_vbo = new QOpenGLBuffer;
    m_vbo->create();
    m_vbo->bind();

    static GLfloat vertices[] = {
        -1,  1,
         1, -1,
         1,  1,
        -1,  1,
        -1, -1,
         1, -1,
    };

    m_vbo->allocate(vertices, sizeof(vertices));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    m_vbo->release();
}

QOpenGLShaderProgram *FluidRenderer::linkFragment(QString frag) {
    QFile fragfile(frag);
    fragfile.open(QFile::ReadOnly);

    auto prog = new QOpenGLShaderProgram;
    prog->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    prog->addShaderFromSourceCode(QOpenGLShader::Fragment, QString::fromUtf8(fragfile.readAll()));
    prog->link();
    return prog;
}

void FluidRenderer::initializeBuffer() {
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    if (! m_fieldFbo[0] || m_fieldFbo[0]->size() != m_simSize) {
        for (int i = 0; i < 2; ++i) {
            delete m_fieldFbo[i];
            m_fieldFbo[i] = new QOpenGLFramebufferObject(m_simSize,
                                      QOpenGLFramebufferObject::NoAttachment,
                                      GL_TEXTURE_2D, GL_RGBA16F);
            m_fieldFbo[i]->addColorAttachment(m_simSize, GL_RGBA16F);
            f->glBindTexture(GL_TEXTURE_2D, m_fieldFbo[i]->texture());
            f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        }
    }

    m_item->window()->resetOpenGLState();
    f->glViewport(0, 0, m_simSize.width(),m_simSize.height());
    m_vao->bind();

    m_progInit->bind();
    m_progInit->setUniformValue(0, GLint(m_item->m_initMode));
    GLenum bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    switch (m_item->m_initMode) {
    case 0: case 1: {
        qDebug() << "init0";
        QOpenGLTexture den_tex(m_init_tex);
        f->glActiveTexture(GL_TEXTURE1);
        den_tex.bind();

        m_fieldFbo[0]->bind();
        f->glDrawBuffers(2, bufs);
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[0]->release();

        m_fieldFbo[1]->bind();
        f->glDrawBuffers(2, bufs);
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[1]->release();

        break;
    }
    case 2: {
        m_fieldFbo[!m_cfbo]->bind();
        f->glActiveTexture(GL_TEXTURE0);
        f->glBindTexture(GL_TEXTURE_2D, m_fieldFbo[m_cfbo]->textures()[0]);
        f->glActiveTexture(GL_TEXTURE1);
        f->glBindTexture(GL_TEXTURE_2D, m_fieldFbo[m_cfbo]->textures()[1]);

        QPointF point = m_item->m_ellp;
        point.setX(point.x() / m_item->width());
        point.setY(point.y() / m_item->height());
        m_progInit->setUniformValue(2, point);
        m_progInit->setUniformValue(3, GLfloat(m_item->m_ellr));
        m_progInit->setUniformValue(4, GLfloat(m_item->width() / m_item->height()));
        m_progInit->setUniformValue(5, GLfloat(m_item->m_elld));
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[!m_cfbo]->release();

        m_cfbo = !m_cfbo;
        break;
    }
    default:
        qDebug() << "init-1" << m_item->m_initMode;
        break;
    }

    m_vao->release();
    m_item->window()->resetOpenGLState();
}

void FluidRenderer::render() {
    auto f = QOpenGLContext::currentContext()->extraFunctions();

    m_vao->bind();
    m_progDisp->bind();
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, m_fieldFbo[m_cfbo]->textures()[0]);
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, m_fieldFbo[m_cfbo]->textures()[1]);
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    m_vao->release();

    m_item->window()->resetOpenGLState();

    f->glViewport(0,0,m_simSize.width(),m_simSize.height());

    m_vao->bind();

    for (int i = 0; i < m_iterations; ++i) {
        m_fieldFbo[!m_cfbo]->bind();
        f->glActiveTexture(GL_TEXTURE0);
        f->glBindTexture(GL_TEXTURE_2D, m_fieldFbo[m_cfbo]->textures()[0]);
        f->glActiveTexture(GL_TEXTURE1);
        f->glBindTexture(GL_TEXTURE_2D, m_fieldFbo[m_cfbo]->textures()[1]);

        m_progFluid->bind();
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[!m_cfbo]->release();

        m_cfbo = !m_cfbo;
    }

    m_vao->release();

    m_item->window()->resetOpenGLState();

    if (m_running)
        update();
}

void FluidRenderer::synchronize(QQuickFramebufferObject *item) {
    auto sim = dynamic_cast<FluidSim *>(item);
    if (!sim) return;
    bool reinit_buffer = false;
    m_running = m_item->m_running;
    m_progDisp->bind();
    m_progDisp->setUniformValue(0,GLint(sim->m_display));
    m_progFluid->bind();
    m_progFluid->setUniformValue(0, GLfloat(sim->m_dt));
    m_progFluid->setUniformValue(2, GLfloat(sim->m_v));
    m_progFluid->setUniformValue(3, GLfloat(sim->m_k));
    m_progFluid->setUniformValue(4, GLfloat(0), GLfloat(sim->m_g));
    m_progFluid->setUniformValue(5, GLfloat(sim->m_vconf));
    m_iterations = sim->m_factor;

    if (m_simSize != QSize(sim->m_simw, sim->m_simh)) {
        m_simSize = QSize(sim->m_simw, sim->m_simh);
        m_progFluid->setUniformValue(1, 1.0f / sim->m_simw, 1.0f / sim->m_simh);
        reinit_buffer = true;
    }

    m_progFluid->release();

    if (m_init_tex != sim->m_init_tex) {
        m_init_tex = sim->m_init_tex;
        reinit_buffer = true;
    }

    if (reinit_buffer || sim->m_initMode >= 0) {
        initializeBuffer();
        sim->m_initMode = -1;
    }
}

QOpenGLFramebufferObject *FluidRenderer::createFramebufferObject(const QSize &size) {
    return new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::NoAttachment);
}

FluidSim::FluidSim() {
    m_init_tex = QPixmap(1,1).toImage();
}

QQuickFramebufferObject::Renderer *FluidSim::createRenderer() const {
    return new FluidRenderer(this);
}
