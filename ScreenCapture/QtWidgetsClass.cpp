#include "QtWidgetsClass.h"
#include "ScreenRecorder.h"

QtWidgetsClass::QtWidgetsClass(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);
	ScreenRecorder* sc = new ScreenRecorder();
}

QtWidgetsClass::~QtWidgetsClass()
{
}


void QtWidgetsClass::on_RECButton_clicked() {
	//sc.SetUpScreenRecorder(sc);
	this->recButton->setEnabled(false);
	this->pauseResumeButton->setEnabled(true);
	this->stopButton->setEnabled(true);
}

void QtWidgetsClass::on_STOPButton_clicked() {
	sc.StopRecording(sc);
}

void QtWidgetsClass::on_PAUSEButton_clicked() {
	sc.PauseRecording(sc);
}

void QtWidgetsClass::on_RESIZEButton_clicked() {
	scResFr = new ScreenResizeFrame(this);
	scResFr->show();

	//scResFr->close();
}


