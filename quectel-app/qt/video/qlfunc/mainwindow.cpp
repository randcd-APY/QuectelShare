#include "mainwindow.h"
#include "ExamplePaintSurface.h"
#include <QPainter>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	this->setGeometry(0, 0, 300, 200);
}

MainWindow::~MainWindow()
{

}

void MainWindow::paintEvent(QPaintEvent *e)
{
    ExamplePaintSurface paintSurface;
    paintSurface.resize(300, 200);
    paintSurface.render();
    QImage image = paintSurface.grabFramebuffer();

	image.save("p.png");
    QPainter qp;
    qp.begin(this);
    qp.drawImage(rect(), image, image.rect());
    qp.end();
	printf("paintEvent end");
}
