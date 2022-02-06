#include "QtWidgetsClass.h"
#include "ScreenRecorder.h"
#include <QLabel>
#include <QFileDialog>

QtWidgetsClass::QtWidgetsClass(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);
	sc = new ScreenRecorder();
	this->pathText->setText(QString::fromStdString(sc->RecordingPath));
}

QtWidgetsClass::~QtWidgetsClass()
{
}


void QtWidgetsClass::on_RECButton_clicked() {
	recButton->setEnabled(false);
	pauseResumeButton->setEnabled(true);
	stopButton->setEnabled(true);
	std::thread t1(&ScreenRecorder::SetUpScreenRecorder, sc);
	t1.detach();
}

void QtWidgetsClass::on_STOPButton_clicked() {
	sc->StopRecording();
	recButton->setEnabled(true);
	pauseResumeButton->setEnabled(false);
	stopButton->setEnabled(false);
}

void QtWidgetsClass::on_PAUSEButton_clicked() {
	sc->PauseRecording();
	recButton->setEnabled(false);
	pauseResumeButton->setEnabled(true);
	stopButton->setEnabled(true);
}

void QtWidgetsClass::on_RESIZEButton_clicked() {
	scResFr = new ScreenResizeFrame();
	scResFr->show();
	hide();

	//scResFr->close();
}

void QtWidgetsClass::on_PATHButton_clicked() {
	sc->RecordingPath = QFileDialog::getSaveFileName(this, "Pick save location...", "C://output.mp4", ".mp4").toStdString();
	this->pathText->setText(QString::fromStdString(sc->RecordingPath));
}


