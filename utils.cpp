#include "utils.h"
#include <QDir>

void Utils::getFilesInDirectoryRecursive(const QUrl &itemUrl, QList<QUrl> &files)
{
    QDir dir(itemUrl.toLocalFile());
    if(dir.exists())
    {
        for(auto i: dir.entryList(QDir::Files | QDir::AllDirs))
        {
            if(i.compare(".") == 0 || i.compare("..") == 0)
            {
                continue;
            }

            getFilesInDirectoryRecursive(QUrl(itemUrl.toString()+"/"+i), files);
        }
    }
    else
    {
        files.emplaceBack(itemUrl);
    }
}

void Utils::debugRequest(QNetworkRequest request)
{
#if defined (QT_DEBUG)
  qDebug() << request.url();
  const QList<QByteArray>& rawHeaderList(request.rawHeaderList());
  foreach (QByteArray rawHeader, rawHeaderList) {
    qDebug() << rawHeader << ":" << request.rawHeader(rawHeader);
  }
#endif
}
