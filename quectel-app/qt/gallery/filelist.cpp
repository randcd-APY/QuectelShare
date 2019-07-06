#include "filelist.h"
#include "ui_filelist.h"
#include "gallery.h"

Gallery *newGallery;
QString filepath;
fileList::fileList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileList)
{
    ui->setupUi(this);

    QFileSystemModel* model= new QFileSystemModel();
    model->setNameFilterDisables(false);
    model->setFilter(QDir::Dirs|QDir::Drives|QDir::Files|QDir::NoDotAndDotDot);
    QStringList list_name;
    list_name<<"*.jpg"<<"*.gif"<<"*.png"<<"*.bmp";
    model->setNameFilters(list_name);
    //model->setRootPath("E:\\");
    ui->listView->setMovement(QListView::Static);
    ui->listView->setViewMode(QListView::IconMode);//ListMode);//IconMode
    ui->listView->setGridSize(QSize(200,200));
    ui->listView->setModel(model);
//    ui->listView->setRootIndex(model->setRootPath("Y:\\work\\SC20_Android8.1_R07_r00043_linux\\"));
    ui->listView->setRootIndex(model->setRootPath("/data/DCIM/"));

    ui->closeButton->setGeometry(50, 1200, 80, 80);
    ui->closeButton->setText("");
    ui->closeButton->setStyleSheet("border-image:url(:/back.jpg)");

    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(pictureShow(QModelIndex)));
}

fileList::~fileList()
{
    delete ui;
}

void fileList::pictureShow(const QModelIndex &index)
{
        filepath.clear();
//        filepath.append("Y\:\\work\\SC20_Android8.1_R07_r00043_linux\\").append(index.data(Qt::DisplayRole).toString());
        filepath.append("/data/DCIM/").append(index.data(Qt::DisplayRole).toString());
        newGallery = new Gallery(this);
        newGallery->show();
        return;
        //barnett change
        QImage image;
        image.load(filepath);
        ui->listShow->setPixmap(QPixmap(filepath));
        ui->listShow->setGeometry((720-image.width())/2,(1280-image.height())/2,image.width(),image.height());
        ui->listShow->show();

}

QString GETPPATH::getPicPath()
{
    QString getFilePath = filepath;
    return getFilePath;
}

void fileList::on_closeButton_clicked()
{
    this->close();
}
