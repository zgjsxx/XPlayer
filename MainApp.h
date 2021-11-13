#ifndef MAINAPP_H
#define MAINAPP_H

#include <QApplication>
#include <QPointer>

class QJSEngine;
class QQmlEngine;
class QQmlApplicationEngine;

class MainApp : public QApplication
{
    Q_OBJECT
public:
    static QObject* GetInstance(QQmlEngine* engine, QJSEngine* scriptEngine);

public:
    MainApp(int &argc, char** argv);
    ~MainApp() override;

    int Main(int argc, char *argv[]);

private:
    bool Initialize();
    bool Uninitialize();

    bool InitializePath();
    bool UninitializePath();

    bool InitializeQmlEngine();

    bool InitializeUI(QQmlApplicationEngine* pQmlEngine);


private:
    QString  m_strAppRootPath;
    QString  m_strImageRootPath;
    QScopedPointer<QQmlApplicationEngine>   m_pQmlEngine;
};


MainApp* GetMainApp();


#endif // MAINAPP_H
