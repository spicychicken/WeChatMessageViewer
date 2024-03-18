#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "qtui/WeChatViewerMainWindow.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QTranslator translator;
    /* const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "qtWeChatBKViewer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }*/
    // load zh_CN ts
    if (translator.load("zh_CN", ":/tr/translations/")) {
        app.installTranslator(&translator);
    }

    WeChatViewerMainWindow w;
    w.show();
    return app.exec();
}
