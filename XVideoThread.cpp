#include <iostream>
#include "XVideoThread.h"
#include "XDecode.h"

using namespace std;

//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if (!para)return false;

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
    if (!decode->Open(para))
    {
        cout << "audio MYDecode open failed!" << endl;
        re = false;
    }

    cout << "MYAudioThread::Open :" << re << endl;
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
        if (synpts >0 && synpts < decode->pts)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }
        AVPacket *pkt = Pop();

        ////没有数据
        //if (packs.empty() || !decode)
        //{
        //	vmux.unlock();
        //	msleep(1);
        //	continue;
        //}
        //
        //AVPacket *pkt = packs.front();
        //packs.pop_front();
        bool re = decode->Send(pkt);
        if (!re)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame * frame = decode->Recv();
            if (!frame) break;
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
    bool re = decode->Send(pkt);
    if (!re)
    {
        vmux.unlock();
        return true;   //表示结束解码
    }
    AVFrame *frame = decode->Recv();
    if (!frame)
    {
        vmux.unlock();
        return false;
    }
    //到达位置
    if (decode->pts >= seekpts)
    {
        if (call)
            call->Repaint(frame);
        vmux.unlock();
        return true;
    }
    MYFreeFrame(&frame);
    vmux.unlock();
    return false;
}

XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{

}

