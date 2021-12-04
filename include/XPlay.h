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

    Q_PROPERTY(long long totalPts READ getTotalPts  WRITE setTotalPts NOTIFY totalPtsChanged)
public:
    XPlay();
    ~XPlay();
public:
    // For QML invokable function(s)
    Q_INVOKABLE void testFunc();

    //open thread, pass the file path(can accept rtmp url)
    Q_INVOKABLE void urlPass(QString url);
    //play postion slider
    Q_INVOKABLE void posFind(double value);
    //pause the player
    Q_INVOKABLE void playOrPause();
    //speed
    Q_INVOKABLE void goOn();
    //fast backword
    Q_INVOKABLE void goBack();
    //change the background
    Q_INVOKABLE void changeBackground(QString url);
    //change the video path
    Q_INVOKABLE void changeVideoPath(QString url);
    //change the subtitle path
    Q_INVOKABLE void changeSubTitlePath(QString url);
    //change the volume
    Q_INVOKABLE void setVolume(double newVolume);
    //stop the play
    Q_INVOKABLE void setStop();
    //load subtitle
    Q_INVOKABLE void loadSubTitle(QString url);

    //a timer to let the play slider move
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
    //the path of background
    QString backGround() const;
    void setBackGround(QString url);
    //choose background path
    QString backGroundChoose() const;
    //choose video path
    QString videoPath() const;
    void setVideoPath(QString url);
    //choose subtitle path
    QString subTitlePath() const;
    void setSubTitlePath(QString url);
    //ubtitletext
    QString subTitleText() const;
    void setSubTitleText(QString text);

    long long getTotalPts() const;
    void setTotalPts(long long pts) ;

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
    void totalPtsChanged(QString text);

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
    long long m_totalPts {0};
};

#endif // MYPLAY_H
