#include "QtWidgetsClass.h"
#include "ScreenRecorder.h"
#include <QLabel>
#include <QFileDialog>
#include <stdlib.h>
#include <algorithm>
#include <string>
#if linux
#include <X11/Xlib.h>
#endif
#if WIN32
#include <Windows.h>
#include <shellapi.h>
#pragma comment(lib, "shell32")
#endif



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
	sc->recordAudio = checkBox->isChecked();
	std::thread t1(&ScreenRecorder::SetUpScreenRecorder, sc);
	t1.detach();
	this->showMinimized();
}

void QtWidgetsClass::on_STOPButton_clicked() {
	sc->StopRecording();
	recButton->setEnabled(true);
	pauseResumeButton->setEnabled(false);
	stopButton->setEnabled(false);
}

void QtWidgetsClass::on_PAUSEButton_clicked() {
	sc->PauseRecording();
	this->showMinimized();
	recButton->setEnabled(false);
	pauseResumeButton->setEnabled(true);
	stopButton->setEnabled(true);
}

void QtWidgetsClass::on_RESIZEButton_clicked() {
	scResFr = new ScreenResizeFrame(Q_NULLPTR, sc);
	scResFr->show();
}

void QtWidgetsClass::on_PATHButton_clicked() {
	QString path = QFileDialog::getSaveFileName(this, "Pick save location...", QString::fromStdString(sc->RecordingPath), ".mp4");
	if(path.isEmpty()) {
#if WIN32
		sc->RecordingPath = "..\\media\\output.mp4";
#elif
		sc->RecordingPath = "../media//output.mp4";
#endif
	}
	else {
		sc->RecordingPath = path.toStdString();
	}

	this->pathText->setText(QString::fromStdString(sc->RecordingPath));
}


void QtWidgetsClass::on_FULLSCREENButton_clicked()
{
	sc->cropX = 0;
	sc->cropY = 0;
#if linux
	Display* disp = XOpenDisplay(NULL);
	Screen* scrn = DefaultScreenOfDisplay(disp);
	sc->cropH = scrn->height;
	sc->cropW = scrn->width;
#endif
#if WIN32
	sc->cropW = GetSystemMetrics(SM_CXSCREEN);
	sc->cropH = GetSystemMetrics(SM_CYSCREEN);
#endif
}

void QtWidgetsClass::on_OPENPATHButton_clicked()
{
	std::string directory;
	size_t last_slash_idx = sc->RecordingPath.rfind('\\');
	if(last_slash_idx == std::string::npos) last_slash_idx = sc->RecordingPath.rfind('/');
	if(last_slash_idx == std::string::npos) last_slash_idx = sc->RecordingPath.rfind('//');
	if (std::string::npos != last_slash_idx)
	{
		directory = sc->RecordingPath.substr(0, last_slash_idx);
	}
	ShellExecute(NULL, L"open", string_to_wstring(directory).c_str(), NULL, NULL, SW_NORMAL);
}

std::wstring QtWidgetsClass::string_to_wstring(const std::string& text) {
	return std::wstring(text.begin(), text.end());
}




