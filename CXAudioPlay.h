#ifndef CXAUDIOPLAY_H
#define CXAUDIOPLAY_H

#include <mutex>
#include <QAudioFormat>
#include <QAudioOutput>
#include "XAudioPlay.h"

class CXAudioPlay : public XAudioPlay
{
public:
    virtual void SetVolume(double newVolume) override;
    virtual long long GetNoPlayMs() override;
    virtual void Clear() override;
    virtual void Close() override;
    virtual bool Open() override;
    virtual void SetPause(bool isPause) override;
    virtual bool Write(const unsigned char *data, int datasize) override;
    virtual int GetFree() override;
public:
    QAudioOutput *output = NULL;
    QIODevice *io = NULL;
    std::mutex mux;
};

#endif // CXAUDIOPLAY_H
