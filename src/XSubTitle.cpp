#include <QFileInfo>
#include <QList>
#include "XSubTitle.h"
#include "Logger.h"
//#include "MYSrtParser.h"


XSubTitle::XSubTitle()
{
    LOG_INFO << "initial XSubTitle";
}

XSubTitle::~XSubTitle()
{

}

typedef struct subNode{
    int num;
    int beginTime;
    int endTime;
    QString text;
}subNode;
QList<subNode> subNodelist;

//打开
void XSubTitle::Open(QString url)
{
    m_mSubTitleFile.reset(new QFile(url));
    QFileInfo fileInfo(url);
    if(fileInfo.suffix() == "srt")
        m_mSubType = SUBTITLE_TYPE_SRT;
    if(m_mSubTitleFile->open(QIODevice::ReadOnly))  //只读，读取时行尾终止符被转换为'\n'
    {
        LOG_DEBUG << "open subtitle success";
        subNodelist.clear();
        m_mIsOpen = true;
        while(!m_mSubTitleFile->atEnd())
        {
            subNode sn;
            QString line = "";
            QString cache = "";

            line = (QString)m_mSubTitleFile->readLine();               //序号行

            sn.num = line.left(line.length() - 2).toInt();               //-4代表去掉"\r\n"

            line = (QString)m_mSubTitleFile->readLine();               //时间行

            int begin_hour = line.mid(0,2).toInt();
            int begin_minute = line.mid(3,2).toInt();
            int begin_second = line.mid(6,2).toInt();
            int begin_milli = line.mid(9,3).toInt();
            int beginTime = (begin_hour * 3600 + begin_minute * 60 + begin_second) * 1000 + begin_milli;  //起始时间
            int end_hour = line.mid(17,2).toInt();
            int end_minute = line.mid(20,2).toInt();
            int end_second = line.mid(23,2).toInt();
            int end_milli = line.mid(26,3).toInt();
            int endTime = (end_hour * 3600 + end_minute * 60 + end_second) * 1000 + end_milli;            //终止时间

            sn.beginTime = beginTime;
            sn.endTime = endTime;

            line = (QString)m_mSubTitleFile->readLine();              //文本行
            while(line != "\r\n")
            {
//                if(!(line.indexOf("{") == -1))
//                    qDebug()<<line.indexOf("{")<<" "<<line.indexOf("}");
                while(!(line.indexOf("{") == -1))                       //找不到返回-1,找到返回索引
                    line.replace(line.indexOf("{"),line.indexOf("}") - line.indexOf("{") + 1,"");
                cache += line.left(line.length() - 2) + "\n";        //去除"\r\n",添加"\n"
                line = (QString)m_mSubTitleFile->readLine();          //文本行
            }
            cache = cache.left(cache.length() - 1);                  //去除最后一行的"\n"

            sn.text = cache;

            subNodelist.append(sn);
        }
//        for(int i = 0;i<subNodelist.count();i++)
//        {
//            qDebug()<<subNodelist[i].text;
//        }
        return;
    }
    LOG_DEBUG << "open subtitle file failed!";
}

bool XSubTitle::Read(int currentTime)
{
    if(!m_mSubTitleFile->isOpen())
        return false;
    for(int i = 0;i < subNodelist.count();i++)
    {
        if(subNodelist[i].beginTime < currentTime && subNodelist[i].endTime > currentTime)
        {
            m_mNodeStartTime = subNodelist[i].beginTime;
            m_mNodeEndTime = subNodelist[i].endTime;
            m_mText = subNodelist[i].text;
            return true;
        }
        if(subNodelist[i].beginTime > currentTime)
        {
            //m_mText = "";
            return false;
        }
    }
    return true;
}

//是否打开
bool XSubTitle::isOpen()
{
    return m_mIsOpen;
}
//开始时间
int XSubTitle::nodeStartTime()
{
    return m_mNodeStartTime;
}
//结束时间
int XSubTitle::nodeEndTime()
{
    return m_mNodeEndTime;
}
//字幕内容
QString XSubTitle::text()
{
    return m_mText;
}
