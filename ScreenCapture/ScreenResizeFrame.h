#pragma once

#include <QWidget>
#include <QtWidgets/qframe.h>
#include "ui_ScreenResizeFrame.h"

class ScreenResizeFrame : public QWidget
{
	Q_OBJECT

public:
	ScreenResizeFrame(QWidget *parent = Q_NULLPTR);
	~ScreenResizeFrame();

	void resizeEvent(QResizeEvent* event);

private:
	Ui::ScreenResizeFrame ui;
	QFrame* vFrameL;
	QFrame* vFrameR;
	QFrame* hFrameU;
	QFrame* hFrameD;
};
