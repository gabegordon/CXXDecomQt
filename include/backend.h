#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <string>
#include <vector>
#include <set>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString folderName WRITE setFolderName)
    Q_PROPERTY(QStringList ofiles READ ofiles NOTIFY ofilesChanged)
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QString progress READ progress NOTIFY progressChanged)
public:
    explicit BackEnd(QObject *parent = nullptr);
    virtual ~BackEnd() {}

    void setFolderName(const QString& folderName);
    void setProgress(const std::string& prog);

    QString currentFile();
    QStringList ofiles();
    QString progress();

public slots:
    void decodeh5();
    void runDecom();
    void addPacketFile(const QString &packetFile);
    void removePacketFile(const QString &packetFile);

signals:
    void ofilesChanged();
    void currentFileChanged();
    void progressChanged();
    void finished();
    void finishedh5();

private:
    QString m_userName;
    QString m_folderName;
    bool m_allAPIDs;
    std::set<std::string> m_ofiles;
    bool m_debug;
    std::vector<std::string>m_packetFiles;
    QString m_currentFile;
    QString m_progress;
};
#endif // BACKEND_H
