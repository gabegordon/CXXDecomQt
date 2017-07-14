#include <iostream>
#include <QDebug>
#include <QGuiApplication>
#include "backend.h"
#include "h5Decode.h"
#include "DatabaseReader.h"
#include "Decom.h"


BackEnd::BackEnd(QObject* parent, const bool debug) :
    QObject(parent),
    m_folderName{},
    m_allAPIDs{true},
    m_ofiles{},
    m_debug{debug}
{
}

void BackEnd::setFolderName(const QString& folderName)
{
    if (folderName == m_folderName)
        return;

    m_folderName = folderName;
}

void BackEnd::setProgress(const std::string& prog)
{
    m_progress = QString::fromStdString(prog);
    emit progressChanged();
    QGuiApplication::sync();
}

void BackEnd::addPacketFile(const QString &packetFile)
{
    m_packetFiles.emplace(packetFile.toStdString());
}

QStringList BackEnd::ofiles()
{
    QStringList tQList;

    for(const std::string& s : m_ofiles)
    {
        tQList.append(QString::fromStdString(s));
    }
    return tQList;
}

QString BackEnd::currentFile()
{
    return m_currentFile;
}

QString BackEnd::progress()
{
    return m_progress;
}

void BackEnd::decodeh5()
{
    if (m_folderName == "")
        return;

    h5Decode h5Dec(m_folderName.toStdString().substr(6));
    m_ofiles = h5Dec.init();
    emit ofilesChanged();
}

void BackEnd::runDecom()
{
    if(m_ofiles.size() == 0)
        return;
    DatabaseReader dr("databases/CXXParams.csv", m_allAPIDs);  // Read databases

    for(const auto& packetFile : m_packetFiles)
    {
        m_currentFile = QString::fromStdString(packetFile);
        emit currentFileChanged();
        QGuiApplication::sync();

        size_t found;
        std::string instrument;
        if ((found = packetFile.find("-")) != std::string::npos)  // Get instrument from filename
            instrument = packetFile.substr(0, found);

        Decom decomEngine(instrument, m_debug, dr.getEntries());  // Run decom
        decomEngine.init("output/" + packetFile, this);
    }
    emit finished();
}
