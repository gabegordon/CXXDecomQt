#include <iostream>
#include <QDebug>
#include "backend.h"
#include "h5Decode.h"


BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_folderName{},
    m_allAPIDs{true},
    m_ofiles{}
{
}

QString BackEnd::userName()
{
    return m_userName;
}

void BackEnd::setFolderName(const QString &folderName)
{
    if (folderName == m_folderName)
        return;

    m_folderName = folderName;
    emit folderNameChanged();
}

void BackEnd::setUserName(const QString &userName)
{
    if (userName == m_userName)
        return;

    m_userName = userName;
    emit userNameChanged();
}

void BackEnd::decodeh5()
{
    if (m_folderName == "")
        return;

    h5Decode h5Dec(m_folderName.toStdString().substr(6));
    m_ofiles = h5Dec.init();
}

