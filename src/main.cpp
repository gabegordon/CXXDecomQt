/** @mainpage h5 %Decom Utility
    This is a tool designed to parse .h5 (RDR) files containing Science or HK data. \n
    It takes an input folder of .h5 files and outputs CSVs for each APID contained within the input directory's h5 files. \n
    These CSVs contain human-readable data. For each data field in the APID a corresponding column is generated in the CSV file. \n
    \n
    Instructions for Running can be found [here](@ref running) \n
    Instructions for Compiling can be found [here](@ref compiling) \n
    Simple description of control flow [here](@ref control) \n
*/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "backend.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<BackEnd>("io.qt.backend", 1, 0, "BackEnd");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
