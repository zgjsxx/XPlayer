#include <iostream>
#include "XAudioThread.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include "XDecode.h"
#include "DebugLog.h"
using namespace std;

XAudioThread::XAudioThread():
    m_pResample(nullptr),
    m_pAudioPlay(nullptr),
    XDecodeThread("audio_thread")
{
    LOG_DBG << "initial XAudioThread" << std::endl;
    if (m_pResample == nullptr)
    {
        LOG_DBG << "create Resample object" << std::endl;
        m_pResample = new XResample();
    }
    else
    {
        LOG_DBG << "Resample object is not null" << std::endl;
    }

    if (!m_pAudioPlay)
    {
        m_pAudioPlay = XAudioPlay::Get();
    }
}


XAudioThread::~XAudioThread()
{
    //等待线程退出
    isExit = true;
    delete m_pResample;
    m_pResample = nullptr;
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
    if(m_pAudioPlay)
    {
        m_pAudioPlay->Close();
        amux.lock();
        delete m_pAudioPlay;
        m_pAudioPlay = nullptr;
        amux.unlock();
    }
}

bool XAudioThread::Open(AVCodecParameters *para, int sampleRate, int channels)
{
    LOG_DBG << "XAudioThread Open" << std::endl;
    if(!para)
    {
        LOG_DBG << "No Para" << std::endl;
        return false;
    }
    amux.lock();
    pts = 0;
    bool re = true;

    if(m_pResample == nullptr)
    {
        LOG_DBG << "m_pResample is nullptr" << std::endl;
    }
    if(!m_pResample->Open(para,false))
    {
        LOG_DBG << "XResample open failed!" << std::endl;
        re = false;
    }
    LOG_DBG << "XResample open success" << std::endl;

    m_pAudioPlay->sampleRate = sampleRate;
    m_pAudioPlay->channels = channels;
    if(!m_pAudioPlay->Open())
    {
        re = false;
        cout << "XAudioPlay open failed!" << endl;
    }

    LOG_DBG << "m_pAudioPlay open success" << std::endl;
    if (!m_pDecode->Open(para))
    {
        cout << "Audio Decoder open failed!" << endl;
        re = false;
    }
    LOG_DBG << "m_pDecode open success" << std::endl;
    amux.unlock();

    LOG_DBG << "Audio decode open success! : " << re << endl;
    return re;
}
//暂停
void XAudioThread::SetPause(bool isPause)
{
    //amux.lock();
    this->isPause = isPause;
    if (m_pAudioPlay)
        m_pAudioPlay->SetPause(isPause);
    //amux.unlock();
}

//改变音量
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
    LOG_DBG << "XAudioThread start" << std::endl;
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
        if(pkt == nullptr)
        {
            //LOG_DBG << "pkt is null" << std::endl;
        }
        bool re = m_pDecode->Send(pkt);
        if (!re)
        {
            //LOG_DBG << "no data" << std::endl;
            amux.unlock();
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame * frame = m_pDecode->Recv();
            if (!frame)
            {
                //LOG_DBG << "break" <<std::endl;
                break;
            }

            //减去缓冲中未播放的时间
            pts = m_pDecode->pts - m_pAudioPlay->GetNoPlayMs();
            //LOG_DBG << "audio pts = " << pts << endl;
            //重采样
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

