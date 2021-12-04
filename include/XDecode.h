#ifndef XDECODE_H
#define XDECODE_H

#include <mutex>

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

extern void MYFreePacket(AVPacket **pkt);
extern void MYFreeFrame(AVFrame **frame);


class XDecode
{
public:
    XDecode();
    virtual ~XDecode();

public:
    virtual bool Open(AVCodecParameters *para);
    virtual bool Send(AVPacket *pkt);
    virtual AVFrame *Recv();
    virtual void Close();
    virtual void Clear();

public:
    bool isAudio = false;
    long long pts = 0;
    AVCodecContext *codec = 0;

private:
    std::mutex mux;
};
#endif // XDECODE_H
