#pragma once

#include <QWidget>
#include "ui_ScreenResizeFrame.h"

class ScreenResizeFrame : public QWidget
{
	Q_OBJECT

public:
	ScreenResizeFrame(QWidget *parent = Q_NULLPTR);
	~ScreenResizeFrame();

private:
	Ui::ScreenResizeFrame ui;
};
