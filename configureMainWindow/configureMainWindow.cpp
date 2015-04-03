/****************************************************************************
 ** object : ConfigureMainWindow 
 ** ��ҵ��ƶ����˵������ý���
 ** by luchaodong
 ** class Ui::ConfigureMainWindow: public Ui_ConfigureMainWindow {}
 ** ConfigureMainWindow 
 ****************************************************************************/

#include <QtGui>
#include <QPixmap>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget> 
#include <QPalette>
#include <iostream>
#include "configureMainWindow.h"
using namespace std;
ConfigureMainWindow::ConfigureMainWindow(QWidget *parent):
	QMainWindow(parent),
	ui(new Ui::ConfigureMainWindow) //Ui namespace ,not this 
{
	ui->setupUi(this);
	
	//���ñ���ͼ
	QRect screen_size = QApplication::desktop()->screenGeometry(); //get window size
	QPixmap pix("../picture/background.jpg", 0, Qt::AutoColor);
	pix = pix.scaled(screen_size.width(), screen_size.height(), Qt::IgnoreAspectRatio); //photo size
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pix));
	setPalette(palette);

//��ť����
	ui->buttonQuit->setText("");
	ui->buttonQuit->setFixedSize(100,100);
	ui->buttonQuit->setIconSize(QSize(100,100));
	pix.load("../picture/quit.png", 0, Qt::AutoColor);
	pix = pix.scaled(100, 100, Qt::IgnoreAspectRatio);
	ui->buttonQuit->setIcon(QIcon(pix));
	
//���ð�ťʧ��
	ui->buttonBA1->setEnabled(false);
	ui->buttonBA2->setEnabled(false);
	ui->buttonBA3->setEnabled(false);

/*
//set button font color
	palette.setColor(QPalette::ButtonText, Qt::white);
	ui->buttonVoice->setPalette(palette);
	ui->buttonVoice->setFlat(true);

	palette.setColor(QPalette::ButtonText, Qt::white);
	ui->buttonImage->setPalette(palette);
	ui->buttonImage->setFlat(true);
	
	palette.setColor(QPalette::ButtonText, Qt::white);
	ui->buttonAvoid->setPalette(palette);
	ui->buttonAvoid->setFlat(true);
	
	palette.setColor(QPalette::ButtonText, Qt::white);
	ui->buttonPlan->setPalette(palette);
	ui->buttonPlan->setFlat(true);

	palette.setColor(QPalette::ButtonText, Qt::black);
	ui->buttonRemote->setPalette(palette);
	ui->buttonRemote->setFlat(true);
	
	palette.setColor(QPalette::ButtonText, Qt::black);
	ui->buttonBA1->setPalette(palette);
	ui->buttonBA1->setFlat(true);

	palette.setColor(QPalette::ButtonText, Qt::black);
	ui->buttonBA2->setPalette(palette);
	ui->buttonBA2->setFlat(true);

	palette.setColor(QPalette::ButtonText, Qt::black);
	ui->buttonFileManage->setPalette(palette);
	ui->buttonFileManage->setFlat(true);

	palette.setColor(QPalette::ButtonText, Qt::black);
	ui->buttonLocal->setPalette(palette);
	ui->buttonLocal->setFlat(true);

	palette.setColor(QPalette::ButtonText, Qt::black);
	ui->buttonQuit->setPalette(palette);
	ui->buttonQuit->setFlat(true);

	palette.setColor(QPalette::ButtonText, Qt::black);
	ui->buttonBA3->setPalette(palette);
	ui->buttonBA3->setFlat(true);
*/
	palette.setColor(QPalette::WindowText, Qt::white);
	ui->label->setPalette(palette);

//signal and slots
	connect(ui->buttonVoice, SIGNAL(clicked()),
		this, SLOT(buttonVoicePressed()));
	connect(ui->buttonImage, SIGNAL(clicked()),
		this, SLOT(buttonImagePressed()));
	connect(ui->buttonAvoid, SIGNAL(clicked()),
		this, SLOT(buttonAvoidPressed()));
	connect(ui->buttonPlan, SIGNAL(clicked()),
		this, SLOT(buttonPlanPressed()));
	connect(ui->buttonRemote, SIGNAL(clicked()),
		this, SLOT(buttonRemotePressed()));
	connect(ui->buttonBA1, SIGNAL(clicked()),
		this, SLOT(buttonBA1Pressed()));
	connect(ui->buttonBA2, SIGNAL(clicked()),
		this, SLOT(buttonBA2Pressed()));
	connect(ui->buttonFileManage, SIGNAL(clicked()),
		this, SLOT(buttonFileManagePressed()));
	connect(ui->buttonLocal, SIGNAL(clicked()),
		this, SLOT(buttonLocalPressed()));
	connect(ui->buttonLocal, SIGNAL(clicked()),
		this, SLOT(buttonLocalPressed()));
	connect(ui->buttonQuit, SIGNAL(clicked()),
		this, SLOT(buttonQuitPressed()));
	connect(ui->buttonBA3, SIGNAL(clicked()),
		this, SLOT(buttonBA3Pressed()));

	connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)),
		this, SLOT(processFinished(int, QProcess::ExitStatus)));


}

ConfigureMainWindow::~ConfigureMainWindow()
{
	delete ui;
}

void ConfigureMainWindow::buttonVoicePressed()
{
	cout << "voiceIden" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./voiceIden", argList);
}

void ConfigureMainWindow::buttonImagePressed()
{
	cout << "imageIden" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./imageIden", argList);

}

void ConfigureMainWindow::buttonAvoidPressed()
{
	cout << "avoid" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./avoid", argList);

}

void ConfigureMainWindow::buttonPlanPressed()
{
	cout << "planRoad" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./planRoad", argList);

}

void ConfigureMainWindow::buttonRemotePressed()
{
	cout << "remoteSet" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./remoteSet", argList);

}

void ConfigureMainWindow::buttonBA1Pressed()
{
	cout << "ba1" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./ba1", argList);
}

void ConfigureMainWindow::buttonBA2Pressed()
{
	cout << "ba2" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./ba2", argList);

}

void ConfigureMainWindow::buttonFileManagePressed()
{
	cout << "fileManage" << endl;
	QStringList argList;
	processLauncher("./fileManage", argList);
}

void ConfigureMainWindow::buttonLocalPressed()
{
	cout << "localSet" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./localSet", argList);

}

void ConfigureMainWindow::buttonQuitPressed()
{
	close();
}

void ConfigureMainWindow::buttonBA3Pressed()
{
	cout << "ba3" << endl;
	QStringList argList;
	//argList << "-al";
	processLauncher("./ba3", argList);

}


void ConfigureMainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode);
	Q_UNUSED(exitStatus);
	show();
}

void ConfigureMainWindow::processLauncher(const QString &program, const QStringList &arguments)
{
	
	process.start(program, arguments);
	
	if ( process.waitForStarted(10000) != true )
	{
		QMessageBox::warning(this, tr("Process Error"), tr("Can\'t start the process"), QMessageBox::Yes);
		show();
		return;
	}
	
	hide();
//	process.waitForFinished(-1);  //block until the process is finished, never timeout
}

