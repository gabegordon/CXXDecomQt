#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <string>
#include <set>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString folderName WRITE setFolderName)
    Q_PROPERTY(QStringList ofiles READ ofiles NOTIFY ofilesChanged)
public:
    explicit BackEnd(QObject *parent = nullptr, const bool debug = false);
    virtual ~BackEnd() {}

    void setFolderName(const QString& folderName);
    QStringList ofiles();

public slots:
    void decodeh5();
    void runDecom();

signals:
    void ofilesChanged();

private:
    QString m_userName;
    QString m_folderName;
    bool m_allAPIDs;
    std::set<std::string> m_ofiles;
    std::string m_instrument;
    bool m_debug;
};
#endif // BACKEND_H
