#ifndef OBJECTMODEL_H_
#define OBJECTMODEL_H_

#include <QAbstractListModel>

class ObjectModelPrivate;

typedef QList<QObject* > QObjectList;

class ObjectModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ObjectModel(QObject *parent = nullptr);
    ~ObjectModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual QHash<int, QByteArray> roleNames() const;

    void insert(int index, QObject* object);
    void append(QObject* object);
    void push_front(QObject* object);
    void push_back(QObject* object);

    void replace(int index, QObject* object);
    void set(QObjectList * objectList);

    void remove(QObject* object);
    void remove(int index);
    void clear();

    Q_INVOKABLE QObject * get(int index);
    QObjectList* getAll();

private:
    QScopedPointer<ObjectModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(ObjectModel)
};

#endif // OBJECTMODEL_H_
