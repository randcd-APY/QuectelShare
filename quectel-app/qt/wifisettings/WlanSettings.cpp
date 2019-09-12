#include "WlanSettings.h"

WlanSettings::WlanSettings(QObject *parent) : QObject(parent)
{
    p_wlanThread = new WlanThread(this);
    p_wlanThread->start();
}

void WlanSettings::slideSwitch(bool enable)
{

}

int WlanSettings::scanHotspot()
{
    return 0;
}

int WlanSettings::connectHotspot(QString ssid, QString psk)
{
    return 0;
}
