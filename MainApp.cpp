#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QIcon>
#include <QFont>
#include <QTranslator>
#include <QSettings>
#include "MainApp.h"
#include "XPlay.h"
#include "XVideoOutput.h"
#include "Logger.h"

#define APP_NAME                    "XPlayer"
#define APP_ICON_PATH               "qrc:/image/AppIcon.ico"
#define APP_DISPLAYNAME             "XPlayer"
#define UI_MAIN_PATH                "qrc:/qml/main.qml"
#define UI_TRANSLATION_PATH         ":/translation/XPlayer_zh_CN.qm"
#define IMAGEDIR_RELATIVE_PATH      "./resource/ui/image"
#define DEFAULT_FONT_NAME           "\xE5\xBE\xAE\xE8\xBD\xAF\xE9\x9B\x85\xE9\xBB\x91"// 微软雅黑

MainApp* g_pApp = nullptr;

MainApp* GetMainApp()
{
    return g_pApp;
}

QObject* MainApp::GetInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    QObject* pObj = GetMainApp();
    QQmlEngine::setObjectOwnership(pObj, QQmlEngine::CppOwnership);
    return pObj;

}
MainApp::MainApp(int &argc, char **argv)
       : QApplication(argc, argv)
       , m_strAppRootPath()
       , m_strImageRootPath()
{

}

int MainApp::Main(int argc, char *argv[])
{
    if(!Initialize())
    {
        LOG_DEBUG << "Initialize failed, exit...";
        Uninitialize();
        return EXIT_FAILURE;
    }
    // star to exec the program
    // event loop here
    int nRet = exec();

    return nRet;
}

bool MainApp::Initialize()
{
    setApplicationName(QStringLiteral(APP_NAME));


    //setWindowIcon(QIcon(QStringLiteral("D:\\self-learning\\XPlayer_proj\\XPlayer\\AppIcon.ico")));
    setWindowIcon(QIcon(":/image/AppIcon.png"));


    setApplicationDisplayName(tr(APP_DISPLAYNAME));
    // Initialize Path
    InitializePath();


    if (!InitializeQmlEngine())
    {
        LOG_DEBUG << "initialize QmlEngine failed";
        return false;
    }
    // Initialize UI
    if (!InitializeUI(m_pQmlEngine.data()))
    {
        LOG_DEBUG << "initialize UI failed";
        return false;
    }

    LOG_DEBUG << "initialize everything successfully";
    return true;

}
bool MainApp::Uninitialize()
{

}

bool MainApp::InitializePath()
{
    m_strAppRootPath = qApp->applicationDirPath();
    QDir dirApp(m_strAppRootPath);
    if (!dirApp.exists())
    {
        return false;
    }
    // Image root directory path
    m_strImageRootPath = m_strAppRootPath;
    m_strImageRootPath.append(QStringLiteral("/")).append(QStringLiteral(IMAGEDIR_RELATIVE_PATH));
    QDir dirImg(m_strImageRootPath);
    if (!dirImg.exists())
    {
    }
    m_strImageRootPath = dirImg.canonicalPath();

    return true;
}

bool MainApp::InitializeQmlEngine()
{
    if (m_pQmlEngine != nullptr)
    {
        return true;
    }

    // Create QML engine instance
    m_pQmlEngine.reset(new QQmlApplicationEngine);

    return (nullptr != m_pQmlEngine);
}

bool MainApp::InitializeUI(QQmlApplicationEngine *pQmlEngine)
{
    if(pQmlEngine == nullptr)
    {
        return false;
    }
    setFont(QFont(DEFAULT_FONT_NAME,9));
    qmlRegisterSingletonType<MainApp>("com.may.myplayer", 1, 0, "MainApp",
                                      &MainApp::GetInstance);
    qmlRegisterType<XPlay>("com.may.myplayer", 1, 0, "XPlay");
    qmlRegisterType<XVideoOutput>("com.may.myplayer", 1, 0, "XVideoOutput");

    pQmlEngine->load(QUrl(QStringLiteral(UI_MAIN_PATH)));


//    QList<QObject*> rootObjs(pQmlEngine->rootObjects());
//    return !rootObjs.isEmpty();;
    return true;

}

int MainApp::demoNum() const
{
    return m_mDemoNum;
}

void MainApp::setDemoNum(int newValue)
{
    if (m_mDemoNum != newValue)
    {
        m_mDemoNum = newValue;
        emit demoNumChanged(m_mDemoNum);
    }
}

QString MainApp::language() const
{
    return m_mLanguage;
}

void MainApp::setLanguage(QString newValue)
{
    if (m_mLanguage != newValue)
    {
        m_mLanguage = newValue;
        emit languageChanged(m_mLanguage);
    }
}

void MainApp::changeLanuage(QString language)
{
    if(language == MainApp::language())
        return;
    LOG_DEBUG << language;
    m_pTranslator.reset(new QTranslator);
    QSettings *settingIni = new QSettings("setting.ini",QSettings::IniFormat);
    settingIni->setValue("Config/Language",language);
    if(language == "Chinese")
    {
        if (!m_pTranslator->load(QStringLiteral(UI_TRANSLATION_PATH)))
            {return;}
        // Add translator
        installTranslator(m_pTranslator.data());
    }
    if(language == "English")
        installTranslator(m_pTranslator.data());
    setLanguage(language);
    delete settingIni;
}


MainApp::~MainApp() = default;

