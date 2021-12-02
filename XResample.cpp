#include <iostream>
extern "C"
{
    #include <libswresample/swresample.h>
    #include <libavcodec/avcodec.h>
}
#include "XResample.h"
#include "Logger.h"
using namespace std;


XResample::XResample()
{
    LOG_INFO << "Initial XResample";
}


XResample::~XResample()
{
    LOG_INFO << "Destroy XResample";
}

void XResample::Close()
{
    mux.lock();
    //Definition: SwrContext *actx = nullptr;
    if(actx)
    {
        swr_free(&actx);
    }
    mux.unlock();
}

//输出参数和输入参数一致（除了采样格式，其输出为S16）
bool XResample::Open(AVCodecParameters *para, bool isClearPara)
{
    LOG_DEBUG << "XResample::Open" ;
    if (!para)
    {
       return false;
    }
    mux.lock();

    //if actx is NULL, will assign space automatically

    //Pay attention to: the input layout channel should be same with out layout channel,
    //the 2nd paramter(av_get_default_channel_layout(para->channels)) and 5th parameter(av_get_default_channel_layout(para->channels))
    //if not same,for the audio will lose some info.

    //Definition : SwrContext *actx = nullptr;
    actx = swr_alloc_set_opts(actx,                     //resample context
        av_get_default_channel_layout(para->channels),	//output layout
        (AVSampleFormat)m_outFormat,	                //output format 1 AV_SAMPLE_FMT_S16
        para->sample_rate,					            //output sample rate
        av_get_default_channel_layout(para->channels),  //input layout
        (AVSampleFormat)para->format,                   //input format
        para->sample_rate,                              //input sample rate
        0,
        0
    );

    if (isClearPara)
    {
        avcodec_parameters_free(&para);
    }

    int re = swr_init(actx);
    mux.unlock();
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        LOG_WARN << "swr_init  failed! :" << buf;
        return false;
    }
    //unsigned char *pcm = NULL;
    return true;
}

//返回重采样后大小,不管成功与否都释放indata空间
int XResample::Resample(AVFrame *indata, unsigned char *d)
{
    if (!indata)
    {
        return 0;
    }
    if (!d)
    {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *data[2] = { 0 };
    data[0] = d;
    int re = swr_convert(actx,
        data, indata->nb_samples,		//输出
        (const uint8_t**)indata->data, indata->nb_samples	//输入
    );
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)m_outFormat);
    av_frame_free(&indata);
    if (re <= 0)
    {
        return re;
    }
    return outSize;
}

