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
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QString progress READ progress NOTIFY progressChanged)
public:
    explicit BackEnd(QObject *parent = nullptr, const bool debug = false);
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

signals:
    void ofilesChanged();
    void currentFileChanged();
    void progressChanged();
    void finished();

private:
    QString m_userName;
    QString m_folderName;
    bool m_allAPIDs;
    std::set<std::string> m_ofiles;
    bool m_debug;
    std::set<std::string>m_packetFiles;
    QString m_currentFile;
    QString m_progress;
};
#endif // BACKEND_H
