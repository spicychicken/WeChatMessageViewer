#ifndef _WECHAT_IMAGE_PROVIDER_H_
#define _WECHAT_IMAGE_PROVIDER_H_

#include <QQuickImageProvider>
#include <QImage>
 
class WeChatImageProvider : public QQuickImageProvider {
public:
    WeChatImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}
 
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

};

#endif