#pragma once

#include <QWidget>
#include "ui_QtWidgetsClass.h"

class QtWidgetsClass : public QWidget, public Ui::QtWidgetsClass
{
	Q_OBJECT

public:
	QtWidgetsClass(QWidget *parent = Q_NULLPTR);
	~QtWidgetsClass();
};
