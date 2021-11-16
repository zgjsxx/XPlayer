#ifndef XPLAY_H
#define XPLAY_H
#include <QObject>
class XPlay : public QObject
{
    Q_OBJECT

    //Q_PROPERTY(QString backGround READ backGround WRITE setBackGround NOTIFY backGroundChanged)

public:
    XPlay();
    ~XPlay();

public:
    QString backGround() const;
    void setBackGround(QString url);

private:
    QString  m_strBackgroundPath{""};
};

#endif // XPLAY_H
