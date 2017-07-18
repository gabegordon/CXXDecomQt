/** @mainpage H5 Decom Utility
    This is a tool designed to parse .h5 (RDR) files containing science or hk data. ...
    It takes an input folder of .h5 files and outputs CSVs for each APID contained within the input directory's h5 files. ...
    These CSVs contain human-readable data. For each data field in the APID a corresponding column is generated in the CSV file. ...
    ...
    Instructions for Running can be found [here](@ref running)
    Instructions for Compiling can be found [here](@ref compiling)
*/


#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "backend.h"

int main(int argc, char *argv[])
{
#ifdef __linux__
        QProcess::execute("cd output && rm -f *.txt");  // Clear output directory
        QProcess::execute("mkdir -p output");
#else
        QProcess::execute("cd output && del /Q *.txt 2>NUL 1>NUL");
        QProcess::execute("if not exist output mkdir output 2>NUL 1>NUL");
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<BackEnd>("io.qt.backend", 1, 0, "BackEnd");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
