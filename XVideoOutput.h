#ifndef XVIDEOOUTPUT_H
#define XVIDEOOUTPUT_H

#include <QQuickPaintedItem>
#include <QImage>
#include <memory>
#include <mutex>
#include "IVideoCall.h"

struct SwsContext;

class XVideoOutput : public QQuickPaintedItem, public IVideoCall
{
    Q_OBJECT
public:
    explicit XVideoOutput(void);
    ~XVideoOutput();
public:
    virtual void Init(int width, int height);
    virtual void Repaint(AVFrame *frame);
signals:
    void requestUpdate();
public slots:
    void procUpdate();
protected:
    virtual void paint(QPainter *pPainter);
private:
    std::mutex  mux;
    QImage m_oVideoFrame;
    SwsContext*  m_pSwsContext{nullptr};
    uint8_t *m_pRGBBuffer{nullptr};

};

#endif // XVIDEOOUTPUT_H
