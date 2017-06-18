#include "fluidsim.h"

#include <QQuickWindow>
#include <QtOpenGL/QtOpenGL>

static const char *vertexShaderSource =
    "#version 130\n"
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

    m_progFluid = new QOpenGLShaderProgram;
    m_progFluid->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    {
        QFile frag(":/fluid.frag");
        frag.open(QFile::ReadOnly);
        m_progFluid->addShaderFromSourceCode(QOpenGLShader::Fragment, QString::fromUtf8(frag.readAll()));
    }
    m_progFluid->link();

    m_progDisp = new QOpenGLShaderProgram;
    m_progDisp->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    {
        QFile frag(":/display.frag");
        frag.open(QFile::ReadOnly);
        m_progDisp->addShaderFromSourceCode(QOpenGLShader::Fragment, QString::fromUtf8(frag.readAll()));
    }
    m_progDisp->link();

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

    initializeBuffer();

    m_progFluid->bind();
    m_progFluid->setUniformValue(1,1./simw,1./simh); // inv_size
}

void FluidRenderer::initializeBuffer() {
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    for (int i = 0; i < 2; ++i) {
        m_fieldFbo[i] = new QOpenGLFramebufferObject(simw,simh,
                                  QOpenGLFramebufferObject::NoAttachment,
                                  GL_TEXTURE_2D,GL_RGBA16F);
        f->glBindTexture(GL_TEXTURE_2D,m_fieldFbo[i]->texture());
        f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }
    m_domainFbo = new QOpenGLFramebufferObject(simw,simh,
                              QOpenGLFramebufferObject::NoAttachment,
                              GL_TEXTURE_2D,GL_LUMINANCE4);

    QOpenGLShaderProgram progInit;
    progInit.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    {
        QFile frag(":/init.frag");
        frag.open(QFile::ReadOnly);
        progInit.addShaderFromSourceCode(QOpenGLShader::Fragment, QString::fromUtf8(frag.readAll()));
    }
    progInit.link();

    f->glViewport(0,0,simw,simh);

    QOpenGLTexture den_tex(QImage(":/init.png"));
    den_tex.bind(0);
    progInit.bind();

    m_fieldFbo[m_cfbo]->bind();
    progInit.setUniformValue(0,GLint(0));
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    m_fieldFbo[m_cfbo]->release();

    m_domainFbo->bind();
    progInit.setUniformValue(0,GLint(1));
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    m_domainFbo->release();
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

    f->glViewport(0,0,simw,simh);
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
    update();
}

void FluidRenderer::synchronize(QQuickFramebufferObject */*item*/) {
    m_progFluid->bind();
    m_progFluid->setUniformValue(0,GLfloat(m_item->m_dt));
    m_progFluid->setUniformValue(2,GLfloat(m_item->m_v));
    m_progFluid->setUniformValue(3,GLfloat(m_item->m_k));
    m_progFluid->setUniformValue(4,GLfloat(0),GLfloat(m_item->m_g));
    m_iterations = m_item->m_factor;
    m_progFluid->release();
}

QOpenGLFramebufferObject *FluidRenderer::createFramebufferObject(const QSize &size) {
    return new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::NoAttachment);
}
QQuickFramebufferObject::Renderer *FluidSim::createRenderer() const {
    return new FluidRenderer(this);
}
