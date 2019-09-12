#ifndef MODELMANAGER_H_
#define MODELMANAGER_H_

#include <QObject>

#include "ObjectModel.h"
#include "HotspotElement.h"
#include "WlanThread.h"

class ModelManagerPrivate;

class ModelManager : public QObject
{
    Q_OBJECT
public:
    explicit ModelManager(QObject *parent = nullptr);
    ~ModelManager();

    Q_INVOKABLE void initData();
    Q_INVOKABLE ObjectModel* objectModel();
    Q_INVOKABLE void insert();

    Q_INVOKABLE int enableWlanNetwork();
    Q_INVOKABLE int disableWlanNetwork();
    Q_INVOKABLE int selectEnableNetwork(QString network);
    Q_INVOKABLE int selectDisableNetwork(QString network);
    Q_INVOKABLE QString getHotspotSSID(int index);
    Q_INVOKABLE void getHotspotElement(int index);
    Q_INVOKABLE int connectHotspot(QString ssid, QString psk);
    Q_INVOKABLE int disconnectHotspot();
    Q_INVOKABLE int checkHotspotConnect(QString ssid);

public slots:
    void addNewHotspotItem();
    void networkEventChanged(QString event);
    void currentHotspotChanged(QString name);

signals:
    void signalNetworkEventChanged(QString event);

private:
    QScopedPointer<ModelManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(ModelManager)
};

#endif // MODELMANAGER_H_
