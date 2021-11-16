#ifndef XDEMUX_H
#define XDEMUX_H

#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
    virtual bool Open(const char *url);

    virtual AVPacket *Read();

    virtual AVPacket *ReadVideo();

    virtual bool Seek(double pos);

    virtual void Clear();
    virtual void Close();
public:
    XDemux();
    virtual ~XDemux();

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
    int videoStream = 0;
    int audioStream = 1;
};

#endif // XDEMUX_H
