#pragma once

#include <QWidget>
#include "ui_QtWidgetsClass.h"
#include "ScreenRecorder.h"
#include "ScreenResizeFrame.h"

class QtWidgetsClass : public QWidget, public Ui::QtWidgetsClass
{
	Q_OBJECT

public slots:
	void on_RECButton_clicked();
	void on_STOPButton_clicked();
	void on_PAUSEButton_clicked();
	void on_RESIZEButton_clicked();

public:
	QtWidgetsClass(QWidget *parent = Q_NULLPTR);
	~QtWidgetsClass();
	ScreenRecorder sc;

private:
	ScreenResizeFrame* scResFr;
};
