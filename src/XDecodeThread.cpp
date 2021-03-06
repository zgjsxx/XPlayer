#include "XDecode.h"
#include "XDecodeThread.h"
#include "Logger.h"

XDecodeThread::XDecodeThread(std::string name)
{
    m_name = name;
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
    while (!m_packs.empty())
    {
        AVPacket *pkt = m_packs.front();
        MYFreePacket(&pkt);
        m_packs.pop_front();
    }
    mux.unlock();
}

AVPacket *XDecodeThread::Pop()
{
    mux.lock();
    if (m_packs.empty())
    {
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = m_packs.front();
    m_packs.pop_front();
    mux.unlock();
    return pkt;
}

void XDecodeThread::Push(AVPacket *pkt)
{

    if (!pkt)
    {
        LOG_DEBUG << "No packet to push" ;
        return;
    }
    //push the packet into queue
    while (!isExit)
    {
        mux.lock();
        if (m_packs.size() < MAXLISTSIZE)
        {
            m_packs.push_back(pkt);
            mux.unlock();
            break;
        }
        else
        {
            //LOG_DEBUG << QString::fromStdString(m_name)  << " queue is full";
        }

        mux.unlock();
        msleep(1);
    }
}

void XDecodeThread::clearPacket()
{
    mux.lock();
    m_packs.clear();
    mux.unlock();
}
