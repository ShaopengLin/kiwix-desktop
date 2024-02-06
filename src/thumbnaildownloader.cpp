#include "thumbnaildownloader.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QIcon>

ThumbnailDownloader::ThumbnailDownloader()
{
    connect(this, &ThumbnailDownloader::oneThumbnailDownloaded, [=]() {
        if (m_downloadQueue.size() != 0)
            downloadThumbnail(m_downloadQueue.takeFirst());
        else
            m_isDownloading = false;
    });
}

ThumbnailDownloader::~ThumbnailDownloader()
{
}

void ThumbnailDownloader::addDownload(QString url, ThumbnailId index)
{
    m_downloadQueue.append({index, url});
    if (!m_isDownloading)
        startDownload();
}

void ThumbnailDownloader::startDownload()
{
    if (m_downloadQueue.size() == 0) {
        m_isDownloading = false;
        return;
    }
    m_isDownloading = true;
    downloadThumbnail(m_downloadQueue.takeFirst());
}

void ThumbnailDownloader::downloadThumbnail(ThumbnailInfo thumbnailInfo)
{
    QNetworkRequest req(thumbnailInfo.second);
    auto reply = manager.get(req);
    connect(reply, &QNetworkReply::finished, this, [=](){
        fileDownloaded(reply, thumbnailInfo);
    });
}

void ThumbnailDownloader::fileDownloaded(QNetworkReply *pReply, ThumbnailInfo thumbnailInfo)
{
    auto downloadedData = pReply->readAll();
    emit oneThumbnailDownloaded(thumbnailInfo.first, thumbnailInfo.second, downloadedData);
    pReply->deleteLater();
}
