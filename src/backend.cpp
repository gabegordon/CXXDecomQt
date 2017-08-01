#include <QGuiApplication>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "backend.hpp"
#include "DatabaseReader.hpp"

BackEnd::BackEnd(QObject* parent) :
    QObject(parent),
    m_folderName{},
    m_allAPIDs{false},
    m_ofiles{},
    m_debug{false},
    m_H5{false},
    m_PDS{false},
    m_h5Dec{},
    m_pdsDec{},
    m_type{DataTypes::SCType::J1}
{}

/**
 * Sets foldername member based on QML file selector.
 *
 * @param folderName Folder path.
 */
void BackEnd::setFolderName(const QString& folderName)
{
    if (folderName == m_folderName)
        return;

    m_folderName = folderName;
}

/**
 * Sets selectedAPIDs to comma separated string entered by user.
 *
 * @param apids APID string.
 */
void BackEnd::setAPIDs(const QString& apids)
{
    m_selectedAPIDs = apids;
}

/**
 * Toggles H5 decode on, and PDS decode off.
 */
void BackEnd::setH5()
{
    m_H5 = true;
    m_PDS = false;
}

/**
 * Toggles H5 decode on, and PDS decode off.
 */
void BackEnd::setPDS()
{
    m_PDS = true;
    m_H5 = false;
}

/**
 * Given a string containing the progressbar (or another message) convert to QString. Then signal Qt.
 *
 * @param prog String to be display.
 */
void BackEnd::setProgress(const std::string& prog)
{
    m_progress = QString::fromStdString(prog);
    emit progressChanged();
    QCoreApplication::processEvents();
}

/**
 * Sends finished signal qt to display exit button for user.
 */
void BackEnd::setFinished() const
{
    emit finished();
}

/**
 * Displays the name of the current file being decommed in the GUI.
 *
 * @param filename Filename string to display.
 */
void BackEnd::setCurrentFile(const std::string& filename)
{
    m_currentFile = QString::fromStdString(filename);
    emit currentFileChanged();
}

/**
 * Sets satellite type.
 *
 * @param type Type to be set.
 */
void BackEnd::setType(const DataTypes::SCType& type)
{
    m_type = type;
}

/**
 * Toggles APID filtering.
 */
void BackEnd::toggleAllAPIDs()
{
    m_allAPIDs = !m_allAPIDs;
}

/**
 * Given user selected packet file, add to list of selected files.
 *
 * @param packetFile File to add.
 */
void BackEnd::addPacketFile(const QString &packetFile)
{
    m_packetFiles.emplace_back(packetFile.toStdString());
}

/**
 * Remove packet file if user deselects a file.
 * @param packetFile File to remove.
 */
void BackEnd::removePacketFile(const QString &packetFile)
{
    m_packetFiles.erase(std::remove(std::begin(m_packetFiles), std::end(m_packetFiles), packetFile.toStdString()), std::end(m_packetFiles));
}

/**
 * Getter function for files. Allows Qt to display list of possible packet files.
 *
 * @return List of file names
 */
QStringList BackEnd::ofiles() const
{
    QStringList tQList;

    for (const std::string& s : m_ofiles)
    {
        tQList.append(QString::fromStdString(s));
    }
    return tQList;
}

/**
 * Getter function (for Qt) for the current file being decommed.
 */
QString BackEnd::currentFile() const
{
    return m_currentFile;
}

QString BackEnd::progress() const
{
    return m_progress;
}

/**
 * Gets lists of possible files to decom. Passes to Qt for user selection.
 */
void BackEnd::getFiles()
{
    if (m_folderName == "")
        return;

    std::string folderName;
#ifdef __linux__
    folderName = m_folderName.toStdString().substr(7);  // Remove "file://" from filename
#else
    folderName = m_folderName.toStdString().substr(8);  // On Windows remove "file:///"
#endif

    if (m_H5)
    {
        m_ofiles = m_h5Dec.getFileTypeNames(folderName, this);
    }
    else
    {
        m_ofiles = m_pdsDec.getFileTypeNames(folderName);
    }
    emit ofilesChanged();
}

/**
 * Main decom loop. Loads databases and then loops through all user-selected packet files and decoms them.
 */
void BackEnd::runDecom()
{
    if (m_ofiles.size() == 0)
        return;
    DatabaseReader dr(m_allAPIDs, m_type, getSelectedAPIDs());  // Read databases
    if (m_H5)
        m_h5Dec.init(this, m_packetFiles, m_debug, dr.getEntries(), m_type);
    else
        m_pdsDec.init(this, m_packetFiles, m_debug, dr.getEntries(), m_type);
}

/**
 * Converts QString of APIDs to vector of integers.
 *
 * @return Vector containing the apids.
 */
std::vector<uint32_t> BackEnd::getSelectedAPIDs()
{
    std::vector<uint32_t> apids;
    std::string apidString = m_selectedAPIDs.toStdString();

    if (apidString == "")
        return apids;

    std::vector<std::string> splitstring;
    boost::split(splitstring, apidString, boost::is_any_of(","));
    for (const auto& apid : splitstring)
    {
        apids.emplace_back(std::stoul(apid));
    }
    return apids;
}
