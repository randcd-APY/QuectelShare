#ifndef DEMOSTATUS_H
#define DEMOSTATUS_H

#include <QObject>
#include <QString>

class DemoStatus : public QObject
{
	//  Q_OBJECT

public:
		DemoStatus(QString str);

//signals:
		bool demoStartedFlags();
		//void demoFinishedFlags();

};

#endif
