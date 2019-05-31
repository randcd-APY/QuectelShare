#include "gps.h"
#include "ui_gps.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QTableView>
#include <QStandardItemModel>




Location gpsloc_g;
GnssSvNotification Gsv_g;
bool gps_status=false;
QStandardItemModel  *model = new QStandardItemModel();




void quec_location_cb (Location loc){
        gpsloc_g=loc;
        gps_status=true;
    }

void quec_nmea_cb(uint64_t time,const char *nmea,int len)
{
    qDebug("nmea:%s",nmea);
}

void quec_sv_cb(GnssSvNotification Gsv)
{
    Gsv_g=Gsv;
}

QlLocationCallbacks Qlc={
    quec_location_cb,
    quec_nmea_cb,
    quec_sv_cb

};

gps::gps(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gps)
{
    ui->setupUi(this);
    QTimer *gpstimer=new QTimer(this);
    connect(gpstimer,&QTimer::timeout,this,&gps::gps_update);
    gpstimer->start(1000);
    ui->ButtonStop->setEnabled(false);
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("SV"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("ID"));
    model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("CN0"));
    ui->tableSvInfo->setModel(model);
    ui->tableSvInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableSvInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


}

gps::~gps()
{
    delete ui;
}

myThread::myThread()
{

}
void myThread::run()
{
    QL_Gnss_Start();
}

void gps::on_ButtonStart_clicked()
{
    ui->ButtonStart->setEnabled(false);
    QL_Gnss_Init(Qlc);
    myThread *thread1 = new myThread;
       thread1->start();
    ui->ButtonStop->setEnabled(true);
}



void gps::gps_update()
{
    QString str;
    QString SvType[10]={"","GPS","SBAS","GLONASS","QZSS","BEIDOW","GALILEO"};

    if(gps_status)
    {
        str.sprintf("Fixed:LAT=%f LON=%f",gpsloc_g.latitude,gpsloc_g.longitude);
        ui->label_fix->setText(str);
	gps_status=false;
   }else{
        ui->label_fix->setText("No Fix");
    }
    model->removeRows(0,model->rowCount());
    for(int i = 0; i < Gsv_g.count; i++)
    {
        model->setItem(i,0,new QStandardItem(SvType[Gsv_g.gnssSvs[i].type]));
        str.sprintf("%d",Gsv_g.gnssSvs[i].svId);
        model->setItem(i,1,new QStandardItem(str));
        str.sprintf("%.1f",Gsv_g.gnssSvs[i].cN0Dbhz);
        model->setItem(i,2,new QStandardItem(str));
    }
    ui->tableSvInfo->sortByColumn(2,Qt::DescendingOrder);
    //ui->label_nmea->setText(GNmea_g.nmea);


}

void gps::on_ButtonExit_clicked()
{
    QL_Gnss_Stop();
    close();
}

void gps::on_ButtonStop_clicked()
{
    ui->ButtonStop->setEnabled(false);
    QL_Gnss_Stop();
    ui->ButtonStart->setEnabled(true);
}


