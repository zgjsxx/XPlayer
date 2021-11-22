#ifndef XPLAY_H
#define XPLAY_H

#include <QObject>
class XPlay : public QObject
{
    Q_OBJECT
    // For QML property(-ies)
    // e.g.
    Q_PROPERTY(int demoNum READ demoNum WRITE setDemoNum NOTIFY demoNumChanged)

    Q_PROPERTY(int testNum READ testNum WRITE setTestNum NOTIFY testNumChanged)

    Q_PROPERTY(double posNum READ posNum WRITE setPosNum NOTIFY posNumChanged)

    Q_PROPERTY(QObject* source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool isPlay READ isPlay WRITE setIsPlay NOTIFY isPlayingChanged)

    Q_PROPERTY(bool openSuccess READ openSuccess WRITE setOpenSuccess NOTIFY openSuccessChanged)

    Q_PROPERTY(QString backGround READ backGround WRITE setBackGround NOTIFY backGroundChanged)

    Q_PROPERTY(QString backGroundChoose READ backGroundChoose WRITE setBackGround NOTIFY backGroundChooseChanged)

    Q_PROPERTY(QString videoPath READ videoPath WRITE setVideoPath NOTIFY videoPathChanged)

    Q_PROPERTY(QString subTitlePath READ subTitlePath WRITE setSubTitlePath NOTIFY subTitlePathChanged)

    Q_PROPERTY(QString subTitleText READ subTitleText WRITE setSubTitleText NOTIFY subTitleTextChanged)
public:
    XPlay();
    ~XPlay();
public:
    // For QML invokable function(s)
    Q_INVOKABLE void testFunc();

    //打开线程，传输文件路径
    Q_INVOKABLE void urlPass(QString url);
    //播放进度条
    Q_INVOKABLE void posFind(double value);
    //播放暂停
    Q_INVOKABLE void playOrPause();
    //快进
    Q_INVOKABLE void goOn();
    //快退
    Q_INVOKABLE void goBack();
    //改变储存背景
    Q_INVOKABLE void changeBackground(QString url);
    //改变视频选择路径
    Q_INVOKABLE void changeVideoPath(QString url);
    //改变字幕选择路径
    Q_INVOKABLE void changeSubTitlePath(QString url);
    //改变音量
    Q_INVOKABLE void setVolume(double newVolume);
    //停止
    Q_INVOKABLE void setStop();
    //加载字幕
    Q_INVOKABLE void loadSubTitle(QString url);

    //定时器 滑动条显示
    void timerEvent(QTimerEvent *e);

    QObject *source() const;

    void setSource(QObject *source);


    // For QML property(-ies)
    // e.g.
    // demoNum
    int demoNum() const;
    void setDemoNum(int newValue);

    int testNum() const;
    void setTestNum(int newValue);

    double posNum() const;
    void setPosNum(double newValue);

    bool isPlay() const;
    void setIsPlay(bool isPlaying);

    bool openSuccess() const;
    void setOpenSuccess(bool openSuccess);
    //背景图片路径
    QString backGround() const;
    void setBackGround(QString url);
    //选择背景路径
    QString backGroundChoose() const;
    //选择视频路径
    QString videoPath() const;
    void setVideoPath(QString url);
    //选择字幕路径
    QString subTitlePath() const;
    void setSubTitlePath(QString url);
    //字幕文本
    QString subTitleText() const;
    void setSubTitleText(QString text);

signals:
    void demoNumChanged(int newValue);
    void testNumChanged(int newValue);
    void posNumChanged(double newValue);
    void sourceChanged(QObject *object);
    void isPlayingChanged(bool isPlaying);
    void openSuccessChanged(bool openSuccess);
    void backGroundChanged(QString url);
    void backGroundChooseChanged(QString url);
    void videoPathChanged(QString url);
    void subTitlePathChanged(QString url);
    void subTitleTextChanged(QString text);

private:
    int     m_mDemoNum{ 0 };
    int     m_mTestNum{ 0 };
    double  m_mPosNum{ 0.0 };
    QObject*    m_mSource{nullptr};
    bool        m_mIsPlaying{false};
    bool        m_mOpenSuccess{false};
    QString     m_strBackgroundPath{""};
    QString     m_strBackGroundChoosePath{""};
    QString     m_strVideoPath{""};
    QString     m_strTheVideoPath{ "" };
    QString     m_strSubTitlePath{""};
    QString     m_mSubTitleText{""};
};

#endif // MYPLAY_H
