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

#define CHANGE_STEP 50//�����ı����ǶȵĲ���
shmType* shmPtr = NULL;//���ù����ڴ��ȫ�ֱ���ָ��

//��ʼ��ȫ�ֱ���shmPtr�����������̵��ڴ����ӣ������г�ʼ�����á�
//���0Ϊ�ɹ���ʼ����1��2��3�ֱ��Ӧ��ȡipc�Ŵ���,�����ڴ�����ʧ�ܣ������ڴ�ӳ��ʧ��
int shm_init(void)
{
//--------------�����ڴ潨������--------------------
    key_t ipcKey;
    int IshmId;

    ipcKey = ftok("./shm", 'a');
    if(ipcKey == -1)
    {
        perror("ftok error");
        return 1;//����ʧ��
    }

    IshmId = shmget(ipcKey, sizeof(shmType),S_IRUSR | S_IWUSR | IPC_CREAT | 0777);//0666��ubuntu�´˲�����Ҫ��У��

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

//�����ڴ�����ʼ��
    memset(shmPtr, 0, sizeof(shmType));
    //ǰһ��1�����ڽ��̼�ʹ�ã���һ��1����һ���ź�����ʼֵ��1���źſ��У������ź�æµ
    sem_init(&shmPtr->shmSem, 1, 1);

//���ո������������е��ź�������������źţ����̽�����˯��״̬��
    //���ź���æµ�������̼�ʹ������Ҳ�����˯��״̬��
    sem_init(&shmPtr->input.sem_input_wakeup, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_wakeup, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_wakeup, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_wakeup, 1, 0);
    sem_init(&shmPtr->tower.sem_tower_wakeup, 1, 0);

    //�������ź������Ӧ�Ĳ�������ʼ��
    //���ñ�־Ϊfalse��ʹ�����̽�������wakeup�ź�����˯��״̬
    shmPtr->input.b_input_running = true;//����imageͼ��Ĭ��һֱ���� 
    sem_post(&shmPtr->input.sem_input_wakeup);//��ʼvideo����
    
    shmPtr->wtofile.b_wtofile_running = false; 
    shmPtr->wtolcd.b_wtolcd_running = false; 
    shmPtr->deal.b_deal_running = false; 
    shmPtr->tower.b_tower_running = false; 

//�ź�����ʼ��Ϊæµ������image.jpg��д���ź�ͬ����
    sem_init(&shmPtr->input.sem_wr_enable, 1, 0);
    sem_init(&shmPtr->wtofile.sem_wtofile_standby, 1, 0);
    sem_init(&shmPtr->wtolcd.sem_wtolcd_standby, 1, 0);
    sem_init(&shmPtr->deal.sem_deal_standby, 1, 0);
    //��֮���Ӧ��������ʼ��
    shmPtr->wtofile.b_finish_wtofile = true;//�������˳����trueΪ�˳�
    shmPtr->wtolcd.b_finish_wtolcd = true;
    shmPtr->deal.b_finish_deal = true;

    return 0;//�ɹ����й����ڴ洴������ʼ��
}

//�˵��û�ɾ�������ڴ棬��Ҫ�������ڴ�ӳ�䶼���(���߱�֤����ʹ�ã��������)
int shm_destroy(void)
{
    sem_destroy(&shmPtr->shmSem);//���������ź���
    shmctl(IshmId, IPC_RMID, 0);//ɾ�������ڴ�ӳ����
}

//ֱ�Ӵ������ֽ��̣������̴������������״̬��
int process_create(void)
{
//-------------��������-------------------
    char SshmId[20];//���ڽ����ִ��͵�shmId

    sprintf(SshmId, "%d", IshmId);

    char* execvInput[] = {"./input", SshmId, NULL};
    char* execvWtoFile[] = {"./wtofile", SshmId, argv[1], argv[2], NULL};
    char* execvWtoLCD[] = {"./wtolcd", SshmId, NULL};
    char* execvDeal[] = {"./deal", SshmId, NULL};
    char* execvTower[] = {"./tower", SshmId, NULL};

    if(fork()==0)//д���ӽ���,��ʼ�������˯��״̬
    {
        execv("./tower", execvTower, NULL);//����������Դд�����
        perror("execv tower error");
    }
    
    if(fork()==0)//д���ӽ���,��ʼ�������˯��״̬
    {
        execv("./input", execvInput, NULL);//����������Դд�����
        perror("execv input error");
    }

    if(fork()==0)//������������ʾ��lcd
    {
        execv("./wtofile", execvWtoFile, NULL);//����������Դд�����
        perror("execv wtofile error");
    }

    if(fork()==0)//������������ʾ��lcd
    {
        execv("./wtolcd", execvWtoLCD, NULL);//����������Դд�����
        perror("execv wtolcd error");
    }

    if(fork()==0)//ͼ�������
    {
        execv("./deal", execvDeal, NULL);//����������Դд�����
        perror("execv deal error");
    }

    return 0;
}

//�޸Ľ������̵ı�־λ���������ź����ȴ������̽�����
int process_destroy( void )
{
    sem_wait(&shmPtr->shmSem);//�޸Ĺ����ڴ�����ǰ����ź���
    shmPtr->b_endflag = true;//�ù����ڴ洦���˳���־ΪΪ�棬�����̶�ȡ���˳���ǰ���̡�

    //���������ź������������߽���
    sem_post(&shmPtr->input.sem_input_wakeup);//��ʼvideo����
    sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//��ʼvideo����
    sem_post(&shmPtr->wtolcd.sem_wtolcd_wakeup);//��ʼvideo����
    sem_post(&shmPtr->deal.sem_deal_wakeup);//��ʼvideo����
    sem_post(&shmPtr->tower.sem_tower_wakeup);//��ʼvideo����
    //���������ź������������߽���
    sem_post(&shmPtr->wtofile.sem_wtofile_standby);//��ʼvideo����
    sem_post(&shmPtr->wtolcd.sem_wtolcd_standby);//��ʼvideo����
    sem_post(&shmPtr->deal.sem_deal_standby);//��ʼvideo����

    sem_post(&shmPtr->shmSem);//�ͷ��ź���

    int status;

    while(wait(&status) != -1);//һֱ�ȴ���������ӽ��̶�����,ÿһ�εȴ���������

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

	
    shm_init();//��ʼ��ȫ�ֹ����ڴ�ָ�롣
    process_create();//��������Ҫ�Ľ��̣���������һ�������ڴ��ʼ������У�
}

ImageIden::~ImageIden()
{
    process_destroy();//�˳�ǰ����������
    shm_destroy();//�����˳���ɾ�������ڴ��ϵ
    
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

    sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
    
    if((shmPtr->tower.veri_angle + CHANGE_STEP) < 3000)
    {
        shmPtr->tower.veri_angle += CHANGE_STEP;//����Ϊ
    }    
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�

    //���ѽ���
    shmPtr->tower.b_tower_running = true;
    sem_post(&shmPtr->tower.sem_tower_wakeup);
    
    sem_post(&shmPtr->shmSem);
}


void ImageIden::btDownPushed()
{
	cout << "down" << endl;

    sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
    
    if(shmPtr->tower.veri_angle > CHANGE_STEP)
    {
        shmPtr->tower.veri_angle -= CHANGE_STEP;//����Ϊ
    }    
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�

    //���ѽ���
    shmPtr->tower.b_tower_running = true;
    sem_post(&shmPtr->tower.sem_tower_wakeup);
    
    sem_post(&shmPtr->shmSem);
}


void ImageIden::btLeftPushed()
{
	cout << "left" << endl;

    sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
    
    if(shmPtr->tower.hori_angle > CHANGE_STEP)
    {
        shmPtr->tower.hori_angle -= CHANGE_STEP;//����Ϊ
    }    
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�

    //���ѽ���
    shmPtr->tower.b_tower_running = true;
    sem_post(&shmPtr->tower.sem_tower_wakeup);
    
    sem_post(&shmPtr->shmSem);
}

void ImageIden::btRightPushed()
{
  	cout << "right" << endl;

    sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
    
    if((shmPtr->tower.hori_angle + CHANGE_STEP) < 3000)
    {
        shmPtr->tower.hori_angle += CHANGE_STEP;//����Ϊ
    }    
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�

    //���ѽ���
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
        sem_post(&shmPtr->wtofile.sem_wtofile_wakeup);//��ʼvideo����
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
        sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
        shmPtr->tower.hori_angle = ui->leHorizontal->text().toInt();//����Ϊ
    
        //���ѽ���
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);
    }
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�
}

void ImageIden::verticalAngleSet()
{
	//qDebug() << ui->leVertical->text();

    if( ui->leVertical->text().toInt() > 0 && ui->leVertical->text().toInt() < 3000)
    {
        sem_wait(&shmPtr->shmSem);//��Ҫ�޸Ĺ����ڴ�Ƕ���Ϣ����ȡ�ź���
        shmPtr->tower.veri_angle = ui->leVertical->text().toInt();//����Ϊ
    
        //���ѽ���
        shmPtr->tower.b_tower_running = true;
        sem_post(&shmPtr->tower.sem_tower_wakeup);
    
        sem_post(&shmPtr->shmSem);
    }
    else
        return;//ֱ�ӷ��ز����Ƕ��޸�
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
