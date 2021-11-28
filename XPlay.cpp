#include "XPlay.h"
#include <QSettings>
#include <QFileInfo>
#include <QPointer>
#include "XDemuxThread.h"
#include "XVideoOutput.h"
#include "XSubTitle.h"
#include <iostream>
#include "Logger.h"
static QScopedPointer<XDemuxThread> dt;
static QScopedPointer<XSubTitle> st;

XPlay::XPlay()
{
    LOG_DEBUG << "start to initial XPlay" ;
    dt.reset(new XDemuxThread);
    st.reset(new XSubTitle);
    //demux thread start, also trigger starting audio thread and video thread
    dt->Start();
    startTimer(40);
    setBackGround(NULL);
    setVideoPath(NULL);
    setSubTitlePath(NULL);
}

XPlay::~XPlay()
{
    LOG_INFO<< "close XPlay";
    if(m_mOpenSuccess)
    {
        QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
        settingIni->setValue("Progress/" + m_strTheVideoPath,dt->pts);
        delete settingIni;
        LOG_INFO<< "close setting.ini";
    }
    dt->Close();
}

void XPlay::testFunc()
{
    setDemoNum(demoNum() + 1);
}

/*
 * @brief: open thread and transfer the file path
*/
void XPlay::urlPass(QString url)
{
    XVideoOutput* video =qobject_cast<XVideoOutput*>(source());
    if(video == nullptr)
    {
        LOG_DEBUG << "video is nullptr";
    }

    m_strTheVideoPath = url;
    if(video != nullptr)
    {
        QFileInfo fileInfo(url);
        QString subTitleP = url.left(url.length() - fileInfo.suffix().length());
        QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
        QString saveProgress = settingIni->value("Progress/" + url).toString();
        LOG_DEBUG << "Last play time is " << saveProgress ;
        delete settingIni;
        fileInfo = QFileInfo(subTitleP+"srt");     //打开视频时自动查找视频所在文件夹有对应名称的无字幕文件，有则打开
        if(fileInfo.exists())
        {
            st->Open(fileInfo.absoluteFilePath());
        }
        else
        {
            LOG_DEBUG << "there is no subtitle file in the dir";
        }

        //Definition: static QScopedPointer<XDemuxThread> dt;
        if(!dt->Open(url.toLocal8Bit(),video))
        {
            return;
        }

        if(saveProgress != "")
        {
            dt->Seek(saveProgress.toDouble() / (double)dt->totalMs);
        }

    }
    LOG_DEBUG << "open url path is " << url;
    m_totalPts = dt->totalMs;
    LOG_DEBUG << "m_totalPts is " << m_totalPts;
    setIsPlay(!dt->isPause);
    setOpenSuccess(dt->getOpenSuccess());
}

//play slider bar
void XPlay::posFind(double value)
{
    double pos = ((double)(int)(((double)dt->pts / (double)dt->totalMs) * 1000)) / 1000;
    double newValue = ((double)(int)(value * 1000)) / 1000;
    LOG_DEBUG << "pos value = " << pos ;
    dt->Seek(newValue);
}

void XPlay::timerEvent(QTimerEvent *e)
{
    //if (isSliderPress)return;
    setTestNum(m_mTestNum + 1);
    long long total = dt->totalMs;
    int currentTime = (int)dt->pts;
    if(st->isOpen())
    {
        if(!(st->nodeStartTime() <= currentTime && st->nodeEndTime() >= currentTime) || (st->nodeEndTime() == st->nodeStartTime()))   //如果已经读取到现在的节点则没必要在read，或者处于刚开始时需要执行read
            if(st->Read(currentTime))
                setSubTitleText(st->text());
            else
                setSubTitleText("");
    }
    if (total > 0)
    {
        double pos = ((double)(int)(((double)dt->pts / (double)total) * 1000)) / 1000;     //保留三位小数，后面置0，方便比较
        setPosNum(pos);
    }
}

/*
 * @brief: play or pause the video
*/
void XPlay::playOrPause()
{
    bool isPause = !dt->isPause;
    setIsPlay(!isPause);
    dt->SetPause(isPause);
}

/*
 * @brief: fast forward the video
*/
void XPlay::goOn()
{
    double pos = ((double)(int)(((double)dt->pts / (double)dt->totalMs) * 1000)) / 1000 + 0.02;   //依照需求调整快进的程度
    dt->Seek(pos);
}

/*
 * @brief: rewind the video
*/
void XPlay::goBack()
{
    double pos = ((double)(int)(((double)dt->pts / (double)dt->totalMs) * 1000)) / 1000 - 0.02;   //依照需求调整快退的程度
    dt->Seek(pos);
}

/*
 * @brief: change the background
*/
void XPlay::changeBackground(QString url)
{
    setBackGround(url);
}

/*
 * @brief: change the video path
*/
void XPlay::changeVideoPath(QString url)
{
    setVideoPath(url);
}

/*
 * @brief: change the subtitle path
*/
void XPlay::changeSubTitlePath(QString url)
{
    setSubTitlePath(url);
}

/*
 * @brief: change the volume path
*/
void XPlay::setVolume(double newVolume)
{
    dt->SetVolume(((double)(int)(newVolume*100))/100);
}

/*
 * @brief: stop the video
*/
void XPlay::setStop()
{
    if(m_mOpenSuccess)
    {
        dt->Close();
        dt.reset(new XDemuxThread);
        dt->Start();
        if(st->isOpen())
        {
            st.reset(new XSubTitle);
            setSubTitleText("");
        }
        setOpenSuccess(false);
    }
}

/*
 * @brief: load the subtitle
*/
void XPlay::loadSubTitle(QString url)
{
    if(m_mOpenSuccess)
    {
        st.reset(new XSubTitle);
        st->Open(url);
    }
}
// demoNum
int XPlay::demoNum() const
{
    return m_mDemoNum;
}

void XPlay::setDemoNum(int newValue)
{
    if (m_mDemoNum != newValue)
    {
        m_mDemoNum = newValue;
        emit demoNumChanged(m_mDemoNum);
    }
}

int XPlay::testNum() const
{
    return m_mTestNum;
}

void XPlay::setTestNum(int newValue)
{
    if (m_mTestNum != newValue)
    {
        m_mTestNum = newValue;
        emit testNumChanged(m_mTestNum);
    }
}

double XPlay::posNum() const
{
    return m_mPosNum;
}

void XPlay::setPosNum(double newValue)
{
    if (m_mPosNum != newValue)
    {
        m_mPosNum = newValue;
        emit posNumChanged(m_mPosNum);
    }
}

bool XPlay::isPlay() const
{
    return m_mIsPlaying;
}

void XPlay::setIsPlay(bool isPlaying)
{
    if(m_mIsPlaying != isPlaying)
    {
        m_mIsPlaying = isPlaying;
        emit isPlayingChanged(m_mIsPlaying);
    }
}

bool XPlay::openSuccess() const
{
    return m_mOpenSuccess;
}

void XPlay::setOpenSuccess(bool openSuccess)
{
    if(m_mOpenSuccess != openSuccess)
    {
        m_mOpenSuccess = openSuccess;
        emit openSuccessChanged(m_mOpenSuccess);
    }
}

QObject* XPlay::source() const
{
    return m_mSource;

}
void XPlay::setSource(QObject *source)
{
    if(m_mSource != source)
    {
        m_mSource = source;
        emit sourceChanged(m_mSource);
    }
}
//背景图片路径
QString XPlay::backGround() const
{
    return m_strBackgroundPath;
}
void XPlay::setBackGround(QString url)
{
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    QString backGroundpath = settingIni->value("Path/Background").toString();
    if(url == NULL)
    {
        if(backGroundpath == NULL)
        {
            delete settingIni;
            return;
        }
        else
        {
            m_strBackgroundPath = backGroundpath;
            QFileInfo fileInfo(backGroundpath);
            m_strBackGroundChoosePath = fileInfo.path();
            LOG_DEBUG <<m_strBackGroundChoosePath<<endl;
            delete settingIni;
            emit backGroundChanged(m_strBackgroundPath);
            emit backGroundChooseChanged(m_strBackGroundChoosePath);
            return;
        }
    }
    if(m_strBackgroundPath != url)
    {
        settingIni->setValue("Path/Background",url);
        m_strBackgroundPath = url;
        QFileInfo fileInfo(url);
        m_strBackGroundChoosePath = fileInfo.path();
        emit backGroundChanged(m_strBackgroundPath);
        emit backGroundChooseChanged(m_strBackGroundChoosePath);
    }
    delete settingIni;
}
//选择背景路径
QString XPlay::backGroundChoose() const
{
    return m_strBackGroundChoosePath;
}

//选择视频路径
QString XPlay::videoPath() const
{
    return m_strVideoPath;
}
void XPlay::setVideoPath(QString url)
{
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    QString videoPath = settingIni->value("Path/VideoChoose").toString();
    if(url == NULL)
    {
        if(videoPath == NULL)
        {
            delete settingIni;
            return;
        }
        else
        {
            m_strVideoPath = videoPath;
            delete settingIni;
            emit videoPathChanged(m_strVideoPath);
            return;
        }
    }
    if(m_strVideoPath != url)
    {
        QFileInfo fileInfo(url);
        m_strVideoPath = fileInfo.path();
        settingIni->setValue("Path/VideoChoose",m_strVideoPath);
        emit videoPathChanged(m_strVideoPath);
    }
    delete settingIni;
}
//选择字幕路径
QString XPlay::subTitlePath() const
{
    return m_strSubTitlePath;
}
void XPlay::setSubTitlePath(QString url)
{
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    QString subTitlePath = settingIni->value("Path/SubTitleChoose").toString();
    if(url == NULL)
    {
        if(subTitlePath == NULL)
        {
            delete settingIni;
            return;
        }
        else
        {
            m_strSubTitlePath = subTitlePath;
            delete settingIni;
            emit subTitlePathChanged(m_strSubTitlePath);
            return;
        }
    }
    if(m_strSubTitlePath != url)
    {
        QFileInfo fileInfo(url);
        m_strSubTitlePath = fileInfo.path();
        settingIni->setValue("Path/SubTitleChoose",m_strSubTitlePath);
        emit subTitlePathChanged(m_strSubTitlePath);
    }
    delete settingIni;
}

//字幕文本
QString XPlay::subTitleText() const
{
    return m_mSubTitleText;
}
void XPlay::setSubTitleText(QString text)
{
    if(m_mSubTitleText != text)
    {
        m_mSubTitleText = text;
        emit subTitleTextChanged(m_mSubTitleText);
    }
}

long long XPlay::getTotalPts()
{
    return m_totalPts;
}

void XPlay::setTotalPts(long long pts) const
{
    m_totalPts = pts;
}
