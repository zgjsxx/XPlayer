#ifndef XVIDEOTHEAD_H
#define XVIDEOTHEAD_H
#include "XDecodeThread.h"
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"

struct AVPacket;
struct AVCodecParameters;
class MYDecode;


class XVideoThread : public XDecodeThread
{
public:
    XVideoThread();
    virtual ~XVideoThread();
public:
    void SetPause(bool isPause);
    virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);
    void run();
    void revisePts(long long pts);
public:
    bool isPause = false;
    //同步时间，由外部传入
    long long synpts = 0;
    std::mutex vmux;
    bool m_bIsFirstSeekFrame = false;
protected:
    IVideoCall *call = 0;

};

#endif // XVIDEOTHEAD_H
