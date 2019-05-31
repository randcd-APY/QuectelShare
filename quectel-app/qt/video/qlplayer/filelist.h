#ifndef FILELIST_H
#define FILELIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QDir>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDebug>
#include <QFileSystemModel>
#include "videoplayer.h"


namespace Ui {
class fileList;
}

class fileList : public QWidget
{
    Q_OBJECT

public:
    explicit fileList(QWidget *parent = 0);
    ~fileList();
    void getVidPath(QString);

private:
    Ui::fileList *ui;

//    QListView *m_file_list_view;
//    QStringListModel *m_string_list_model;
//    QSortFilterProxyModel *m_proxy_model;

public slots:
    void videoListShow(const QModelIndex &index);


private slots:
    void on_closeButton_clicked();
};

class GETPPATH {

public:
    QString getVidPath();

};

#endif // FILELIST_H
