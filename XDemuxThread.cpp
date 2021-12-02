#include <iostream>
extern "C"
{
    #include <libavformat/avformat.h>
}
#include <iostream>
#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "Logger.h"
#include "XDecode.h"

XDemuxThread::XDemuxThread()
{
    LOG_INFO << "Initial XDemuxThread";
}

XDemuxThread::~XDemuxThread()
{
    LOG_INFO << "Destroy XDemuxThread";
    isExit = true;
    wait();
}

void XDemuxThread::Start()
{
    LOG_INFO << "start DemuxThread";
    mux.lock();
    if(!m_pDemux)
    {
        m_pDemux = new XDemux();
    }
    //Definition" XVideoThread *vt = 0;
    if(!vt)
    {
        vt = new XVideoThread();
    }

    //Definition MYAudioThread *at = 0;
    if(!at)
    {
        LOG_DEBUG << "new at" ;
        at = new XAudioThread();
    }
    QThread::start();
    if(vt)
    {
        vt->start();
    }
    if(at)
    {
        at->start();
    }

    mux.unlock();
}

void  XDemuxThread::SetVolume(double newVolume)
{
    //mux.lock();
    if(!at)
    {
        //mux.unlock();
        return;
    }
    //mux.unlock();
    LOG_DEBUG << "after change volume, volume is :" <<newVolume;
    at->SetVolume(newVolume);
}

void XDemuxThread::Clear()
{
    mux.lock();
    if (m_pDemux)
    {
        m_pDemux->Clear();
    }
    if (vt)
    {
        vt->Clear();
    }
    if (at)
    {
        at->Clear();
    }
    mux.unlock();
}

void XDemuxThread::run()
{
    LOG_DEBUG << "XDemuxThread start";
    while (!isExit)
     {
         mux.lock();
         if (isPause)
         {
             mux.unlock();
             msleep(5);
             continue;
         }
         if (!m_pDemux)
         {
             mux.unlock();
             msleep(5);
             continue;
         }

         //音视频同步
         if (vt && at)
         {
             pts = at->pts;
             //将audiothread的pts传入videothread
             vt->synpts = at->pts;
         }

         AVPacket *pkt = m_pDemux->Read();
         if (!pkt)
         {
             //LOG_DEBUG << "no pkt" ;
             mux.unlock();
             msleep(5);
             continue;
         }
         //判断数据是音频
         if (m_pDemux->IsAudio(pkt))
         {
             //audio pkt
             if (at)
             {
                 //LOG_DEBUG << "push audio";
                 at->Push(pkt);
             }
         }
         else
         {
             //video pkt
             if (vt)
             {
                 //LOG_DEBUG << "push video";
                 vt->Push(pkt);
             }
         }
         mux.unlock();
         msleep(1);
     }
}

void XDemuxThread::Seek(double pos)
{
    //清理缓冲
    Clear();
    mux.lock();
    bool status = this->isPause;
    mux.unlock();

    SetPause(true);
    mux.lock();
    if (m_pDemux)
    {
        m_pDemux->Seek(pos);
    }
    //flush the codec buffer
    if(openSuccess)
    {
        avcodec_flush_buffers(vt->m_pDecode->codec);
        avcodec_flush_buffers(at->m_pDecode->codec);
    }

    //remove the packet in the queue
    vt->clearPacket();
    at->clearPacket();

    vt->m_bIsFirstSeekFrame = true;
    LOG_DEBUG << "first video seek frame do not sync" ;

    mux.unlock();


    //if the current status is pause,set it to start
    if (!status)
    {
        SetPause(false);
    }

}
void XDemuxThread::SetPause(bool isPause)
{
    mux.lock();
    this->isPause = isPause;
    if (at)
    {
       at->SetPause(isPause);
    }
    if (vt)
    {
        vt->SetPause(isPause);
    }
    mux.unlock();
}

void XDemuxThread::setOpenSuccess(bool value)
{
    openSuccess = value;
}

bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
    if(url == 0 || url[0] == '\0')
    {
        LOG_DEBUG << "file url patch is None";
        return false;
    }
    LOG_DEBUG << "url path is " << url ;

    mux.lock();
    if(!m_pDemux){
        m_pDemux = new XDemux();
    }
    if(!vt)
    {
        vt = new XVideoThread();
    }
    if(!at)
    {
        LOG_DEBUG << "new at" ;
        at = new XAudioThread();
    }
    bool re = m_pDemux->Open(url);
    if(!re)
    {
        mux.unlock();
        std::cout<< "demux open failed" << std::endl;
    }
    setOpenSuccess(true);
    if (!vt->Open(m_pDemux->CopyVideoParam(), call, m_pDemux->m_width, m_pDemux->m_height))
    {
        re = false;
        std::cout << "vt->Open failed!" << std::endl;
    }
    //打开音频解码器和处理线程
    if (!at->Open(m_pDemux->CopyAudioParam(), m_pDemux->m_sampleRate, m_pDemux->m_channels))
    {
        re = false;
        std::cout << "at->Open failed!" << std::endl;
    }
    totalMs = m_pDemux->m_totalMs;
    mux.unlock();
    return re;

}

void XDemuxThread::Close()
{
    isExit = true;
    if(m_pDemux)
    {
        m_pDemux->Close();
    }
    if (vt)
    {
       vt->Close();
    }
    if (at)
    {
       at->Close();
    }
    mux.lock();
    QThread::quit();
    delete m_pDemux;
    delete vt;
    delete at;
    m_pDemux = nullptr;
    vt = nullptr;
    at = nullptr;
    openSuccess = false;
    mux.unlock();
}

bool XDemuxThread::getOpenSuccess()
{
    return openSuccess;
}
