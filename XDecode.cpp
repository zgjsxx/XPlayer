#include <iostream>
extern "C"
{
    #include<libavcodec/avcodec.h>
}
#include "XDecode.h"

void MYFreePacket(AVPacket **pkt)
{
    if (!pkt || !(*pkt))return;
    av_packet_free(pkt);
}
void MYFreeFrame(AVFrame **frame)
{
    if (!frame || !(*frame))return;
    av_frame_free(frame);
}

void XDecode::Close()
{
    mux.lock();
    if(codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    pts = 0;
    mux.unlock();
}

void XDecode::Clear()
{

}

bool XDecode::Open(AVCodecParameters *param)
{
    if(!param)
    {
        return false;
    }
    Close();
    //find the video decoder
    AVCodec *vcodec = avcodec_find_decoder(param->codec_id);
    if(!vcodec)
    {
        avcodec_parameters_free(&param);
        std::cout << "can't find the codec id " << param->codec_id << std::endl;
        return false;
    }
    mux.lock();

    //create decoder context
    codec = avcodec_alloc_context3(vcodec);

    //create decoder context parameter
    avcodec_parameters_to_context(codec, param);
    avcodec_parameters_free(&param);

    //decode with 8 thread
    codec->thread_count = 8;
    //open decode context
     int re = avcodec_open2(codec, 0, 0);
     if (re != 0)
     {
         avcodec_free_context(&codec);
         mux.unlock();
         char buf[1024] = { 0 };
         av_strerror(re, buf, sizeof(buf) - 1);
         std::cout << "<XDecode:Open> avcodec_open2  failed! :" << buf << std::endl;
         return false;
     }
     mux.unlock();
     std::cout << " avcodec_open2 success!" << std::endl;
     return true;
}

bool XDecode::Send(AVPacket *pkt)
{
    if(!pkt || pkt->size <= 0 || !pkt->data)
    {
        return false;
    }
    mux.lock();
    if(!codec)
    {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec,pkt);
    mux.unlock();
    av_packet_free(&pkt);
    if(re != 0)
    {
        return false;
    }
    return true;
}

AVFrame* XDecode::Recv()
{
    mux.lock();
    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec, frame);
    mux.unlock();
    if (re != 0)
    {
        av_frame_free(&frame);
        return NULL;
    }
    //cout << "["<<frame->linesize[0] << "] " << flush;
    pts = frame->pts;
    return frame;
}

XDecode::XDecode()
{

}

XDecode::~XDecode()
{

}
