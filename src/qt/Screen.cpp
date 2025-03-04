#include "Screen.h"

#include <QGuiApplication>
#include <QScreen>

Screen::Screen() {
    if (qApp) {
        connect(qApp, &QGuiApplication::primaryScreenChanged, this, &Screen::getScreenInfos);

        getScreenInfos(qApp->primaryScreen());
    }
}

Screen::~Screen() {
}

void Screen::getScreenInfos(const QScreen* scr) {
    if (scr) {
        m_scr = scr;

        m_screenDpi = scr->physicalDotsPerInch();
        m_screenPar = scr->devicePixelRatio();

        Q_EMIT screenChanged();
    }
}

Screen* Screen::instance() {
    static Screen screen;
    return &screen;
}