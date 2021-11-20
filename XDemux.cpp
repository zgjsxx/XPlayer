#include <iostream>
extern "C"
{
    #include <libavformat/avformat.h>
}
#include "XDemux.h"

using namespace std;

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

XDemux::XDemux()
{
    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if(isFirst)
    {
        av_register_all();
        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}

XDemux::~XDemux()
{
    //doing nothing now
}

bool XDemux::Open(const char *url)
{
    //close the before setting
    Close();

    //paramter setting
    AVDictionary *opts = NULL;

    //set rpsp tcp
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    //set max delay
    av_dict_set(&opts, "max_delay", "500", 0);

    int re = avformat_open_input(
        &ic,
        url,
        0,
        &opts
     );
    if(re != 0)
    {
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof (buf) - 1);
        cout << "open " << url << " failed! :" << buf << endl;
        return false;
    }

    //get the information of the stream
    re = avformat_find_stream_info(ic,0);
    this->totalMs = ic->duration/(AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;
    av_dump_format(ic,0,url,0);

    //获取视频流
     m_videoStreamIndex = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
     AVStream *as = ic->streams[m_videoStreamIndex];
     width = as->codecpar->width;
     height = as->codecpar->height;

     cout << "======================================================================" << endl;
     cout << m_videoStreamIndex << "视频信息" << endl;
     cout << "codec_id = " << as->codecpar->codec_id << endl;
     cout << "format = " << as->codecpar->format << endl;
     cout << "width=" << as->codecpar->width << endl;
     cout << "height=" << as->codecpar->height << endl;
     //帧率 fps 分数转换
     cout << "video fps = " << r2d(as->avg_frame_rate) << endl;

     cout << "======================================================================" << endl;
     cout << m_audioStreamIndex << "音频信息" << endl;
     //获取音频流
     m_audioStreamIndex = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
     as = ic->streams[m_audioStreamIndex];
     sampleRate = as->codecpar->sample_rate;
     channels = as->codecpar->channels;

     cout << "codec_id = " << as->codecpar->codec_id << endl;
     cout << "format = " << as->codecpar->format << endl;
     cout << "sample_rate = " << as->codecpar->sample_rate << endl;
     //AVSampleFormat
     cout << "channels = " << as->codecpar->channels << endl;
     //一帧数据：存放一定量样本数
     cout << "frame_size = " << as->codecpar->frame_size << endl;
     //fps = sample_rate/frame_size
     mux.unlock();


     return true;
}

bool XDemux::IsAudio(AVPacket *pkt)
{
    if(!pkt)
    {
        //pkt does not exist
        return false;
    }
    if(pkt->stream_index == m_videoStreamIndex)
    {
        return false;
    }
    return true;

}

AVCodecParameters *XDemux::CopyAudioParam()
{
    //parameter for decoding the audio
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[m_audioStreamIndex]->codecpar);
    mux.unlock();
    return pa;
}

AVCodecParameters *XDemux::CopyVideoParam()
{
    //parameter for decoding the video
    mux.lock();
    if(!ic)
    {
        mux.lock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[m_videoStreamIndex]->codecpar);
    mux.unlock();
    return pa;
}

void XDemux::Clear()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    //清理读取缓冲
    avformat_flush(ic);
    mux.unlock();
}

void XDemux::Close()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_close_input(&ic);
    //媒体总时长（毫秒）
    totalMs = 0;
    mux.unlock();
}

//seek 位置 pos 0.0 ~1.0
bool XDemux::Seek(double pos)
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return false;
    }
    //清理读取缓冲
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[m_videoStreamIndex]->duration * pos;
    int re = av_seek_frame(ic, m_videoStreamIndex, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    if (re < 0) {
        return false;
    }
    return true;
}

AVPacket *XDemux::Read()
{
    mux.lock();
    if (!ic) //容错
    {
        mux.unlock();
        return 0;
    }
    AVPacket *pkt = av_packet_alloc();
    //读取一帧，并分配空间
    int re = av_read_frame(ic, pkt);
    if (re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return 0;
    }
    //pts转换为毫秒
    pkt->pts = pkt->pts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    mux.unlock();
    //cout << pkt->pts << " "<<flush;
    return pkt;

}

AVPacket *XDemux::ReadVideo()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return 0;
    }
    mux.unlock();
    AVPacket *pkt = NULL;
    for(int i = 0;i < 20;i++)
    {
        pkt = Read();
        if(!pkt)
        {
            break;
        }
        if(pkt->stream_index == m_videoStreamIndex)
        {
            break;
        }
        av_packet_free(&pkt);
    }
    return pkt;

}

