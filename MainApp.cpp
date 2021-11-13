#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QIcon>
#include <QFont>
#include <QDebug>
#include "MainApp.h"


#define APP_NAME                    "XPlayer"
#define APP_ICON_PATH               ":/image/AppIcon.ico"
#define APP_DISPLAYNAME             "XPlayer"
#define UI_MAIN_PATH                "qrc:/qml/main.qml"
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
        qDebug()<<"Initialize failed, exit...";
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
    setWindowIcon(QIcon(QStringLiteral(APP_ICON_PATH)));
    setApplicationDisplayName(tr(APP_DISPLAYNAME));
    // Initialize Path
    InitializePath();


    if (!InitializeQmlEngine())
    {
        qDebug()<<"initialize QmlEngine failed";
        return false;
    }
    // Initialize UI
    if (!InitializeUI(m_pQmlEngine.data()))
    {
        qDebug()<<"initialize UI failed";
        return false;
    }

    qDebug()<<"initialize everything successfully";
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

    pQmlEngine->load(QUrl(QStringLiteral(UI_MAIN_PATH)));

//    QList<QObject*> rootObjs(pQmlEngine->rootObjects());
//    return !rootObjs.isEmpty();;
    return true;

}

MainApp::~MainApp() = default;

