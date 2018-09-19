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
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
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

    if (! m_domainFbo || m_domainFbo->size() != m_simSize) {
        delete m_domainFbo;
        delete m_fieldFbo[0];
        delete m_fieldFbo[1];
        for (int i = 0; i < 2; ++i) {
            m_fieldFbo[i] = new QOpenGLFramebufferObject(m_simSize,
                                      QOpenGLFramebufferObject::NoAttachment,
                                      GL_TEXTURE_2D,GL_RGBA16F);
            f->glBindTexture(GL_TEXTURE_2D,m_fieldFbo[i]->texture());
            f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        }
        m_domainFbo = new QOpenGLFramebufferObject(m_simSize,
                                  QOpenGLFramebufferObject::NoAttachment,
                                  GL_TEXTURE_2D,GL_RGBA8);
        m_progInit->bind();
        m_progInit->setUniformValue(1,m_simSize);
    }

    m_item->window()->resetOpenGLState();
    f->glViewport(0,0,m_simSize.width(),m_simSize.height());
    m_vao->bind();

    QOpenGLTexture den_tex(QImage(":/init.png"));
    den_tex.bind(0);
    m_progInit->bind();

    switch (m_item->m_initMode) {
    case 0: case 1: default:
        m_fieldFbo[0]->bind();
        m_progInit->setUniformValue(0, GLint(0));
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[0]->release();

        m_fieldFbo[1]->bind();
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[1]->release();

        m_domainFbo->bind();
        m_progInit->setUniformValue(0, GLint(1));
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_domainFbo->release();
        break;
    case 2: {
        m_fieldFbo[m_cfbo]->bind();
        f->glActiveTexture(GL_TEXTURE0);
        f->glBindTexture(GL_TEXTURE_2D,m_fieldFbo[m_cfbo]->texture());
        m_progInit->setUniformValue(0, GLint(m_item->m_initMode));
        QPointF point = m_item->m_ellp;
        point.setX(point.x() / m_item->width());
        point.setY(point.y() / m_item->height());
        m_progInit->setUniformValue(2,point);
        m_progInit->setUniformValue(3, GLfloat(m_item->m_ellr));
        m_progInit->setUniformValue(4, GLfloat(m_item->width()/m_item->height()));
        m_progInit->setUniformValue(5, GLfloat(m_item->m_elld));
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[m_cfbo]->release();
    }
        break;
    }

    m_vao->release();
}

void FluidRenderer::render() {
    auto f = QOpenGLContext::currentContext()->functions();

    m_vao->bind();

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D,m_fieldFbo[m_cfbo]->texture());
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D,m_domainFbo->texture());

    m_progDisp->bind();
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    m_vao->release();

    m_item->window()->resetOpenGLState();

    f->glViewport(0,0,m_simSize.width(),m_simSize.height());
    m_vao->bind();

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D,m_domainFbo->texture());
    for (int i = 0; i < m_iterations; ++i) {
        f->glActiveTexture(GL_TEXTURE0);
        f->glBindTexture(GL_TEXTURE_2D,m_fieldFbo[m_cfbo]->texture());

        m_fieldFbo[!m_cfbo]->bind();
        m_progFluid->bind();
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        m_fieldFbo[!m_cfbo]->release();

        m_cfbo = !m_cfbo;
    }

    m_vao->release();

    if (m_running)
        update();
}

void FluidRenderer::synchronize(QQuickFramebufferObject *item) {
    auto sim = dynamic_cast<FluidSim *>(item);
    if (!sim) return;
    m_running = m_item->m_running;
    m_progDisp->bind();
    m_progDisp->setUniformValue(0,GLint(sim->m_display));
    m_progFluid->bind();
    m_progFluid->setUniformValue(0,GLfloat(sim->m_dt));
    m_progFluid->setUniformValue(2,GLfloat(sim->m_v));
    m_progFluid->setUniformValue(3,GLfloat(sim->m_k));
    m_progFluid->setUniformValue(4,GLfloat(0),GLfloat(sim->m_g));
    m_iterations = sim->m_factor;

    if (m_simSize != QSize(sim->m_simw,sim->m_simh)) {
        m_simSize = QSize(sim->m_simw,sim->m_simh);
        m_progFluid->setUniformValue(1,1./sim->m_simw,1./sim->m_simh);
        initializeBuffer();
    }

    m_progFluid->release();

    if (sim->m_initMode >= 0) {
        initializeBuffer();
        sim->m_initMode = -1;
    }
}

QOpenGLFramebufferObject *FluidRenderer::createFramebufferObject(const QSize &size) {
    return new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::NoAttachment);
}
QQuickFramebufferObject::Renderer *FluidSim::createRenderer() const {
    return new FluidRenderer(this);
}
