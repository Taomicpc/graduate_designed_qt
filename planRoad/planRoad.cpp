/****************************************************************************
 ** object : PlanRoad 
 ** ��ҵ��������˵���ͼ��ʶ��
 ** by luchaodong
 ** class Ui::PlanRoad : public Ui_PlanRoad {}
 ** PlanRoad 
 ****************************************************************************/

#include <QtGui>
#include <QPixmap>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget> 
#include <QPalette>
#include <iostream>
#include "planRoad.h"

using namespace std;

PlanRoad::PlanRoad(QWidget *parent):
	QWidget(parent),
	ui(new Ui::PlanRoad) //Ui namespace ,not this 
{
	ui->setupUi(this);
	
	//���ñ���ͼ
	QRect screen_size = QApplication::desktop()->screenGeometry(); //get window size
	QPixmap pix("/opt/gb_ms/picture/background_1.jpg", 0, Qt::AutoColor);
	pix = pix.scaled(screen_size.width(), screen_size.height(), Qt::IgnoreAspectRatio); //photo size
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pix));
	setPalette(palette);

//��ť����
	ui->buttonQuit->setText("");
	ui->buttonQuit->setFixedSize(81,32);
	ui->buttonQuit->setIconSize(QSize(81,32));
	pix.load("/opt/gb_ms/picture/return.png", 0, Qt::AutoColor);
	pix = pix.scaled(81, 32, Qt::IgnoreAspectRatio);
	ui->buttonQuit->setIcon(QIcon(pix));
//signal and slots


}

PlanRoad::~PlanRoad()
{
	delete ui;
}



