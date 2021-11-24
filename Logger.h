#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <iostream>
#include <QThread>

//https://blog.csdn.net/xiaoyink/article/details/79721481
//https://github.com/jaredtao/TaoLogger
namespace Logger
{

#define LOG_DEBUG qDebug() << "<@" <<__FUNCTION__<<"@@" <<__FILE__<<":"<<__LINE__<<">" << "<" << getpid() << ":" << QThread::currentThread() << ">"
#define LOG_INFO  qInfo() << "<@" <<__FUNCTION__<<"@@" <<__FILE__<<":"<<__LINE__<<">" << "<" << getpid() << ":" << QThread::currentThread() << ">"
#define LOG_WARN  qWarning() << "<@" <<__FUNCTION__<<"@@" <<__FILE__<<":"<<__LINE__<<">" << "<" << getpid() << ":" << QThread::currentThread() << ">"
#define LOG_CRIT  qCritical() << "<@" <<__FUNCTION__<<"@@" <<__FILE__<<":"<<__LINE__<<">" << "<" << getpid() << ":" << QThread::currentThread() << ">"

void initLog(const QString &logPath = QStringLiteral("Log"), int logMaxCount = 1024, bool async = true);

} // namespace Logger
#endif // LOGGER_H
