/** @mainpage h5 %Decom Utility
    This is a tool designed to parse .h5 or .PDS (RDR) files containing Science or HK data. \n
    It takes an input folder of files and outputs CSVs for each APID contained within the input directory's files. \n
    These CSVs contain human-readable data. For each data field in the APID a corresponding column is generated in the CSV file. \n
    \n
    Instructions for Running can be found [here](@ref running) \n
    Instructions for Compiling can be found [here](@ref compiling) \n
    Simple description of control flow [here](@ref control) \n
*/
// TODO Buffer ATMS Instrument Format Read/Write.
// TODO Find a way to detect instrument type from PDS files.

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "backend.hpp"

int main(int argc, char *argv[])
{
#ifdef __linux__
    system("mkdir -p output");
    system("cd output && rm -f *.txt");  // Clear output directory
#else
    system("if not exist output mkdir output 2>NUL 1>NUL");
    system("cd output && del /Q *.txt 2>NUL 1>NUL");
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<BackEnd>("io.qt.backend", 1, 0, "BackEnd");  // Expose backend class to QML

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();  // Start Qt event loop
}
