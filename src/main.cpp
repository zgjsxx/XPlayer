#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <qquickwindow.h>
#include "MainApp.h"
#include "Logger.h"

extern MainApp* g_pApp;

int main(int argc, char *argv[])
{
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    Logger::initLog();
    LOG_INFO << "Init log module successfully";
    MainApp app(argc, argv);

    g_pApp = &app;
    int nRet = app.Main(argc, argv);
    g_pApp = nullptr;

    return nRet;
}
