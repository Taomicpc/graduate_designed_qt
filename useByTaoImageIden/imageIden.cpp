/****************************************************************************
 ** object : ImageIden 
 ** ��ҵ��������˵���ͼ��ʶ�� - ht
 ** by luchaodong
 ** class Ui::ImageIden : public Ui_ImageIden {}
 ** ImageIden 
 ****************************************************************************/

#include <QtGui>
#include <QPixmap>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget> 
#include <QPalette>
#include <QTimer>
#include <iostream>
#include "imageIden.h"

using namespace std;

ImageIden::ImageIden(QWidget *parent):
	QMainWindow(parent),
	ui(new Ui::ImageIden), //Ui namespace ,not this 
	im(new TQInputMethod),
	m_getImg(new QImage), timer1(new QTimer), timer2(new QTimer)
{
	ui->setupUi(this);
	
	//input
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



//signal and slots
	connect(ui->actionFromFile, SIGNAL(triggered()), this, SLOT(loadPicture()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(buttonQuit()));
	connect(ui->buttonQuit, SIGNAL(clicked()), this, SLOT(buttonQuit()));

	connect(ui->btUp, SIGNAL(clicked()), this, SLOT(btUpPushed()));	
	connect(ui->btDown, SIGNAL(clicked()), this, SLOT(btDownPushed()));	
	connect(ui->btLeft, SIGNAL(clicked()), this, SLOT(btLeftPushed()));	
	connect(ui->btRight, SIGNAL(clicked()), this, SLOT(btRightPushed()));	
	connect(ui->btSave, SIGNAL(clicked()), this, SLOT(btSavePushed()));	
		
	connect(ui->leHorizontal, SIGNAL(editingFinished()), this, SLOT(horizontalAngleSet()));	
	connect(ui->leVertical, SIGNAL(editingFinished()), this, SLOT(verticalAngleSet()));	
		
	connect(ui->rbRefrashImg, SIGNAL(toggled(bool)), this, SLOT(setRefrashImage(bool)));	

	
	connect(timer1, SIGNAL(timeout()), this, SLOT(doWhenTimeout1()));
	timer1->setSingleShot(false); //��δ���
	
	connect(timer2, SIGNAL(timeout()), this, SLOT(doWhenTimeout2()));
	timer2->setSingleShot(false); //��δ���
	
	m_getImg->load("/opt/gb_ms/picture/background.jpg");
	*m_getImg = m_getImg->scaled(QSize(250,330), Qt::IgnoreAspectRatio); //photo size
	ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));
	
}

ImageIden::~ImageIden()
{
	delete ui;
	delete im;
	delete m_getImg;
	delete timer1;
	delete timer2;
}

void ImageIden::loadPicture()
{
	QString fileName;

	fileName = QFileDialog::getOpenFileName(this, trUtf8("ѡ��ͼ��"), "",
					tr("Images(*.png *.bmp *.jpg *.tif *.GIF)"));
	if (fileName.isEmpty())
		return;
	else
	{
		if ( !( m_getImg->load(fileName) ) )
		{
			QMessageBox::information(this,
					tr("Open img error"),
					tr("Open img error!"));
			return;
		}
		*m_getImg = m_getImg->scaled(QSize(250,330), Qt::IgnoreAspectRatio); //photo size
		ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));
	}
}

void ImageIden::buttonQuit()
{
	emit returned();
	close();
}

void ImageIden::btUpPushed()
{
	cout << "up" << endl;

}


void ImageIden::btDownPushed()
{
	cout << "down" << endl;

}


void ImageIden::btLeftPushed()
{
	cout << "left" << endl;

}

void ImageIden::btRightPushed()
{
  	cout << "right" << endl;

}


void ImageIden::btSavePushed()
{
	cout << "save" << endl;
	if (ui->leNumber->text().toInt() >0 && ui->leTime->text().toInt() > 0)
	{
		//������ʱ���� ��ʱ���� n��ͼƬ
		int setTime = ui->leTime->text().toInt() * 1000;
		cout << setTime << endl;
		timer2->start(setTime);
		m_timeoutCount = 0;
		

	}	
	else
	{
		cout << "save 1 " << endl;
	
	}

}

void ImageIden::horizontalAngleSet()
{
	qDebug() << ui->leHorizontal->text();

}

void ImageIden::verticalAngleSet()
{
	 qDebug() << ui->leVertical->text();


}


void ImageIden::setRefrashImage(bool checked)
{
	if (checked)
	{
		cout << "checked" << endl;
		if ( !(timer1->isActive()) )
		{
		        timer1->start(2000);	
		}
	}
	else
		if (timer1->isActive())
			timer1->stop();
}


void ImageIden::doWhenTimeout1()
{
	cout << "Refrash image" << endl;	
	//ˢ��ͼƬ

}


void ImageIden::doWhenTimeout2()
{
	++m_timeoutCount;
	cout << "time out 2 " << m_timeoutCount << endl;	
	
	//��ʱ����ͼƬ��ʱ������

	if (m_timeoutCount >= ui->leNumber->text().toInt())
		timer2->stop();


}
