#include <iostream>
#include "XVideoThread.h"
#include "XDecode.h"
#include "Logger.h"

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
        if (synpts >0 && synpts < m_pDecode->pts)
        {
            //音频落后于视频
            //LOG_DEBUG << "synpts is " << synpts ;
            //LOG_DEBUG << "m_pDecode->pts is " << m_pDecode->pts ;
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
        LOG_DEBUG << "decode finish" ;
        vmux.unlock();
        return true;   //表示结束解码
    }
    AVFrame *frame = m_pDecode->Recv();
    if (!frame)
    {
        vmux.unlock();
        return false;
    }


    //LOG_DEBUG << "m_pDecode->pts =" << m_pDecode->pts ;
    //LOG_DEBUG << "seekpts =" << seekpts ;
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
    return true;
}


