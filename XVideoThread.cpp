#include <iostream>
#include "XVideoThread.h"
#include "XDecode.h"
#include "DebugLog.h"
using namespace std;

XVideoThread::XVideoThread():
    XDecodeThread("video_thread")
{
    LOG_DBG << "initial XVideoThread" << std::endl;
}


XVideoThread::~XVideoThread()
{

}

//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if (!para)
    {
        LOG_DBG << "No Param" << std::endl;
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
        LOG_DBG << "audio MYDecode open failed!" << std::endl;
        re = false;
    }

    LOG_DBG << "XVideoThread::Open :" << re << std::endl;
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
    LOG_DBG << "XVideoThread start" << std::endl;
    while (!isExit)
    {
        vmux.lock();

        if (this->isPause)
        {
            //LOG_DBG << "Video Thead is pause" << std::endl;
            vmux.unlock();
            msleep(5);
            continue;
        }

        //音视频同步
        if (synpts >0 && synpts < m_pDecode->pts)
        {
            //音频落后于视频
            //LOG_DBG << "synpts is " << synpts << std::endl;
            //LOG_DBG << "m_pDecode->pts is " << m_pDecode->pts << std::endl;
            vmux.unlock();
            msleep(1);
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
            //显示视频
            if (call)
            {
                call->Repaint(frame);
            }

        }
        vmux.unlock();
    }
}

//解码pts，如果接受到的解码数据pts > seekpts 返回true  并且显示画面
bool XVideoThread::RepaintPts(AVPacket *pkt, long long seekpts)
{
    vmux.lock();
    bool re = m_pDecode->Send(pkt);
    if (!re)
    {
        LOG_DBG << "decode finish" << std::endl;
        vmux.unlock();
        return true;   //表示结束解码
    }
    AVFrame *frame = m_pDecode->Recv();
    if (!frame)
    {
        vmux.unlock();
        return false;
    }


    LOG_DBG << "m_pDecode->pts =" << m_pDecode->pts << std::endl;
    LOG_DBG << "seekpts =" << seekpts << std::endl;
//    //到达位置
//    if (m_pDecode->pts >= seekpts)
//    {
//        if (call)
//        {
//            call->Repaint(frame);
//        }
//        LOG_DBG << "Repaint suc" << std::endl;
//        vmux.unlock();
//        return true;
//    }


    MYFreeFrame(&frame);
    vmux.unlock();
    return false;
}


