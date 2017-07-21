#include <iostream>
#include <QGuiApplication>
#include <algorithm>
#include "backend.h"
#include "h5Decode.h"
#include "DatabaseReader.h"
#include "Decom.h"


BackEnd::BackEnd(QObject* parent) :
    QObject(parent),
    m_folderName{},
    m_allAPIDs{true},
    m_ofiles{},
    m_debug{true},
    m_NPP{false}
{}

/**
 * Sets foldername member based on QML file selector.
 *
 * @param folderName Folder path
 */
void BackEnd::setFolderName(const QString& folderName)
{
    if (folderName == m_folderName)
        return;

    m_folderName = folderName;
}

/**
 * Given a string containing the progressbar convert to QString. Then signal and update Qt.
 *
 * @param prog String to be display
 */
void BackEnd::setProgress(const std::string& prog)
{
    m_progress = QString::fromStdString(prog);
    emit progressChanged();
    QGuiApplication::sync();
}

/**
 * Given user selected of packet files, add to list of selected files.
 * @param packetFile File to add
 */
void BackEnd::addPacketFile(const QString &packetFile)
{
    m_packetFiles.emplace_back(packetFile.toStdString());
}

/**
 * Remove packet file if user deselects a file.
 * @param packetFile File to remove
 */
void BackEnd::removePacketFile(const QString &packetFile)
{
    m_packetFiles.erase(std::remove(std::begin(m_packetFiles), std::end(m_packetFiles), packetFile.toStdString()), std::end(m_packetFiles));
}

/**
 * Getter function for pkt files. Allows Qt to display list of possible packet files.
 *
 * @return List of file names
 */
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

/**
 * Passes user selected folder to h5Decode and parses all files in the directory.
 */
void BackEnd::decodeh5()
{
    if (m_folderName == "")
        return;
#ifdef __linux__
    h5Decode h5Dec(m_folderName.toStdString().substr(7));  // Remove "file:///" from filename
#else
    h5Decode h5Dec(m_folderName.toStdString().substr(8));
#endif
    m_ofiles = h5Dec.init(this);
    emit finishedh5();
    emit ofilesChanged();
}

/**
 * Main decom loop. Loads databases and then loops through all user-selected packet files and decoms them.
 */
void BackEnd::runDecom()
{
    if(m_ofiles.size() == 0)
        return;
    DatabaseReader dr("databases/CXXParams.csv", m_allAPIDs, m_NPP);  // Read databases

    for(const auto& packetFile : m_packetFiles)
    {
        m_currentFile = QString::fromStdString(packetFile);
        emit currentFileChanged();
        QGuiApplication::sync();

        size_t found;
        std::string instrument;
        if ((found = packetFile.find("-")) != std::string::npos)  // Get instrument from filename
            instrument = packetFile.substr(0, found);

        Decom decomEngine(instrument, m_debug, dr.getEntries(), m_NPP);  // Run decom
        decomEngine.init("output/" + packetFile, this);
    }
    emit finished();
}
