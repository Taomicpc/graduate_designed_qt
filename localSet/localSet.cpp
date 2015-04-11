/****************************************************************************
 ** object : LocalSet 
 ** ��ҵ��������˵���ͼ��ʶ��
 ** by luchaodong
 ** class Ui::LocalSet : public Ui_LocalSet {}
 ** LocalSet 
 ****************************************************************************/

#include <QtGui>
#include <QPixmap>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget> 
#include <QPalette>
#include <iostream>
#include "localSet.h"


using namespace std;
LocalSet::LocalSet(QWidget *parent):
	QWidget(parent),
	ui(new Ui::LocalSet) //Ui namespace ,not this 
{
	ui->setupUi(this);

	im = new TQInputMethod;
	QWSServer::setCurrentInputMethod(im);
 	((TQInputMethod*)im)->setVisible(false);

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
	

	initRemoteSetStatus();  //��ʼ��Զ������������

	connect(ui->buttonQuit, SIGNAL(clicked()),
		this, SLOT(buttonQuitPushed()));

	connect(ui->checkBoxRemoteSet, SIGNAL(stateChanged(int)),
		this, SLOT(RemoteSetCheckBoxStatusChanged(int)));
	

}

LocalSet::~LocalSet()
{
	delete ui;
	delete im;
}

void LocalSet::buttonQuitPushed()
{
	emit returned();
	close();
}

void LocalSet::RemoteSetCheckBoxStatusChanged(int state)
{
	if (state == Qt::Checked)
	{
		cout << "checked" << endl;
	}
	else
	{
		cout << "unchecked" << endl;
	}
}

void LocalSet::initRemoteSetStatus()
{

//��ȡ�ļ�����
	ui->checkBoxRemoteSet->setCheckState(Qt::Checked);

}

