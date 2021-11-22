#ifndef XSUBTITLE_H
#define XSUBTITLE_H

#include <QObject>
#include <QPointer>
#include <QFile>


class XSubTitle : public QObject
{
    Q_OBJECT
 public:
    XSubTitle();
    ~XSubTitle();
public:
    void Open(QString url);
    bool Read(int currentTime);

    bool isOpen();
    int nodeStartTime();
    int nodeEndTime();

    QString text();
    enum SUBTITLE_TYPE{
        SUBTITLE_TYPE_UNKNOW = 0,
        SUBTITLE_TYPE_SRT,
        SUBTITLE_TYPPE_SSA      //SSA或ASS
        //...
    };
    Q_ENUM(SUBTITLE_TYPE)
private:

    SUBTITLE_TYPE  m_mSubType{ SUBTITLE_TYPE_UNKNOW };
    QScopedPointer<QFile>    m_mSubTitleFile;
    int    m_mNodeStartTime{ 0 };
    int    m_mNodeEndTime{ 0 };
    QString     m_mText{ "" };
    bool     m_mIsOpen{ false };
};

#endif // XSUBTITLE_H
