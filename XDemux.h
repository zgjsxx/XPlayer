#ifndef XDEMUX_H
#define XDEMUX_H

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
    virtual bool Open(const char *url);

    virtual AVPacket *Read();

    virtual AVPacket *ReadVideo();

public:
    XDemux();
    virtual ~XDemux();

    //media total time (ms)
    int totalMs = 0;
    int width = 0;
    int height = 0;
    int sampleRate = 0;
    int channels = 0;

};

#endif // XDEMUX_H
