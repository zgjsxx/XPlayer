#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H


#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XAudioThread.h"
#include "XVideoThread.h"
class XDemux;

class XDemuxThread : public QThread
{
public:
    XDemuxThread();
    virtual ~XDemuxThread();

public:
    virtual bool Open(const char *url, IVideoCall *call);
    virtual void Start();

    virtual void Close();
    virtual void Clear();

    virtual void Seek(double pos);
    void run();
    void SetPause(bool isPause);
    void SetVolume(double newVolume);
    void setOpenSuccess(bool value);
    bool getOpenSuccess();
    void clearVideoQueue();
    void clearAudioQueue();

public:
    bool isExit = false;
    long long pts = 0;
    long long totalMs = 0;
    bool isPause = false;

private:
    std::mutex mux;
    XDemux *m_pDemux = 0;
    bool openSuccess = false;
    XAudioThread *at = 0;
    XVideoThread *vt = 0;

};
#endif // XDEMUXTHREAD_H
