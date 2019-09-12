#ifndef HOTSPOTELEMENT_H
#define HOTSPOTELEMENT_H

#include <QObject>

class HotspotElementPrivate;

class HotspotElement : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hotspotSSID READ hotspotSSID WRITE setHotspotSSID NOTIFY hotspotSSIDChanged)
    Q_PROPERTY(QString hotspotRSSI READ hotspotRSSI WRITE setHotspotRSSI NOTIFY hotspotRSSIChanged)
    Q_PROPERTY(QString hotspotState READ hotspotState WRITE setHotspotState NOTIFY hotspotStateChanged)
public:
    explicit HotspotElement(QObject *parent = nullptr);
    ~HotspotElement();

    HotspotElement(const HotspotElement &other, QObject *parent = nullptr);
    HotspotElement& operator=(const HotspotElement &other);

    QString hotspotSSID() const;
    void setHotspotSSID(const QString& ssid);

    QString hotspotRSSI() const;
    void setHotspotRSSI(const QString& rssi);

    QString hotspotState() const;
    void setHotspotState(const QString& state);

signals:
    void hotspotSSIDChanged(QString ssid);
    void hotspotRSSIChanged(QString rssi);
    void hotspotStateChanged(QString state);

public slots:

private:
    QScopedPointer<HotspotElementPrivate> d_ptr;
    Q_DECLARE_PRIVATE(HotspotElement)
};

#endif // HOTSPOTELEMENT_H
