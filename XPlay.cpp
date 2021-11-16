#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include "XPlay.h"

XPlay::XPlay()
{

}

XPlay::~XPlay()
{
    qDebug() << "close";
}

QString XPlay::backGround() const
{
    return m_strBackgroundPath;
}

//void XPlay::changeBackground(QString url)
//{
//    setBackGround(url);
//}

void XPlay::setBackGround(QString url)
{
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    QString backGroundPath = settingIni->value("Path/Background").toString();
    if(url == NULL)
    {
        if(backGroundPath == NULL)
        {
            delete settingIni;
            return;
        }
        else
        {
            m_strBackgroundPath = backGroundPath;
            QFileInfo fileInfo(backGroundPath);
        }
    }


}
