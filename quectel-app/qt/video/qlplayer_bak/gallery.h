#ifndef GALLERY_H
#define GALLERY_H

#include <QMainWindow>
#include "videoplayer.h"


namespace Ui {
class Gallery;
}

class Gallery : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gallery(QWidget *parent = 0);
    ~Gallery();
	void Close();

private:
    Ui::Gallery *ui;
	QRect vidRect;
	QRect blackBgRect;
	VideoPlayer *videoPlayer;
	uint8_t *vidFrame = nullptr;
	int vidWidth;
	int vidHeight;
	bool isPlaying;
	bool isFirstPlay;

    void selectFile(QString fileName);       //弹出选择文件对话框
	void paintEvent(QPaintEvent *event);
	static void updateScreen(uint8_t* frameRGB, int width, int height);
    static void cleanScreen();

private slots:
    void fileOpenActionSlot(QString fileName);//打开文件动作对应的槽函数

};

#endif // GALLERY_H
