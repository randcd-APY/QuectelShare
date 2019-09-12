#include "HotspotElement.h"

#include "Config.h"

class HotspotElementPrivate
{
public:
    HotspotElementPrivate(HotspotElement *parent)
        : q_ptr(parent)
    {}

private:
    HotspotElement * const q_ptr;
    Q_DECLARE_PUBLIC(HotspotElement)

    QString m_hotspotSSID;
    QString m_hotspotRSSI; //signal level
    QString m_hotspotState;
};

HotspotElement::HotspotElement(QObject *parent) :
    QObject(parent),
    d_ptr(new HotspotElementPrivate(this))
{

}

HotspotElement::~HotspotElement()
{

}

HotspotElement::HotspotElement(const HotspotElement &other, QObject *parent) :
    QObject(parent),
    d_ptr(new HotspotElementPrivate(this))
{
    d_ptr.swap(const_cast< QScopedPointer<HotspotElementPrivate>&>(other.d_ptr));
}

HotspotElement &HotspotElement::operator=(const HotspotElement &other)
{
    d_ptr.swap(const_cast< QScopedPointer<HotspotElementPrivate>&>(other.d_ptr));
    return *this;
}

QString HotspotElement::hotspotSSID() const
{
    C_D(HotspotElement);
    return d->m_hotspotSSID;
}

void HotspotElement::setHotspotSSID(const QString &ssid)
{
    Q_D(HotspotElement);
    if (ssid != d->m_hotspotSSID) {
        d->m_hotspotSSID = ssid;
        emit hotspotSSIDChanged(ssid);
    }
}

QString HotspotElement::hotspotRSSI() const
{
    C_D(HotspotElement);
    return d->m_hotspotRSSI;
}

void HotspotElement::setHotspotRSSI(const QString &rssi)
{
    Q_D(HotspotElement);
    if (rssi != d->m_hotspotRSSI) {
        d->m_hotspotRSSI = rssi;
        emit hotspotRSSIChanged(rssi);
    }
}

QString HotspotElement::hotspotState() const
{
    C_D(HotspotElement);
    return d->m_hotspotState;
}

void HotspotElement::setHotspotState(const QString &state)
{
    Q_D(HotspotElement);
    if (state != d->m_hotspotState) {
        d->m_hotspotState = state;
        emit hotspotStateChanged(state);
    }
}
