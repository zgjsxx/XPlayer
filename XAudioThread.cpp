#include <iostream>
#include "XAudioThread.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include "XDecode.h"

using namespace std;

void XAudioThread::Clear()
{
    XDecodeThread::Clear();
    mux.lock();
    //XAudioPlay *m_pAudioPlay;
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
    if(!para)
    {
        return false;
    }
    amux.lock();
    pts = 0;
    bool re = true;
    if(!m_pResample->Open(para,false))
    {
        std::cout << "MYResample open failed!" << std::endl;
        re = false;
    }
    m_pAudioPlay->sampleRate = sampleRate;
    m_pAudioPlay->channels = channels;
    if(!m_pAudioPlay->Open())
    {
        re = false;
        cout << "MYAudioPlay open failed!" << endl;
    }
    if (!decode->Open(para))
    {
        cout << "Audio MYDecode open failed!" << endl;
        re = false;
    }
    amux.unlock();
    cout << "MYAudioThread::Open success! : " << re << endl;
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
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        amux.lock();
        if (isPause)
        {
            amux.unlock();
            msleep(5);
            continue;
        }
        //没有数据
        //if (packs.empty() || !decode || !res || !ap)
        //{
        //	amux.unlock();
        //	msleep(1);
        //	continue;
        //}

        //AVPacket *pkt = packs.front();
        //packs.pop_front();
        AVPacket *pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            amux.unlock();
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while (!isExit)
        {
            AVFrame * frame = decode->Recv();
            if (!frame) break;

            //减去缓冲中未播放的时间
            pts = decode->pts - m_pAudioPlay->GetNoPlayMs();

            //cout << "audio pts = " << pts << endl;

            //重采样
            int size = m_pResample->Resample(frame, pcm);
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

XAudioThread::XAudioThread()
{
    if (!m_pResample)
    {
        m_pResample = new XResample();
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
    wait();
}
