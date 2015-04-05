/****************************************************************************
 ** object : RemoteSet 
 ** ��ҵ��������˵���Զ������
 ** by luchaodong
 ** class Ui::RemoteSet : public Ui_RemoteSet {}
 ** RemoteSet 
 ****************************************************************************/

#include <QtGui>
#include <QPixmap>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget> 
#include <QPalette>
#include <iostream>
#include "remoteSet.h"


using namespace std;
RemoteSet::RemoteSet(QWidget *parent):
	QWidget(parent),
	ui(new Ui::RemoteSet) //Ui namespace ,not this 
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
	

	connect(ui->buttonQuit, SIGNAL(clicked()),
		this, SLOT(buttonQuitPushed()));
}

RemoteSet::~RemoteSet()
{
	delete ui;
}

void RemoteSet::buttonQuitPushed()
{
	emit returned();
	close();
}
