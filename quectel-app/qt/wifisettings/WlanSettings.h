#ifndef WLANSETTINGS_H
#define WLANSETTINGS_H

#include <QObject>

#include "WlanThread.h"

class WlanSettings : public QObject
{
    Q_OBJECT
public:
    explicit WlanSettings(QObject *parent = nullptr);

    Q_INVOKABLE void slideSwitch(bool enable);

    int scanHotspot(void);
    int obtainHotspotList(void);
    int connectHotspot(QString ssid, QString psk);

signals:

public slots:
private:
    WlanThread *p_wlanThread;
};

#endif // WLANSETTINGS_H
