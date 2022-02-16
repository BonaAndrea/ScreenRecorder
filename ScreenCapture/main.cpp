//#include <bits/stdc++.h>
#include "ScreenRecorder.h"
#if QT
#include "QtWidgetsClass.h"
#include <QtWidgets/QApplication>
#endif
using namespace std;


/* driver function to run the application */
int main(int argc, char* argv[])
{

#if QT
	QApplication a(argc, argv);
	QtWidgetsClass w;
	w.setWindowTitle(QString("Vola mio mini recorder"));

	w.show();
	a.exec();
	
#endif

#if !QT
	screen_record.openVideoDevice();
	screen_record.openAudioDevice();

	screen_record.initOutputFile();
	screen_record.CreateThreads();
#endif

	return 0;
}
