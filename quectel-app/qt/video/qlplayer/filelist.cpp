#include "filelist.h"
#include "ui_filelist.h"
#include "gallery.h"
#include <unistd.h>

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
    list_name<<"*.mp4"<<"*.avi"<<"*.flv"<<"*.mkv";
    model->setNameFilters(list_name);
    ui->listView->setMovement(QListView::Static);
    ui->listView->setViewMode(QListView::ListMode);//IconMode
    ui->listView->setGridSize(QSize(100,100));
    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->setRootPath("/data/misc/camera/video/"));

    ui->closeButton->setGeometry(50, 1200, 80, 80);
    ui->closeButton->setText("");
    ui->closeButton->setStyleSheet("border-image:url(:/back.jpg)");

    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(videoListShow(QModelIndex)));
}

fileList::~fileList()
{
    delete ui;
}

void fileList::videoListShow(const QModelIndex &index)
{
    filepath.clear();
    filepath.append("/data/misc/camera/video/").append(index.data(Qt::DisplayRole).toString());
    newGallery = new Gallery(this);
	QPalette pal(newGallery->palette());

	pal.setColor(QPalette::Background, Qt::black); //设置背景黑色
    newGallery->setAutoFillBackground(true);
    newGallery->setPalette(pal);
    newGallery->show();

    return;
}

QString GETPPATH::getVidPath()
{
    QString getFilePath = filepath;
    return getFilePath;
}

void fileList::on_closeButton_clicked()
{
	printf("on_closeButton_clicked close\n");
	if (newGallery != nullptr)
	{
		printf("newGallery close\n");
		newGallery->Close();
		delete newGallery;
		newGallery = nullptr;
	} else
	{
    	this->close();
	}
}
