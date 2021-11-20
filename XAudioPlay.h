#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H

class XAudioPlay
{
public:
    XAudioPlay();
    virtual ~XAudioPlay();
public:
    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;
    virtual long long GetNoPlayMs() = 0;
    virtual bool Write(const unsigned char *data, int datasize) = 0;
    virtual int GetFree() = 0;
    virtual void SetPause(bool isPause) = 0;
    virtual void SetVolume(double newVolume) = 0;
    static XAudioPlay *Get();
public:
    int sampleRate = 44100;
    int sampleSize = 16;
    int channels = 2;

};
#endif // XAUDIOPLAY_H
