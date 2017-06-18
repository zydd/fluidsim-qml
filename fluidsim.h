#ifndef FLUIDSIM_H
#define FLUIDSIM_H

#include <QQuickFramebufferObject>

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QOpenGLFramebufferObject;
class FluidSim;

class FluidRenderer : public QQuickFramebufferObject::Renderer {
public:
    FluidRenderer(const FluidSim *parent);
    virtual void synchronize(QQuickFramebufferObject *item) override;
    virtual void render() override;
    virtual QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    const int simw = 256;
    const int simh = 256;

    const FluidSim *m_item;
    QOpenGLShaderProgram *m_progFluid;
    QOpenGLShaderProgram *m_progDisp;
    QOpenGLBuffer *m_vbo;
    QOpenGLFramebufferObject *m_fieldFbo[2];
    QOpenGLFramebufferObject *m_domainFbo;
    QOpenGLVertexArrayObject *m_vao;
    int m_cfbo = 0;
    int m_iterations = 1;

    void initializeBuffer();
};

class FluidSim : public QQuickFramebufferObject {
    Q_OBJECT
public:
    friend class FluidRenderer;

    Q_PROPERTY(qreal g MEMBER m_g WRITE setG NOTIFY gChanged)
    Q_PROPERTY(qreal k MEMBER m_k WRITE setK NOTIFY kChanged)
    Q_PROPERTY(qreal dt MEMBER m_dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(qreal v MEMBER m_v WRITE setV NOTIFY vChanged)
    Q_PROPERTY(unsigned factor MEMBER m_factor WRITE setFactor NOTIFY factorChanged)
    virtual QQuickFramebufferObject::Renderer *createRenderer() const override;

public slots:
    inline void setDt(qreal dt) {
        if (m_dt == dt) return;
        m_dt = dt;
        emit dtChanged(dt);
        update();
    }

    inline void setG(qreal g) {
        if (m_g == g) return;
        m_g = g;
        emit gChanged(g);
        update();
    }

    inline void setK(qreal k) {
        if (m_k == k) return;
        m_k = k;
        emit kChanged(k);
        update();
    }

    inline void setV(qreal v) {
        if (m_v == v) return;
        m_v = v;
        emit vChanged(v);
        update();
    }

    inline void setFactor(unsigned factor) {
        if (m_factor == factor) return;
        m_factor = factor;
        emit factorChanged(factor);
        update();
    }

private:
    qreal m_g;
    qreal m_k;
    qreal m_dt;
    qreal m_v;
    unsigned m_factor;

signals:
    void gChanged(qreal);
    void dtChanged(qreal);
    void kChanged(qreal);
    void vChanged(qreal);
    void factorChanged(unsigned);
};

#endif // FLUIDSIM_H

