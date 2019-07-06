#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QStringListModel>
#include "music_select.h"
#include "music_thread.h"
#include "music_recard_thread.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QStringListModel *model;
	QString Get_port();
	int Get_flag();
    QString get_dir();


	private slots:
	void on_toolButton_clicked();

	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

	void on_listView_doubleClicked(const QModelIndex &index);

	void threadStart();

	void threadPR();

	void threadplay();

	void changeButton();

	void changeButton_6();

	void threadPR_6();

	void on_pushButton_5_clicked();


private:
	Ui::MainWindow *ui;
	music_recard_thread * Recards;
	music_select * music_select_sss;
	music_thread * myThreads;
	QString str_post;
};


#endif // MAINWINDOW_H
/*#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QStringListModel>
#include "music_select.h"
#include "music_thread.h"
#include "music_recard_thread.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QStringListModel *model;
	QString Get_port();
	int Get_flag();


	private slots:
	void on_toolButton_clicked();

	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

	void on_listView_doubleClicked(const QModelIndex &index);

	void on_pushButton_3_clicked();

	void threadStart();

	void threadPR();

	void threadplay();

	void changeButton();

	void changeButton_6();

	void threadPR_6();

	void on_pushButton_5_clicked();

	void on_pushButton_6_clicked();

private:
	Ui::MainWindow *ui;
	music_select * music_select_sss;
	music_thread * myThreads;
	
};


#endif // MAINWINDOW_H*/
