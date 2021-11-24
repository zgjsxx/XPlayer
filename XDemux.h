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
    int m_totalMs = 0;
    int m_width = 0;
    int m_height = 0;
    int m_sampleRate = 0;
    int m_channels = 0;
protected:
    std::mutex mux;
    AVFormatContext *ic = nullptr;
    //video and audio mapping
    int m_videoStreamIndex = 0;
    int m_audioStreamIndex = 1;
};

#endif // XDEMUX_H
