#include <iostream>
#include "XVideoThread.h"
#include "XDecode.h"
#include "Logger.h"
extern "C"
{
    #include<libavcodec/avcodec.h>
}

using namespace std;

XVideoThread::XVideoThread():
    XDecodeThread("video_thread")
{
    LOG_INFO << "Initial XVideoThread";
}


XVideoThread::~XVideoThread()
{
    LOG_INFO << "Destroy XVideoThread";
}

//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if (!para)
    {
        LOG_WARN << "No video param";
        return false;
    }

    Clear();

    vmux.lock();
    //synpts = 0;
    //初始化显示窗口
    this->call = call;
    if (call)
    {
        //call->Init(width, height);
    }
    vmux.unlock();
    int re = true;
    if (!m_pDecode->Open(para))
    {
        LOG_WARN << "Video thread decoder open failed";
        re = false;
    }

    return re;
}
void XVideoThread::SetPause(bool isPause)
{
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}

void XVideoThread::run()
{
    LOG_DEBUG << "XVideoThread start";
    while (!isExit)
    {
        vmux.lock();

        if (this->isPause)
        {
            vmux.unlock();
            msleep(5);
            continue;
        }

        //音视频同步
        if (synpts >0 && synpts < m_pDecode->pts && !m_bIsFirstSeekFrame)
        {
            //the audio is behind the video
            //if the m_bIsFirstSeekFrame is true, not sync with audio,because the video pts is not update now
            vmux.unlock();
            msleep(10);
            continue;
        }

        AVPacket *pkt = Pop();
        bool re = m_pDecode->Send(pkt);
        if (!re)
        {
            //No packet
            vmux.unlock();
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame * frame = m_pDecode->Recv();
            if (!frame)
            {
                break;
            }
            //
            m_bIsFirstSeekFrame = false;
            //显示视频
            if (call)
            {
                call->Repaint(frame);
            }

        }
        vmux.unlock();
    }
}

void XVideoThread::revisePts(long long pts)
{
    m_pDecode->pts = pts;
}
