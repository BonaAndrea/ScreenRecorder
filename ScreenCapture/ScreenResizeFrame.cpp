#include "ScreenResizeFrame.h"
#include <QtWidgets/qframe.h>
#include <QVBoxLayout>


ScreenResizeFrame::ScreenResizeFrame(QWidget *parent)
	: QWidget(parent)
{
	//setAttribute(Qt::WA_TranslucentBackground);
	//setWindowFlags(Qt::FramelessWindowHint);
	ui.setupUi(this);
	
}

ScreenResizeFrame::~ScreenResizeFrame()
{
}

void ScreenResizeFrame::resizeEvent(QResizeEvent* event)
{
	QRect oldpos = ui.setSizeButton->geometry();
    ui.setSizeButton->move(((size().width()/2)-(ui.setSizeButton->size().width()/2)), ((size().height()/2) - (ui.setSizeButton->size().height() / 2)));
	
    QWidget::resizeEvent(event);
}
