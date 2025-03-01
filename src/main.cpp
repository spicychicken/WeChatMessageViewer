#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "qt/Screen.h"
#include "qt/WeChatQt.h"
#include "qt/WeChatImageProvider.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    WeChatImageProvider provider;
    engine.addImageProvider("wechatimg", &provider);

    qmlRegisterSingletonType(QUrl("qrc:/qml/ThemeEngine.qml"), "ThemeEngine", 1, 0, "Theme");
    qmlRegisterSingletonType(QUrl("qrc:/qml/WeChatEngine.qml"), "WeChatEngine", 1, 0, "WeChat");

    QQmlContext *engine_context = engine.rootContext();
    engine_context->setContextProperty("pScreen", Screen::instance());
    engine_context->setContextProperty("pWeChat", WeChatQt::instance());

    const QUrl url(QStringLiteral("qrc:/qml/wcmv_mainwindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
