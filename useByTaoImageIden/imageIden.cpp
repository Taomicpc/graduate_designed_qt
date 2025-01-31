/****************************************************************************
 ** object : ImageIden 
 ** 毕业设计三级菜单　图像识别 - ht
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

#define CHANGE_STEP 50//按键改变舵机角度的步长
int IshmId;
shmType* shmPtr = NULL;//设置共享内存的全局变量指针

//初始化全局变量shmPtr，建立本进程的内存链接，并进行初始化设置。
//输出0为成功初始化，1、2、3分别对应获取ipc号错误,共享内存申请失败，共享内存映射失败
int shm_init(void)
{
//--------------共享内存建立链接--------------------
    key_t ipcKey;

    ipcKey = ftok("./shm", 'a');
    if(ipcKey == -1)
    {
        perror("ftok error");
        return 1;//创建失败
    }

    IshmId = shmget(ipcKey, sizeof(shmType),S_IRUSR | S_IWUSR | IPC_CREAT | 0777);//0666是ubuntu下此操作需要的校验

    if(IshmId == -1)
    {
        perror("shmget error");
        return 2;
    }

    shmPtr = (shmType *)shmat(IshmId, NULL, 0);
   
    if(shmPtr == (void *)-1)
    {
        perror("main.c shmat error");
        return 3;
    }

//共享内存区初始化
    memset(shmPtr, 0, sizeof(shmType));
    shmPtr->tower.hori_angle = 1000;
    shmPtr->tower.veri_angle = 1000;//角度信号初始化
    //前一个1表明在进程间使用，后一个1设置一个信号量初始值，1是信号空闲，０是信号忙碌
    sem_init(&shmPtr->shmSem, 1, 1);

//回收各进程启动运行的信号量，如果不给信号，进程将进入睡眠状态．
    //设信号量忙碌，各进程即使启动，也会进入睡眠状态。
    sem_init(&shmPtr->input.sem_input_wakeup, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_wakeup, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_wakeup, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_wakeup, 1, 0);
    sem_init(&shmPtr->tower.sem_tower_wakeup, 1, 0);

    //与上面信号量相对应的布尔量初始化
    //先置标志为false，使各进程进入上面wakeup信号量的睡眠状态
    shmPtr->input.b_input_running = true;//更新image图像默认一直运行 
    sem_post(&shmPtr->input.sem_input_wakeup);//开始video更新
    
    shmPtr->wtofile.b_wtofile_running = false; 
    shmPtr->wtolcd.b_wtolcd_running = false; 
    shmPtr->deal.b_deal_running = false; 
    shmPtr->tower.b_tower_running = false; 

//信号量初始化为忙碌，用于image.jpg读写的信号同步。
    sem_init(&shmPtr->input.sem_wr_enable, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_standby, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_standby, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_standby, 1, 0);
    //与之相对应布尔量初始化
    shmPtr->wtofile.b_finish_wtofile = true;//各进程退出命令，true为退出
    shmPtr->wtolcd.b_finish_wtolcd = true;
    shmPtr->deal.b_finish_deal = true;

    return 0;//成功进行共享内存创建并初始化
}

//此调用会删除共享内存，需要各共享内存映射都解除(或者保证不再使用，否则出错)
int shm_destroy(void)
{
    sem_destroy(&shmPtr->shmSem);//销毁无名信号量
    shmctl(IshmId, IPC_RMID, 0);//删除共享内存映射区
    return 0;
}

//直接创建各种进程，各进程创建后进入休眠状态。
int process_create(void)
{
//-------------创建进程-------------------
    char SshmId[20];//用于接收字串型的shmId

    sprintf(SshmId, "%d", IshmId);

    char* execvInput[] = {"./input", NULL};
    char* execvWtoFile[] = {"./wtofile",NULL};
    char* execvWtoLCD[] = {"./wtolcd",NULL};
    char* execvDeal[] = {"./deal", SshmId, NULL};
    char* execvTower[] = {"./tower", SshmId, NULL};

    if(fork()==0)//写入子进程,初始化后进入睡眠状态
    {
        execv("./tower", execvTower);//启动共享资源写入进程
        perror("execv tower error");
    }
    
    if(fork()==0)//写入子进程,初始化后进入睡眠状态
    {
        execv("./input", execvInput);//启动共享资源写入进程
        perror("execv input error");
    }

    if(fork()==0)//缓冲区数据显示到lcd
    {
        execv("./wtofile", execvWtoFile);//启动共享资源写入进程
        perror("execv wtofile error");
    }

    if(fork()==0)//缓冲区数据显示到lcd
    {
        execv("./wtolcd", execvWtoLCD);//启动共享资源写入进程
        perror("execv wtolcd error");
    }

    if(fork()==0)//图像处理进程
    {
        execv("./deal", execvDeal);//启动共享资源写入进程
        perror("execv deal error");
    }

    return 0;
}

//修改结束进程的标志位，并发出信号量等待各进程结束。
int process_destroy( void )
{
    sem_wait(&shmPtr->shmSem);//修改共享内存内容前获得信号量
    shmPtr->b_endflag = true;//置共享内存处的退出标志为为真，各进程读取后退出当前进程。

    //给各进程信号量，唤醒休眠进程
    sem_post(&shmPtr->input.sem_input_wakeup);//开始video更新
    sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//开始video更新
    sem_post(&shmPtr->wtolcd.sem_wtolcd_wakeup);//开始video更新
    sem_post(&shmPtr->deal.sem_deal_wakeup);//开始video更新
    sem_post(&shmPtr->tower.sem_tower_wakeup);//开始video更新
    //给各进程信号量，唤醒休眠进程
    sem_post(&shmPtr->wtofile.sem_wtofile_standby);//开始video更新
    sem_post(&shmPtr->wtolcd.sem_wtolcd_standby);//开始video更新
    sem_post(&shmPtr->deal.sem_deal_standby);//开始video更新

    sem_post(&shmPtr->shmSem);//释放信号量

    int status;

    while(waitpid(-1, &status, 0) != -1);//一直等待其产生的子进程都结束,每一次等待都是休眠

    printf("all process exit\n");

    return 0;
}


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
	

	//设置背景图
	QRect screen_size = QApplication::desktop()->screenGeometry(); //get window size
	QPixmap pix("/opt/gb_ms/picture/background_1.jpg", 0, Qt::AutoColor);
	pix = pix.scaled(screen_size.width(), screen_size.height(), Qt::IgnoreAspectRatio); //photo size
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pix));
	setPalette(palette);

//按钮背景
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
	timer1->setSingleShot(false); //多次触发
	
	connect(timer2, SIGNAL(timeout()), this, SLOT(doWhenTimeout2()));
	timer2->setSingleShot(false); //多次触发
	
	m_getImg->load("/opt/gb_ms/picture/background.jpg");
	*m_getImg = m_getImg->scaled(QSize(250,330), Qt::IgnoreAspectRatio); //photo size
	ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));

	
    shm_init();//初始化全局共享内存指针。
    process_create();//创建各需要的进程，必须在上一个共享内存初始化后进行！
}

ImageIden::~ImageIden()
{
    process_destroy();//退出前结束各进程
    shm_destroy();//进程退出后删除共享内存关系
    
	delete ui;
	delete im;
	delete m_getImg;
	delete timer1;
	delete timer2;
}

void ImageIden::loadPicture()
{
	QString fileName;

	fileName = QFileDialog::getOpenFileName(this, trUtf8("选择图像"), "",
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

    sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
    
    if((shmPtr->tower.veri_angle + CHANGE_STEP) < 3000)
    {
        shmPtr->tower.veri_angle += CHANGE_STEP;//增量为
    }    
    else
        return;//直接返回不作角度修改

    //唤醒进程
    shmPtr->tower.b_tower_running = true;
    sem_post(&shmPtr->tower.sem_tower_wakeup);
    
    sem_post(&shmPtr->shmSem);
}


void ImageIden::btDownPushed()
{
	cout << "down" << endl;

    sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
    
    if(shmPtr->tower.veri_angle > CHANGE_STEP)
    {
        shmPtr->tower.veri_angle -= CHANGE_STEP;//增量为
    }    
    else
        return;//直接返回不作角度修改

    //唤醒进程
    shmPtr->tower.b_tower_running = true;
    sem_post(&shmPtr->tower.sem_tower_wakeup);
    
    sem_post(&shmPtr->shmSem);
}


void ImageIden::btLeftPushed()
{
	cout << "left" << endl;

    sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
    
    if(shmPtr->tower.hori_angle > CHANGE_STEP)
    {
        shmPtr->tower.hori_angle -= CHANGE_STEP;//增量为
    }    
    else
        return;//直接返回不作角度修改

    //唤醒进程
    shmPtr->tower.b_tower_running = true;
    sem_post(&shmPtr->tower.sem_tower_wakeup);
    
    sem_post(&shmPtr->shmSem);
}

void ImageIden::btRightPushed()
{
  	cout << "right" << endl;

    sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
    
    if((shmPtr->tower.hori_angle + CHANGE_STEP) < 3000)
    {
        shmPtr->tower.hori_angle += CHANGE_STEP;//增量为
    }    
    else
        return;//直接返回不作角度修改

    //唤醒进程
    shmPtr->tower.b_tower_running = true;
    sem_post(&shmPtr->tower.sem_tower_wakeup);
    
    sem_post(&shmPtr->shmSem);
}


void ImageIden::btSavePushed()
{
	cout << "save" << endl;

    if (ui->leNumber->text().toInt() > 0)
	{	
        sem_wait(&shmPtr->shmSem);
        strcpy(shmPtr->wtofile.name, "hello");
        shmPtr->wtofile.count = ui->leNumber->text().toInt();
        shmPtr->wtofile.b_wtofile_running = true;
        sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//开始video更新
        sem_post(&shmPtr->shmSem);
	}	
	else
	{
		cout << "parameter error " << endl;
	}
}

void ImageIden::horizontalAngleSet()
{
//	qDebug() << ui->leHorizontal->text();
    if( ui->leHorizontal->text().toInt() > 0 && ui->leHorizontal->text().toInt() < 3000)
    {
        sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
        shmPtr->tower.hori_angle = ui->leHorizontal->text().toInt();//增量为
    
        //唤醒进程
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);
    }
    else
        return;//直接返回不作角度修改
}

void ImageIden::verticalAngleSet()
{
	//qDebug() << ui->leVertical->text();

    if( ui->leVertical->text().toInt() > 0 && ui->leVertical->text().toInt() < 3000)
    {
        sem_wait(&shmPtr->shmSem);//需要修改共享内存角度信息，获取信号量
        shmPtr->tower.veri_angle = ui->leVertical->text().toInt();//增量为
    
        //唤醒进程
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);
    }
    else
        return;//直接返回不作角度修改
}


void ImageIden::setRefrashImage(bool checked)
{
	if (checked)
	{
		cout << "checked" << endl;
		if ( !(timer1->isActive()) )
		{
		        timer1->start(200);	
		}
	}
	else
		if (timer1->isActive())
			timer1->stop();
}


void ImageIden::doWhenTimeout1()
{
	cout << "Refrash image" << endl;	
	//刷新图片
	
	m_getImg->load("./image.jpg");
	*m_getImg = m_getImg->scaled(QSize(250,330), Qt::IgnoreAspectRatio); //photo size
	ui->labelPicture->setPixmap(QPixmap::fromImage(*m_getImg));

}


void ImageIden::doWhenTimeout2()
{
	++m_timeoutCount;
	cout << "time out 2 " << m_timeoutCount << endl;	
	
	//延时保存图片定时器触发

	if (m_timeoutCount >= ui->leNumber->text().toInt())
		timer2->stop();


}
