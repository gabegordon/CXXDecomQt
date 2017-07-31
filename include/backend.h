#include <QObject>
#include <QString>
#include <QStringList>
#include <string>
#include <vector>
#include <set>
#include "h5Decode.h"
#include "pdsDecode.h"

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList ofiles READ ofiles NOTIFY ofilesChanged)
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QString progress READ progress NOTIFY progressChanged)
public:
    explicit BackEnd(QObject *parent = nullptr);
    virtual ~BackEnd() {}

    void setProgress(const std::string& prog);
    void setCurrentFile(const std::string& filename);

    QString currentFile();
    QStringList ofiles();
    QString progress();

    bool m_NPP;
public slots:
    void getFiles();
    void runDecom();
    void addPacketFile(const QString &packetFile);
    void removePacketFile(const QString &packetFile);
    void setFolderName(const QString& folderName);
    void setAPIDs(const QString& apids);
    void toggleAllAPIDs();
    void setH5();
    void setPDS();

signals:
    void ofilesChanged();
    void currentFileChanged();
    void progressChanged();
    void finished();
    void finishedDecode();

private:
    QString m_userName;
    QString m_folderName;
    bool m_allAPIDs;
    std::set<std::string> m_ofiles;
    bool m_debug;
    std::vector<std::string>m_packetFiles;
    QString m_currentFile;
    QString m_progress;
    QString m_selectedAPIDs;
    bool m_H5;
    bool m_PDS;
    h5Decode m_h5Dec;
    pdsDecode m_pdsDec;
    std::vector<uint32_t> getSelectedAPIDs();
};
