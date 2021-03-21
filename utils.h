#ifndef UTILS_H
#define UTILS_H

#include <QUrl>
#include <QNetworkRequest>

class Utils
{
public:
    static void getFilesInDirectoryRecursive(const QUrl &itemUrl, QList<QUrl> &files);
    static void debugRequest(QNetworkRequest request);
};

#endif // UTILS_H
