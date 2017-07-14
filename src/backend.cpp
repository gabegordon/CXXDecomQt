#include <iostream>
#include <QDebug>
#include "backend.h"
#include "h5Decode.h"
#include "DatabaseReader.h"
#include "Decom.h"


BackEnd::BackEnd(QObject *parent, const bool debug) :
    QObject(parent),
    m_folderName{},
    m_allAPIDs{true},
    m_ofiles{},
    m_instrument{"ATMS"},
    m_debug{debug}
{
}

void BackEnd::setFolderName(const QString &folderName)
{
    if (folderName == m_folderName)
        return;

    m_folderName = folderName;
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
    // TODO Instrument variable
    DatabaseReader dr{"databases/CXXParams.csv", m_allAPIDs};  // Read databases
    Decom decomEngine{m_instrument, m_debug, dr.getEntries()};  // Run decom
    decomEngine.init("output/" + *(m_ofiles.begin())); // TODO Iterate through outfiles
}

