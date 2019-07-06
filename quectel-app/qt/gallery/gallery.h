#ifndef GALLERY_H
#define GALLERY_H

#include <QMainWindow>


namespace Ui {
class Gallery;
}

class Gallery : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gallery(QWidget *parent = 0);
    ~Gallery();
	int ScreenW;
	int ScreenH;
	void getScreenInfo();
private:
    Ui::Gallery *ui;

    void selectFile(QString fileName);       //弹出选择文件对话框

private slots:
    void fileOpenActionSlot(QString fileName);//打开文件动作对应的槽函数

    void on_BackButton_clicked();
};

#endif // GALLERY_H
