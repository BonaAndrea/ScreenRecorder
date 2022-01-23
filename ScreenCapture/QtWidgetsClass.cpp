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
	ScreenRecorder::SetUpScreenRecorder();
}

