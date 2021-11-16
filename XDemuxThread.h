#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H


#include <mutex>
#include <QThread>
#include "IVideoCall.h"

class XDemux;

class XDemuxThread : public QThread
{
    virtual bool Open(const char *url, IVideoCall *call);
    virtual void Start();

    virtual void Close();
    virtual void Clear();

    virtual void Seek(double pos);
    void run();
    XDemuxThread();
    virtual ~XDemuxThread();
    bool isExit = false;
    long long pts = 0;
    long long totalMs = 0;

protected:
    std::mutex mux;
    XDemux *m_pDemux = 0;
//    XVideoThread *vt = 0;
//    XYAudioThread *at = 0;
//    bool openSuccess = false;

};
#endif // XDEMUXTHREAD_H
