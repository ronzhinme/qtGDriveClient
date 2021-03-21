#include "gdrivehandler.h"
#include "utils.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpPart>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeDatabase>

const QString TOKEN = "ya29.a0AfH6SMBiGozDZji9bSdw7tJ58xuNPkq2bHVYmyfny1N7PDnpac-oCtBdX3UeYdQqrKVW5AwCDNsUv4bDtN2r37RsjtCNdfFOJ3iKvA84dpQ6Apk5r7--fBd4dVnMm1G-enNU4m9cBaf16glUmbo-UGy_iRRZ";
const QString FILES_URL="https://www.googleapis.com/drive/v3/files";
const QString RESUMABLE_OPTION = "uploadType=resumable";
const QString UPLOAD_URL = "https://www.googleapis.com/upload/drive/v3/files";

GDriveHandler::GDriveHandler(QObject *parent)
    : QObject(parent)
{
    m_manager.reset(new QNetworkAccessManager(this));
}

bool GDriveHandler::isAccessManagerValid()
{
    return m_manager.data() != nullptr;
}

QStringList GDriveHandler::getFiles() const
{
    return m_files;
}

void GDriveHandler::setFiles(const QStringList &val)
{
    m_files.clear();
    m_files.append(val);
    Q_EMIT sigFilesChanged();
}

void GDriveHandler::listFilesRequest()
{
    if(!isAccessManagerValid())
            return;

    QString tokenStr = "Bearer " + TOKEN;
    QNetworkRequest newRequest(FILES_URL);
    newRequest.setRawHeader("Authorization", tokenStr.toUtf8());
    newRequest.setRawHeader("Content-Type", "application/json; charset=utf-8");
    newRequest.setRawHeader("Accept", "application/json");

    QNetworkReply* reply = m_manager->get(newRequest);
    connect(reply, &QNetworkReply::finished, [reply, newRequest, this]()
    {
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QNetworkReply::NetworkError err = reply->error();
        if ((err != QNetworkReply::NoError) || (httpStatus == 0))
        {
            Q_EMIT sigRequestError(RequestType::ListFile, QString(""), QString::number(httpStatus));
            return;
        }

        QByteArray ret = reply->readAll();
        reply->abort();
        reply->deleteLater();

        if (ret.isEmpty())
        {
            return;
        }

        QJsonDocument jsdoc;
        jsdoc = QJsonDocument::fromJson(ret);
        QJsonObject jsobj = jsdoc.object();

        if(!m_files.isEmpty())
        {
            m_files.clear();
        }

        for(auto i : jsobj["files"].toArray())
        {
            auto fname = i.toObject()["name"].toString();
            m_files.append(fname);
            Q_EMIT sigFilesChanged();
        }

        Q_EMIT sigRequestCompleted(RequestType::ListFile);
    });
}

void GDriveHandler::createNewFileRequest(const QUrl &itemUrl)
{
    if(!isAccessManagerValid())
        return;

    QString tokenStr = "Bearer " + TOKEN;

    QList<QUrl> files;
    Utils::getFilesInDirectoryRecursive(itemUrl, files);

    for(auto f : files)
    {
        QNetworkRequest newRequest((QUrl(UPLOAD_URL +"?"+ RESUMABLE_OPTION)));
        newRequest.setRawHeader("Authorization", tokenStr.toUtf8());
        newRequest.setRawHeader("Accept", "application/json");
        newRequest.setRawHeader("Content-Type", "application/json");

        QJsonObject jobj;
        jobj["name"] = QFileInfo(QFile(f.toLocalFile())).fileName();

        QNetworkReply* reply = m_manager->post(newRequest, QJsonDocument(jobj).toJson());
        connect(reply, &QNetworkReply::finished, [reply, newRequest, this, f]()
        {
            int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QNetworkReply::NetworkError err = reply->error();
            if ((err != QNetworkReply::NoError) || (httpStatus == 0))
            {
#if defined (QT_DEBUG)
                qDebug() << "Error:" << err << " " << httpStatus;
#endif
                Q_EMIT sigRequestError(RequestType::CreateFile, f);
            }
            else
            {
                QByteArray ret = reply->readAll();
                reply->abort();
                reply->deleteLater();

                auto location = reply->header(QNetworkRequest::LocationHeader);
                Q_EMIT sigRequestCompleted(RequestType::CreateFile, f, location.toString());
            }
        });
    }
}

void GDriveHandler::uploadItemRequest(const QUrl &itemUrl, const QUrl& remoteUrl)
{
    if(!isAccessManagerValid())
        return;

    QString tokenStr = "Bearer " + TOKEN;
    QNetworkRequest newRequest(remoteUrl);

    auto file = new QFile(itemUrl.toLocalFile());
    if(file->open(QIODevice::ReadOnly))
    {
        QMimeDatabase mimeDB;
        QMimeType mime = mimeDB.mimeTypeForFile(file->fileName());
        QByteArray mimeType = mime.name().toUtf8();

        newRequest.setRawHeader("X-Upload-Content-Type", mimeType);
        newRequest.setRawHeader("Content-Length", QByteArray::number(file->size()));
        newRequest.setRawHeader("Content-Type", "application/json; charset=UTF-8");
        newRequest.setRawHeader("Authorization", tokenStr.toUtf8());

        QNetworkReply* reply = m_manager->put(newRequest, file->readAll());
        file->close();
        connect(reply, &QNetworkReply::finished, [reply, newRequest, this, itemUrl, remoteUrl]()
        {
            int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QNetworkReply::NetworkError err = reply->error();
            if ((err != QNetworkReply::NoError) || (httpStatus == 0))
            {
                qDebug() << httpStatus << err;
                Q_EMIT sigRequestError(RequestType::UploadFile, remoteUrl, itemUrl.toString());
            }
            else
            {
                Q_EMIT sigRequestCompleted(RequestType::UploadFile, remoteUrl, itemUrl.toString());
            }
        });
    }
}

