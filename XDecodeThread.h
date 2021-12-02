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
    void clearPacket();
public:
    const int MAXLISTSIZE = 200;
    bool isExit = false;
    std::string m_name;
    XDecode *m_pDecode = 0;
protected:
    std::list<AVPacket *> m_packs;
    std::mutex mux;
};
#endif // XDECODETHREAD_H

