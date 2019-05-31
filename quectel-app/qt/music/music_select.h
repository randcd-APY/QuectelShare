#ifndef MUSIC_SELECT_H
#define MUSIC_SELECT_H

#include <QDir>
#include <QDirIterator>
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QStringListModel>

extern "C"
{
	#include "qlaudio_api.h"
}

class music_select
{
public:
	music_select();

	QStringList addSubFolderImages(QString path);

	QString music_run(QString input);


};

#endif // MUSIC_SELECT_H
