#include "XDecode.h"
#include "XDecodeThread.h"

XDecodeThread::XDecodeThread()
{
    //打开解码器
    if (!m_pDecode)
    {
        m_pDecode = new XDecode();
    }
}

XDecodeThread::~XDecodeThread()
{
    isExit = true;
    wait();
}

void XDecodeThread::Close()
{
    Clear();

    //等待线程结束
    isExit = true;
    wait();
    m_pDecode->Close();

    mux.lock();
    delete m_pDecode;
    m_pDecode = NULL;
    mux.unlock();
}

void XDecodeThread::Clear()
{
    mux.lock();
    m_pDecode->Clear();
    while (!packs.empty())
    {
        AVPacket *pkt = packs.front();
        MYFreePacket(&pkt);
        packs.pop_front();
    }
    mux.unlock();
}

AVPacket *XDecodeThread::Pop()
{
    mux.lock();
    if (packs.empty())
    {
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = packs.front();
    packs.pop_front();
    mux.unlock();
    return pkt;
}

void XDecodeThread::Push(AVPacket *pkt)
{
    if (!pkt)return;
    //进行阻塞——读取比解码快
    while (!isExit)
    {
        mux.lock();
        if (packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
}

