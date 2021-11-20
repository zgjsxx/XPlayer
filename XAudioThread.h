#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H

#include <mutex>
#include "XDecodeThread.h"

struct AVCodecParameters;
class XAudioPlay;
class XResample;

class XAudioThread : public XDecodeThread
{
public:
    XAudioThread();
    virtual ~XAudioThread();
public:
    virtual bool Open(AVCodecParameters *para, int sampleRate, int channels);
    virtual void Close();
    virtual void Clear();
    void run();
    void SetPause(bool pause);
    void SetVolume(double newVolume);
public:
    bool isPause = false;
    long long pts = 0;

protected:
    std::mutex amux;
    XAudioPlay *m_pAudioPlay;
    XResample *m_pResample;


};

#endif // XAUDIOTHREAD_H
