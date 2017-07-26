#include <iostream>
#include <QGuiApplication>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "backend.h"
#include "h5Decode.h"
#include "pdsDecode.h"
#include "DatabaseReader.h"
#include "Decom.h"


BackEnd::BackEnd(QObject* parent) :
    QObject(parent),
    m_NPP{false},
    m_folderName{},
    m_allAPIDs{false},
    m_ofiles{},
    m_debug{false},
    m_H5{false},
    m_PDS{false}
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

void BackEnd::setAPIDs(const QString& apids)
{
    m_selectedAPIDs = apids;
}

void BackEnd::setH5()
{
    m_H5 = true;
    m_PDS = false;
}

void BackEnd::setPDS()
{
    m_PDS = true;
    m_H5 = false;
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

void BackEnd::toggleAllAPIDs()
{
    m_allAPIDs = !m_allAPIDs;
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
 * Passes user selected folder to decode and parses all files in the directory.
 */
void BackEnd::decode()
{
    if (m_folderName == "")
        return;

    std::string folderName;
#ifdef __linux__
    folderName = m_folderName.toStdString().substr(7);  // Remove "file:///" from filename
#else
    folderName = m_folderName.toStdString().substr(8);
#endif

    if(m_H5)
    {
        h5Decode h5Dec(folderName);
        m_ofiles = h5Dec.init(this);
    }
    else
    {
        pdsDecode pdsDec(folderName);
        m_ofiles = pdsDec.init(this);
    }
    emit finishedDecode();
    emit ofilesChanged();
}

/**
 * Main decom loop. Loads databases and then loops through all user-selected packet files and decoms them.
 */
void BackEnd::runDecom()
{
    if(m_ofiles.size() == 0)
        return;
    DatabaseReader dr(m_allAPIDs, m_NPP, getSelectedAPIDs());  // Read databases

    for(const auto& packetFile : m_packetFiles)
    {
        m_currentFile = QString::fromStdString(packetFile);
        emit currentFileChanged();
        QGuiApplication::sync();

        size_t found;
        std::string instrument;
        if ((found = packetFile.find("-")) != std::string::npos)  // Get instrument from filename
            instrument = packetFile.substr(0, found);
        else
            instrument = "CERES";  // CERES PDS files dont have name included
        Decom decomEngine(instrument, m_debug, dr.getEntries(), m_NPP);  // Run decom
        decomEngine.init("output/" + packetFile, this);
    }
    emit finished();
}


/**
 * Converts QString of APIDs to vector of integers.
 *
 * @return Vector containing the apids
 */
std::vector<uint32_t> BackEnd::getSelectedAPIDs()
{
    std::vector<uint32_t> apids;
    std::string apidString = m_selectedAPIDs.toStdString();

    if (apidString == "")
        return apids;

    std::vector<std::string> splitstring;
    boost::split(splitstring, apidString, boost::is_any_of(","));
    for(const auto& apid: splitstring)
    {
        apids.emplace_back(std::stoul(apid));
    }
    return apids;
}
