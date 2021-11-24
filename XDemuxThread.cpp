#include <iostream>
#include <QDebug>
extern "C"
{
    #include <libavformat/avformat.h>
}
#include <iostream>
#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "DebugLog.h"

XDemuxThread::XDemuxThread()
{
    LOG_DBG << "XDemuxThread is created" << std::endl;
}

XDemuxThread::~XDemuxThread()
{
    isExit = true;
    wait();
}


void XDemuxThread::Start()
{
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
        LOG_DBG << "new at" << std::endl;
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
    qDebug()<<newVolume;
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
    LOG_DBG << "XDemuxThread start" << std::endl;
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
                 at->Push(pkt);
             }
         }
         else
         {
             //video pkt
             if (vt)
             {
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

    //暂停
    SetPause(true);
    mux.lock();
    if (m_pDemux)
    {
        m_pDemux->Seek(pos);
    }

    mux.unlock();
    //实际要显示的位置pts
    long long seekPts = pos * m_pDemux->m_totalMs;

    while (!isExit)
    {
        AVPacket *pkt = m_pDemux->ReadVideo();
        if (!pkt)
        {
            LOG_DBG << "No seek packet" << std::endl;
            break;
        }

        //如果解码到seekPts
        if (vt->RepaintPts(pkt, seekPts))
        {
            this->pts = seekPts;
            break;
        }
    }

    //seek是非暂停状态
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
        qDebug()<<"file url patch is None"<<endl;
        return false;
    }
    LOG_DBG << "url path is " << url << std::endl;

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
        LOG_DBG << "new at" << std::endl;
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
