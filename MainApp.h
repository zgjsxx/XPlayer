/*
 * @file: MainApp.h
 * @brief: register object to qml
 * @author: zgjsxx
*/

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
    Q_PROPERTY(int demoNum READ demoNum WRITE setDemoNum NOTIFY demoNumChanged)

    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
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

signals:
    void demoNumChanged(int newValue);
    void languageChanged(QString newValue);

public:
    Q_INVOKABLE void changeLanuage(QString language);
    int demoNum() const;
    void setDemoNum(int newValue);

    QString language() const;
    void setLanguage(QString newValue);

private:
    QString  m_strAppRootPath;
    QString  m_strImageRootPath;
    QScopedPointer<QQmlApplicationEngine>   m_pQmlEngine;
    int m_mDemoNum{0};
    QScopedPointer<QTranslator> m_pTranslator;
    QString  m_mLanguage{""};
};


MainApp* GetMainApp();


#endif // MAINAPP_H
