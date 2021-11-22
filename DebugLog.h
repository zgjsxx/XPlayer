#ifndef DEBUGLOG_H
#define DEBUGLOG_H
#include <iostream>
#include <QThread>
#define LOG_DBG  std::cout << "<@" <<__FUNCTION__<<"@@" <<__FILE__<<":"<<__LINE__<<">" \
          << "<" << getpid() << ":" << QThread::currentThread() << ">"

#endif // DEBUGLOG_H
