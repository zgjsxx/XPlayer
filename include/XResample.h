#ifndef XRESAMPLE_H
#define XRESAMPLE_H

#include <mutex>

struct AVCodecParameters;
struct AVFrame;
struct SwrContext;

class XResample
{
public:
    XResample();
    virtual ~XResample();

public:
    virtual bool Open(AVCodecParameters *para, bool isClearPara = false);
    virtual void Close();
    virtual int Resample(AVFrame *indata,unsigned char *data);

public:
    int m_outFormat = 1;

private:
    std::mutex mux;
    SwrContext *actx = nullptr;
};

#endif // XRESAMPLE_H
