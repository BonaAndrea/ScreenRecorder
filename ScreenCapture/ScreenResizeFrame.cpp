#include "ScreenResizeFrame.h"
#include <QHBoxLayout>
#include <QLabel>


ScreenResizeFrame::ScreenResizeFrame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	QHBoxLayout* box = new QHBoxLayout(this);
	QLabel* pLabel = new QLabel("Center panel", this);
	box->addWidget(pLabel);
	this->setStyleSheet("background-color: rgb(85, 100, 100)");
}

ScreenResizeFrame::~ScreenResizeFrame()
{
}
