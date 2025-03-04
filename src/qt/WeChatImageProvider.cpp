#include "WeChatImageProvider.h"

#include "wechat/WeChatContext.h"

QImage WeChatImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    const auto& imageData = sWECHAT.loadMsgImgData(QUrl::fromPercentEncoding(id.toUtf8()).toStdString());

    return QImage::fromData(QByteArray::fromRawData(imageData.c_str(), imageData.length()));
}