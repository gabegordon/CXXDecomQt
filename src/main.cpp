#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "backend.h"

int main(int argc, char *argv[])
{
#ifdef __linux__
        system("cd output && rm -f *.txt");  // Clear output directory
#else
        system("cd output && del /Q *.txt 2>NUL 1>NUL");
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<BackEnd>("io.qt.backend", 1, 0, "BackEnd");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}