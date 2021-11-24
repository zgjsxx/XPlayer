#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

#include <QThread>

struct AVPacket;
class XDecode;

class XDecodeThread : public QThread
{
public:
    XDecodeThread(std::string name);
    ~XDecodeThread();
public:
    virtual void Push(AVPacket *pkt);
    virtual void Clear();
    virtual void Close();
    virtual AVPacket *Pop();
public:
    const int MAXLISTSIZE = 100;
    bool isExit = false;
    std::string m_name;
protected:
    XDecode *m_pDecode = 0;
    std::list<AVPacket *> m_packs;
    std::mutex mux;
};
#endif // XDECODETHREAD_H

