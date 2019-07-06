#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myThreads = new music_thread();
    music_select_sss = new music_select();
    Recards = new music_recard_thread();

    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(changeButton()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(threadPR()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(threadplay()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(threadStart()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(changeButton_6()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete myThreads;
    delete music_select_sss;
    delete Recards;
}

void MainWindow::on_pushButton_clicked()
{
    int row = ui->listView->currentIndex().row();
    if (row != -1)
    {
        QModelIndex index = model->index(row);
        str_post = model->data(index, Qt::DisplayRole).toString();
        model->removeRows(ui->listView->currentIndex().row(), 1);
        QFile::remove(str_post);
    }

}

void MainWindow::on_pushButton_2_clicked()
{
    QString directory = ui->comboBox->currentText();
    if (directory == NULL)
    {
        QMessageBox::information(NULL, "Title", "please add music directory", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        QStringList music_dir = music_select_sss->addSubFolderImages(directory);
        model = new QStringListModel(music_dir);
        qDebug("%d", model->rowCount());
        model->insertRows(model->rowCount() + 1, model->rowCount() + 1);
        ui->listView->setModel(model);

    }
}
QString directorys;
QString MainWindow::get_dir()
{
    return directorys;
}

void MainWindow::on_toolButton_clicked()
{
    directorys = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("save path"), QDir::currentPath()));
    if (!directorys.isEmpty())
    {
        if (ui->comboBox->findText(directorys) == -1)
        {
            ui->comboBox->addItem(directorys);
        }
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(directorys));
        QStringList music_dir = music_select_sss->addSubFolderImages(directorys);
        model = new QStringListModel(music_dir);
        qDebug("%d", model->rowCount());
        model->insertRows(model->rowCount() + 1, model->rowCount() + 1);
        ui->listView->setModel(model);

    }
}


void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    //music_select *music = new music_select();
    Audio_Playback audio;
    audio.rate = DefualtPlayRate;
    audio.device = DefaultOutputDevice;
    audio.channel = DefaultChannel;
    audio.volume = 10;
    ui->listView->currentIndex().row();
    QString str = model->data(index, Qt::DisplayRole).toString();
    audio.filetype = Mp3;
    QByteArray ba;
    ba = str.toLatin1();
    QL_Audio_Playback(audio, ba.data());
    //str_post = str;
    //myThreads->start();
    //thread1->terminate();
    //music->music_run(str_post);
    //delete music;
    //QMessageBox::information(NULL,tr("information"),str);
}

void MainWindow::threadStart()
{
    myThreads->threadStart();

}

void MainWindow::changeButton()
{
    myThreads->threadPause();
    /*if (!QString::compare(ui->pushButton_3->text(), QString::fromUtf8("Stop_playing")))
    {
        ui->pushButton_3->setText(QString::fromUtf8("continue"));
    }
    else
    {
        ui->pushButton_3->setText(QString::fromUtf8("Stop_playing"));
    }*/
}

void MainWindow::changeButton_6()
{
    if (!QString::compare(ui->pushButton_6->text(), QString::fromUtf8("Record")))
    {
        Recards->threadStart();
        ui->pushButton_6->setText(QString::fromUtf8("Record_end"));
    }
    else
    {
        Recards->threadStop();
        ui->pushButton_6->setText(QString::fromUtf8("Record"));
    }
}

void MainWindow::threadPR_6()
{
    Recards->threadPR();
}

void MainWindow::threadPR()
{
    myThreads->threadPR();
}
QString str_path;
void MainWindow::threadplay()
{
    int row = ui->listView->currentIndex().row();
    if (row != -1)
    {
        QModelIndex index = model->index(row);
        str_post = model->data(index, Qt::DisplayRole).toString();
        str_path = str_post;
    }
}

QString MainWindow::Get_port()
{
    return str_path;
}

int MainWindow::Get_flag()
{
    return 0;
}

void MainWindow::on_pushButton_5_clicked()
{
    this->close();
}

