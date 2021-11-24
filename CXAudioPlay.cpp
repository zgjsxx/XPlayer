#include "CXAudioPlay.h"
#include "DebugLog.h"
void CXAudioPlay::SetVolume(double newVolume)
{
    // Definition: QAudioOutput *output = NULL;
    if(!output)
    {
        return;
    }
    /*
     * void QAudioOutput::setVolume(qreal volume)
     * Sets the output volume to volume.
     * The volume is scaled linearly from 0.0 (silence) to 1.0 (full volume). Values outside this range will be clamped.
    */
    output->setVolume(newVolume);
}


/*
 * CXAudioPlay::GetNoPlayMs()
 * function: get the time of the no play audio
*/
long long CXAudioPlay::GetNoPlayMs()
{
    mux.lock();
    if(!output)
    {
        mux.unlock();
        return 0;
    }
    long long pts = 0;
    //whole buffer size - free bytes = used bytes
    //it means the bytes that has not been played
    double size = output->bufferSize() - output->bytesFree();
    //change it to KB/s,because sampleSize unit is bit
    double sizePerSec = sampleRate * (sampleSize/8) * channels;
    if(sizePerSec <= 0)
    {
        pts = 0;
    }
    else
    {
        pts = (size/sizePerSec) * 1000;
    }
    mux.unlock();
    return pts;
}

void CXAudioPlay::Clear()
{
    mux.lock();
    //Definition: QIODevice *io = NULL;
    if(io)
    {
        io->reset();
    }
    mux.unlock();
}

void CXAudioPlay::Close()
{
    mux.lock();
    if(io)
    {
        io->close();
        io = nullptr;
    }
    if(output)
    {
        output->stop();
        delete output;
        output = nullptr;
    }
    mux.unlock();
}

bool CXAudioPlay::Open()
{
    LOG_DBG << "CXAudioPlay::Open()" << std::endl;
    Close();
    LOG_DBG << "CXAudioPlay::close()" << std::endl;
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setSampleSize(sampleSize);
    fmt.setChannelCount(channels);
    fmt.setCodec("audio/pcm");
    LOG_DBG << "CXAudioPlay::Open2()" << std::endl;
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);
    mux.lock();
    output = new QAudioOutput(fmt);
    io = output->start();
    mux.unlock();
    if(io)
    {
        return true;
    }
    return false;
}

//暂停
void CXAudioPlay::SetPause(bool isPause)
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return;
    }
    if (isPause)
    {
        output->suspend();
    }
    else
    {
        output->resume();
    }
    mux.unlock();
}

//播放音频
bool CXAudioPlay::Write(const unsigned char *data, int datasize)
{
    if (!data || datasize <= 0)return false;
    mux.lock();
    if (!output || !io)
    {
        mux.unlock();
        return false;
    }
    int size = io->write((char *)data, datasize);
    mux.unlock();
    if (datasize != size)
        return false;
    return true;
}

int CXAudioPlay::GetFree()
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return 0;
    }
    int free = output->bytesFree();
    mux.unlock();
    return free;
}








