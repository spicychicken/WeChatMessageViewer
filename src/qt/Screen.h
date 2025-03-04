#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <QObject>
#include <QVariantMap>

class QScreen;

class Screen: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int screenDpi READ getScreenDpi NOTIFY screenChanged)
    Q_PROPERTY(double screenPar READ getScreenPar NOTIFY screenChanged)

    int m_screenDpi = -1;
    double m_screenPar = -1.0;

    int getScreenDpi() { return m_screenDpi; }
    double getScreenPar() { return m_screenPar; }

    // Actual screen
    const QScreen* m_scr = nullptr;

    Screen();
    ~Screen();

Q_SIGNALS:
    void screenChanged();

public slots:
    void getScreenInfos(const QScreen* scr);

public:
    static Screen* instance();
};

#endif