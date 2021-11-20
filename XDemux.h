#ifndef XDEMUX_H
#define XDEMUX_H

#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
    XDemux();
    virtual ~XDemux();

public:
    virtual bool Open(const char *url);

    virtual AVPacket *Read();

    virtual AVPacket *ReadVideo();

    virtual bool IsAudio(AVPacket *pkt);

    virtual AVCodecParameters *CopyVideoParam();

    virtual AVCodecParameters *CopyAudioParam();

    virtual bool Seek(double pos);

    virtual void Clear();
    virtual void Close();

public:
    //media total time (ms)
    int totalMs = 0;
    int width = 0;
    int height = 0;
    int sampleRate = 0;
    int channels = 0;
protected:
    std::mutex mux;
    AVFormatContext *ic = NULL;
    //video and audio mapping
    int m_videoStreamIndex = 0;
    int m_audioStreamIndex = 1;
};

#endif // XDEMUX_H
