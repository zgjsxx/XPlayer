#include <iostream>
#include "XAudioThread.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include "XDecode.h"
#include "Logger.h"
using namespace std;

XAudioThread::XAudioThread():
    m_pResample(nullptr),
    m_pAudioPlay(nullptr),
    XDecodeThread("audio_thread")
{
    LOG_INFO << "Initial XAudioThread" ;
    if (m_pResample == nullptr)
    {
        LOG_DEBUG << "create Resample object" ;
        m_pResample = new XResample();
    }
    else
    {
        LOG_DEBUG << "Resample object is not null" ;
    }

    if (!m_pAudioPlay)
    {
        LOG_DEBUG << "create audioplay object";
        m_pAudioPlay = XAudioPlay::Get();
    }
}

XAudioThread::~XAudioThread()
{
    //wait thread to exit
    LOG_DEBUG << "exit audio thread";
    isExit = true;
    if(m_pResample != nullptr)
    {
        delete m_pResample;
        m_pResample = nullptr;
    }

    wait();
}

void XAudioThread::Clear()
{
    XDecodeThread::Clear();
    mux.lock();
    //Definition: XAudioPlay *m_pAudioPlay;
    if(m_pAudioPlay)
    {
        m_pAudioPlay->Clear();
    }
    mux.unlock();
}

void XAudioThread::Close()
{
    XDecodeThread::Close();
    //Definition: XResample *m_pResample;
    if(m_pResample)
    {
        m_pResample->Close();
        amux.lock();
        delete m_pResample;
        m_pResample = nullptr;
        amux.unlock();
    }
}

bool XAudioThread::Open(AVCodecParameters *para, int sampleRate, int channels)
{
    LOG_DEBUG << "XAudioThread Open" ;
    if(!para)
    {
        LOG_DEBUG << "No Para";
        return false;
    }
    amux.lock();
    pts = 0;
    bool re = true;

    if(m_pResample == nullptr)
    {
        LOG_DEBUG << "m_pResample is nullptr" ;
    }
    if(!m_pResample->Open(para,false))
    {
        LOG_DEBUG << "XResample open failed!" ;
        re = false;
    }
    LOG_DEBUG << "XResample open success" ;

    m_pAudioPlay->sampleRate = sampleRate;
    m_pAudioPlay->channels = channels;
    if(!m_pAudioPlay->Open())
    {
        re = false;
        cout << "XAudioPlay open failed!" << endl;
    }

    LOG_DEBUG << "m_pAudioPlay open success" ;
    if (!m_pDecode->Open(para))
    {
        cout << "Audio Decoder open failed!" << endl;
        re = false;
    }
    LOG_DEBUG << "m_pDecode open success" ;
    amux.unlock();

    LOG_DEBUG << "Audio decode open success! : " << re << endl;
    return re;
}
//pause
void XAudioThread::SetPause(bool isPause)
{
    //amux.lock();
    this->isPause = isPause;
    if (m_pAudioPlay)
    {
        m_pAudioPlay->SetPause(isPause);
    }
    //amux.unlock();
}

//change volume
void XAudioThread::SetVolume(double newVolume)
{
    if(!m_pAudioPlay)
    {
        return;
    }
    m_pAudioPlay->SetVolume(newVolume);
}

void XAudioThread::run()
{
    LOG_DEBUG << "XAudioThread start" ;
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        amux.lock();
        if (isPause)
        {
            //LOG_DBG << "audio is pause" << std::endl;
            amux.unlock();
            msleep(5);
            continue;
        }

        AVPacket *pkt = Pop();

        bool re = m_pDecode->Send(pkt);
        if (!re)
        {
            //LOG_DBG << "no data" << std::endl;
            amux.unlock();
            msleep(1);
            continue;
        }

        while (!isExit)
        {
            AVFrame * frame = m_pDecode->Recv();
            if (!frame)
            {
                break;
            }

            //减去缓冲中未播放的时间
            pts = m_pDecode->pts - m_pAudioPlay->GetNoPlayMs();
            //resample audio
            int size = m_pResample->Resample(frame, pcm);
            //LOG_DBG << "size = " << size << endl;
            //播放音频
            while (!isExit)
            {
                if (size <= 0)break;
                //缓冲未播完，空间不够
                if (m_pAudioPlay->GetFree() < size || isPause)
                {
                    msleep(1);
                    continue;
                }
                m_pAudioPlay->Write(pcm, size);
                break;
            }
        }
        amux.unlock();
    }
    delete[] pcm;
}

