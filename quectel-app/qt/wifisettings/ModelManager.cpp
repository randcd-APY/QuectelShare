#include "ModelManager.h"
#include <QThread>
#include <QDateTime>
#include <QDebug>

class ModelManagerPrivate
{
public:
    ModelManagerPrivate(ModelManager *parent)
        : q_ptr(parent)
        , objectModel(nullptr)
    {
    }

    void init();
    void exit();

private:
    ModelManager * const q_ptr;
    Q_DECLARE_PUBLIC(ModelManager)

    ObjectModel* objectModel;
    QThread workerThread;
    WlanThread *p_wlanThread;
};

ModelManager::ModelManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ModelManagerPrivate(this))
{
    Q_D(ModelManager);
    d->init();
//    initData();
    if (d->p_wlanThread != nullptr) {
        connect(d->p_wlanThread, SIGNAL(signalScanNewHotspot()), this, SLOT(addNewHotspotItem()));
        connect(d->p_wlanThread, SIGNAL(signalNetworkEvent(QString)), this, SLOT(networkEventProcess(QString)));
        connect(d->p_wlanThread, SIGNAL(signalCurrentHotspotChanged(QString)), this, SLOT(currentHotspotChanged(QString)));
    }
}

ModelManager::~ModelManager()
{
    Q_D(ModelManager);
    d->exit();
}

void ModelManager::initData()
{
    Q_D(ModelManager);
    QObjectList testItemList;
    for (int i=0; i<3; ++i) {
        HotspotElement* newTestItem = new HotspotElement(this);
        newTestItem->setHotspotSSID("peeta_" + QString::number(i));
        newTestItem->setHotspotRSSI(QString::number(i));
        testItemList.append(newTestItem);
    }
    d->objectModel->set(&testItemList);
}

ObjectModel *ModelManager::objectModel()
{
    Q_D(ModelManager);
    return d->objectModel;
}

void ModelManager::insert()
{
    Q_D(ModelManager);
    HotspotElement* newTestItem = new HotspotElement(this);
    newTestItem->setHotspotSSID("peeta");
    newTestItem->setHotspotRSSI("7");
    d->objectModel->append(newTestItem);
}

int ModelManager::enableWlanNetwork()
{
    Q_D(ModelManager);

    return d->p_wlanThread->enableNetwork();
}

int ModelManager::disableWlanNetwork()
{
    Q_D(ModelManager);

    d->p_wlanThread->getHotspotHash()->clear();

    return d->p_wlanThread->disableNetwork();
}

int ModelManager::selectEnableNetwork(QString network)
{
    Q_D(ModelManager);
    return d->p_wlanThread->selectEnableNetwork(network);
}

int ModelManager::selectDisableNetwork(QString network)
{
    Q_D(ModelManager);
    return d->p_wlanThread->selectDisableNetwork(network);
}

QString ModelManager::getHotspotSSID(int index)
{
    Q_D(ModelManager);
    QObject *obj = d->objectModel->get(index);
    HotspotElement *item = qobject_cast<HotspotElement *>(obj);
//    qDebug() << "peeta: " << item->hotspotSSID();
    return item->hotspotSSID();
}

void ModelManager::getHotspotElement(int index)
{
    Q_D(ModelManager);
    QObject *obj = d->objectModel->get(index);
    HotspotElement *item = qobject_cast<HotspotElement *>(obj);
    qDebug() << "peeta: " << item->hotspotSSID();
    qDebug() << "peeta: " << item->hotspotRSSI();
}

int ModelManager::connectHotspot(QString ssid, QString psk)
{
    Q_D(ModelManager);
    return d->p_wlanThread->connectHotspot(ssid, psk);
}

int ModelManager::disconnectHotspot()
{
    Q_D(ModelManager);
    return d->p_wlanThread->disconnectHotspot();
}

int ModelManager::checkHotspotConnect(QString ssid)
{
    Q_D(ModelManager);

    if (d->p_wlanThread->getCurrentHotspotName().compare(ssid) == 0)
        return 0;
    return -1;
}

void ModelManager::addNewHotspotItem()
{
    Q_D(ModelManager);

    int i = 0;
    int j = 0; //Control listView Size
    QHash<QString, int> *hotspotHash = d->p_wlanThread->getHotspotHash();
    if (hotspotHash->isEmpty()) {
        d->objectModel->clear();
        return;
    }

    QHash<int, QString> *networklistHash = d->p_wlanThread->getNetworkListHash();
    if (networklistHash == nullptr) {
        return;
    }

    QHash<QString, int>::const_iterator it;//迭代器的使用,it一开始指向的是第0个元素之前的位置
    for(it = hotspotHash->constBegin(), j = 0; it != hotspotHash->constEnd() && j < 30; ++it, j++){
        HotspotElement* newTestItem = new HotspotElement(this);
        newTestItem->setHotspotSSID(it.key());
        newTestItem->setHotspotRSSI(QString::number(it.value()));

        QHash<int, QString>::const_iterator it2;
        for(it2 = networklistHash->constBegin(); it2 != networklistHash->constEnd(); ++it2){
            if (it2.value().compare(it.key()) == 0) {
                if(it2.value().compare(d->p_wlanThread->getCurrentHotspotName()) == 0)
                    newTestItem->setHotspotState("Connected");
                else
                    newTestItem->setHotspotState("Disconnected");
            } else {
                newTestItem->setHotspotState("Encryption");
            }
        }

        for(i = 0; i < d->objectModel->getAll()->size(); ++i) {
            QObject *obj = d->objectModel->get(i);
            HotspotElement *item = qobject_cast<HotspotElement *>(obj);
            if (QString::compare(item->hotspotSSID(), it.key()) == 0) {
                break;
            }
        }

        if (newTestItem->hotspotState().compare("Connected") == 0) {
            QObject *obj2 = d->objectModel->get(0);
            HotspotElement *item2 = qobject_cast<HotspotElement *>(obj2);
            if(item2->hotspotState().compare("Connected") == 0)
                d->objectModel->replace(0, newTestItem);
            else {
                d->objectModel->push_front(newTestItem);
                emit signalNetworkEventChanged("Connected");
            }
        } else if (i != d->objectModel->getAll()->size()) {
            d->objectModel->replace(i, newTestItem);
        } else {
            d->objectModel->append(newTestItem);
        }
    }

    hotspotHash->clear();
}

void ModelManager::networkEventChanged(QString event)
{
//    Q_D(ModelManager);
    emit signalNetworkEventChanged(event);
}

void ModelManager::currentHotspotChanged(QString name)
{
    Q_D(ModelManager);

    for(int i = 0; i < d->objectModel->getAll()->size(); ++i) {
        QObject *obj = d->objectModel->get(i);
        HotspotElement *item = qobject_cast<HotspotElement *>(obj);
        if (QString::compare(item->hotspotSSID(), name) == 0) {
            item->setHotspotState("Connected");
            d->objectModel->replace(i, item);
            qDebug() << "Peeta+++++++++";
            break;
        }
    }
}

void ModelManagerPrivate::init()
{
    Q_Q(ModelManager);
    if (objectModel == nullptr) {
        objectModel = new ObjectModel(q);
    }

    if (p_wlanThread == nullptr) {
        p_wlanThread = new WlanThread(q);
        p_wlanThread->start();
    }
}

void ModelManagerPrivate::exit()
{
    if (p_wlanThread == nullptr) {
        p_wlanThread->requestInterruption();
        p_wlanThread->deleteLater();
        p_wlanThread = nullptr;
    }

    if (objectModel) {
        objectModel->deleteLater();
        objectModel = nullptr;
    }
}
