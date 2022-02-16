#include "QtWidgetsClass.h"
#include "ScreenRecorder.h"
#include <QLabel>
#include <QFileDialog>
#include <QTimer>
#include <stdlib.h>
#include <algorithm>
#include <string>
#if linux
#include <X11/Xlib.h>
#include <QDesktopServices>
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX
#endif
#if WIN32
#include <Windows.h>
#include <shellapi.h>
#pragma comment(lib, "shell32")
#endif
#include <QMessageBox>





QtWidgetsClass::QtWidgetsClass(QWidget* parent)
	: QWidget(parent)
{
	setupUi(this);
	sc = new ScreenRecorder();
	QTimer* timer = new QTimer(this);
#if linux
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        char *path;
        if (count != -1) {
            path = dirname(result);
        }
        std::string pathstr=std::string(path);
        pathstr.append("//output.mp4");
        sc->RecordingPath = pathstr;
#endif
	this->pathText->setText(QString::fromStdString(sc->RecordingPath));


	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(createErrorMessage()));

	timer->start(1000);
}

QtWidgetsClass::~QtWidgetsClass()
{
	
}


void QtWidgetsClass::on_RECButton_clicked() {
	recButton->setEnabled(false);
	pauseResumeButton->setEnabled(true);
	stopButton->setEnabled(true);
	sc->recordAudio = checkBox->isChecked();
	checkBox->setEnabled(false);
	sizeButton->setEnabled(false);
	wholeScreenButton->setEnabled(false);
	openPath->setEnabled(false);
	pathButton->setEnabled(false);
	sc->SetUpScreenRecorder();
	//std::thread t1(&ScreenRecorder::SetUpScreenRecorder, sc);
	//t1.detach();
	this->showMinimized();
}

void QtWidgetsClass::on_STOPButton_clicked() {
	sc->StopRecording();
	if (sc->recordAudio) {
		sc->audioThread.join();
		QMessageBox messageBox;
		messageBox.information(this, "", "");
		messageBox.showMinimized();
		messageBox.close();
	}
	sc->videoThread.join();
	QMessageBox messageBox;
	messageBox.information(this, "", "");
	messageBox.showMinimized();
	messageBox.close();
	recButton->setEnabled(true);
	pauseResumeButton->setEnabled(false);
	stopButton->setEnabled(false);
	checkBox->setEnabled(true);
	sizeButton->setEnabled(true);
	wholeScreenButton->setEnabled(true);
	openPath->setEnabled(true);
	pathButton->setEnabled(true);
	delete sc;
	sc = new ScreenRecorder(pathText->text().toStdString());
	//sc->RecordingPath = pathText->text().toStdString();
}

void QtWidgetsClass::on_PAUSEButton_clicked() {
	sc->PauseRecording();
	if (!sc->pauseCapture) {
		this->showMinimized();
	}
	recButton->setEnabled(false);
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
#if WIN32
	ShellExecute(NULL, L"open", string_to_wstring(directory).c_str(), NULL, NULL, SW_NORMAL);
#elif linux
    QDesktopServices::openUrl(QUrl(QString::fromStdString(directory)));
#endif
}

std::wstring QtWidgetsClass::string_to_wstring(const std::string& text) {
	return std::wstring(text.begin(), text.end());
}

void QtWidgetsClass::createErrorMessage() {

	if (!sc->error_msg.empty()) {
		this->showNormal();
		QMessageBox messageBox;
		messageBox.critical(0, "Error", QString::fromStdString(sc->error_msg));
		exit(EXIT_FAILURE);
	}
}



