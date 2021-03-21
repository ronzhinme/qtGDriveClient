#ifndef GDRIVEHANDLER_H
#define GDRIVEHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>

class GDriveHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList files READ getFiles WRITE setFiles NOTIFY sigFilesChanged);
public:
    enum class RequestType
    {
        CreateFile,
        UploadFile,
        ListFile
    };
    Q_ENUM(RequestType)

    explicit GDriveHandler(QObject *parent = nullptr);
    Q_INVOKABLE void uploadItemRequest(const QUrl &itemUrl,  const QUrl& remoteUrl);
    Q_INVOKABLE void listFilesRequest();
    Q_INVOKABLE void createNewFileRequest(const QUrl &itemUrl);
    QStringList getFiles() const;
    void setFiles(const QStringList &val);

Q_SIGNALS:
    void sigRequestCompleted(RequestType type, const QUrl& itemUrl = QUrl(), const QString &info = "");
    void sigRequestError(RequestType type, const QUrl& itemUrl = QUrl(), const QString &info = "");
    void sigFilesChanged();

private:
    void getFilesInDirectoryRecursive(const QUrl &itemUrl, QList<QUrl> &files);
    bool isAccessManagerValid();
    QSharedPointer<QNetworkAccessManager> m_manager;
    QStringList m_files;
};
Q_DECLARE_METATYPE(GDriveHandler)
#endif // GDRIVEHANDLER_H
