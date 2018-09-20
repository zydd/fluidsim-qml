#ifndef FLUIDSIM_H
#define FLUIDSIM_H

#include <QQuickFramebufferObject>
#include <QQuickItemGrabResult>

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
    QSize m_simSize = QSize(0,0);

    const FluidSim *m_item;
    QOpenGLShaderProgram *m_progFluid;
    QOpenGLShaderProgram *m_progDisp;
    QOpenGLShaderProgram *m_progInit;
    QOpenGLBuffer *m_vbo;
    QOpenGLFramebufferObject *m_fieldFbo[2] = {nullptr, nullptr};
    QOpenGLVertexArrayObject *m_vao;
    QImage m_init_tex;
    int m_cfbo = 0;
    int m_iterations = 1;
    bool m_running = true;

    void initializeBuffer();
    QOpenGLShaderProgram *linkFragment(QString frag);
};

class FluidSim : public QQuickFramebufferObject {
    Q_OBJECT
public:
    friend class FluidRenderer;

    Q_PROPERTY(qreal g MEMBER m_g WRITE setG NOTIFY gChanged)
    Q_PROPERTY(qreal k MEMBER m_k WRITE setK NOTIFY kChanged)
    Q_PROPERTY(qreal dt MEMBER m_dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(qreal v MEMBER m_v WRITE setV NOTIFY vChanged)
    Q_PROPERTY(int factor MEMBER m_factor WRITE setFactor NOTIFY factorChanged)
    Q_PROPERTY(int display MEMBER m_display WRITE setDisplay NOTIFY displayChanged)
    Q_PROPERTY(int simw MEMBER m_simw WRITE setSimw NOTIFY simwChanged)
    Q_PROPERTY(int simh MEMBER m_simh WRITE setSimh NOTIFY simhChanged)
    Q_PROPERTY(bool running MEMBER m_running WRITE setRunning NOTIFY runningChanged)

    FluidSim();

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

    inline void setFactor(int factor) {
        if (m_factor == factor) return;
        m_factor = factor;
        emit factorChanged(factor);
        update();
    }

    inline void reset() {
        m_initMode = 0;
        update();
    }

    inline void setDisplay(int display) {
        if (m_display == display) return;
        m_display = display;
        emit displayChanged(display);
        update();
    }

    inline void setSimw(int simw) {
        if (m_simw == simw) return;
        m_simw = simw;
        emit simwChanged(simw);
        update();
    }

    inline void setSimh(int simh) {
        if (m_simh == simh) return;
        m_simh = simh;
        emit simhChanged(simh);
        update();
    }

    inline void den_ellipse(QPoint pos, qreal rad, qreal density) {
        m_initMode = 2;
        m_ellp = pos;
        m_ellr = rad;
        m_elld = density;
        update();
    }

    inline void setRunning(bool running) {
        if (m_running == running) return;
        m_running = running;
        emit runningChanged(running);
        update();
    }

    inline void setInitTex(QQuickItemGrabResult *tex) {
        m_init_tex = tex->image();
    }

private:
    qreal m_g = 0.07;
    qreal m_k = 4;
    qreal m_dt = 0.05;
    qreal m_v = 0.05;
    int m_factor = 1;
    int m_display = 0;
    int m_simw = 256;
    int m_simh = 256;
    int m_initMode = -1;
    QPoint m_ellp;
    qreal m_ellr;
    qreal m_elld;
    bool m_running = true;
    QImage m_init_tex;

signals:
    void gChanged(qreal);
    void dtChanged(qreal);
    void kChanged(qreal);
    void vChanged(qreal);
    void factorChanged(unsigned);
    void displayChanged(unsigned display);
    void simwChanged(unsigned simw);
    void simhChanged(unsigned simh);
    void runningChanged(bool running);
};

#endif // FLUIDSIM_H

