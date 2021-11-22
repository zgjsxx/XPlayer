#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

#include <QThread>

struct AVPacket;
class XDecode;

class XDecodeThread : public QThread
{
public:
    XDecodeThread();
    ~XDecodeThread();
public:
    virtual void Push(AVPacket *pkt);
    virtual void Clear();
    virtual void Close();
    virtual AVPacket *Pop();
public:
    int maxList = 100;
    bool isExit = false;
protected:
    XDecode *m_pDecode = 0;
    std::list<AVPacket *> packs;
    std::mutex mux;
};
#endif // XDECODETHREAD_H

