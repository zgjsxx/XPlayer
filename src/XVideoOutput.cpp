#include "XVideoOutput.h"
#include <QPainter>
#include <QTimer>
#include <Logger.h>
#include <stdexcept>
extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libavformat/avformat.h>
}

XVideoOutput::XVideoOutput(void)
             : m_oVideoFrame(),
               m_pRGBBuffer(nullptr)
{
    QObject::connect(this, &XVideoOutput::requestUpdate,
                     this, &XVideoOutput::procUpdate);
}

XVideoOutput::~XVideoOutput()
{
    sws_freeContext(m_pSwsContext);
    m_pSwsContext = nullptr;
}

void XVideoOutput::Repaint(AVFrame *frame)
{
    if (!frame)
    {
        return;
    }

    // Convert YUV420P -> RGB32 by libswscale
    if ((nullptr == m_pSwsContext)
        || (m_oVideoFrame.width() != frame->width)
        || (m_oVideoFrame.height() != frame->height))
    {
        sws_freeContext(m_pSwsContext);
        m_pSwsContext = nullptr;
        m_pSwsContext = sws_getContext(frame->width,
                                       frame->height,
                                       static_cast<AVPixelFormat>(frame->format),
                                       frame->width,
                                       frame->height,
                                       AV_PIX_FMT_BGRA,
                                       SWS_FAST_BILINEAR, NULL, NULL, NULL);
    }
    int linesize[8] = {frame->width * 4};
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_BGRA, frame->width, frame->height, 1);

    this->m_pRGBBuffer = new uint8_t[num_bytes];

    uint8_t* szRGBBuffer[8] = {m_pRGBBuffer};
    sws_scale(m_pSwsContext,
              frame->data,
              frame->linesize,
              0,
              frame->height,
              szRGBBuffer,
              linesize);

    mux.lock();
    m_oVideoFrame = QImage(this->m_pRGBBuffer,
                           frame->width,
                           frame->height,
                           QImage::QImage::Format_ARGB32,
                           [](void *info)->void
                                {
                                    //clean up function
                                    uint8_t* p = static_cast<uint8_t*>(info);
                                    delete[] p;
                                },
                            (void*)this->m_pRGBBuffer);

    mux.unlock();
    av_frame_free(&frame);

    emit requestUpdate();
}
void XVideoOutput::paint(QPainter *pPainter)
{
//    static QImage *image = NULL;

//    static int w = 0;
//    static int h = 0;
//    if(w != width() || h != height())
//    {
//        //???????????????????????????????????????
//        if(image)
//        {
//            delete image->bits();  //??????QImage???????????????
//            delete image;
//            image = NULL;
//        }
//    }

//    if (image == NULL)
//    {
//        uchar *buf = new uchar[width() * height() * 4];
//        image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
//    }
//    image->save("D:\c++\ffmpeg_qt\bin\background\1.jpg","JPG",50);
//    pPainter->drawImage(QPoint(0,0),*image);

    // Draw background
    pPainter->fillRect(0, 0, width(), height(), Qt::black);

    // Draw video frame
    mux.lock();
    if (!m_oVideoFrame.isNull())
    {
        QRectF target(0, 0, width(), height());
        QRectF source(m_oVideoFrame.rect());
        pPainter->drawImage(target, m_oVideoFrame, source);
    }

    mux.unlock();
}
void XVideoOutput::Init(int width, int height)
{
//    mux.lock();
//    mux.unlock();
}

void XVideoOutput::procUpdate()
{
    update();
}
