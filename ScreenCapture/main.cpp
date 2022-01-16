//#include <bits/stdc++.h>
#include "ScreenRecorder.h"
#include "QtWidgetsClass.h"
#include <QtWidgets/QApplication>
using namespace std;


/* driver function to run the application */
int main(int argc, char* argv[])
{
	ScreenRecorder screen_record;

	QApplication a(argc, argv);
	QtWidgetsClass w;
	w.show();
	a.exec();

	screen_record.openVideoDevice();
	screen_record.openAudioDevice();
	screen_record.initOutputFile();
	screen_record.CreateThreads();

	cout << "\nprogram executed successfully\n";

	return 0;
}
