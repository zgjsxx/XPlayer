#include "XAudioPlay.h"
#include "CXAudioPlay.h"

XAudioPlay::XAudioPlay()
{

}

XAudioPlay::~XAudioPlay()
{

}

XAudioPlay* XAudioPlay::Get()
{
    static CXAudioPlay play;
    return &play;
}
