#include <iostream>
extern "C"
{
    #include <libavformat/avformat.h>
}

#include "XDemuxThread.h"
#include "XDemux.h"



void XDemuxThread::Start()
{
    mux.lock();
    if(!m_pDemux)
    {
        m_pDemux = new XDemux();
    }

    mux.unlock();
}

void XDemuxThread::Clear()
{
    mux.lock();
    if (m_pDemux)
    {
       m_pDemux->Clear();
    }
//    if (vt)vt->Clear();
//    if (at)at->Clear();
    mux.unlock();
}

void XDemuxThread::Seek(double pos)
{

}

void XDemuxThread::run()
{

}

bool XDemuxThread::Open(const char *url, IVideoCall *call)
{

}

void XDemuxThread::Close()
{

}

XDemuxThread::XDemuxThread()
{

}

XDemuxThread::~XDemuxThread()
{
    isExit = true;
}
