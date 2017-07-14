#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <string>
#include <set>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString folderName WRITE setFolderName NOTIFY folderNameChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);
    virtual ~BackEnd() {}

    QString userName();
    void setFolderName(const QString& folderName);
    void setUserName(const QString& userName);
public slots:
    void decodeh5();


signals:
    void userNameChanged();
    void folderNameChanged();

private:
    QString m_userName;
    QString m_folderName;
    bool m_allAPIDs;
    std::set<std::string> m_ofiles;
};
#endif // BACKEND_H
