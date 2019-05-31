#ifndef GALLERY_H
#define GALLERY_H

#include <QMainWindow>
#include "ffmpeg_player.h"


namespace Ui {
class Gallery;
}

class Gallery : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gallery(QWidget *parent = 0);
    ~Gallery();

private:
    Ui::Gallery *ui;
	QRect vidRect;
	QRect blackBgRect;
	VideoPlayer *vidPlayer;
	uint8_t *vidFrame;
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

    void on_BackButton_clicked();
};

#endif // GALLERY_H
