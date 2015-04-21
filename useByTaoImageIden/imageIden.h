/****************************************************************************
** object : ImageIden 
 ** ��ҵ��������˵���ͼ��ʶ�� - ht
 ** by luchaodong
 ** class Ui::ImageIden : public Ui_ImageIden {}
 ** ImageIden 
 ****************************************************************************/
#ifndef IMAGEIDE_20150420_DEF
#define IMAGEIDE_20150420_DEF

#include "ui_imageIden.h"
#include "TQInputMethod.h"
#include <QWSInputMethod>
extern "C"
{
    #include "public.h"
}

class ImageIden : public QMainWindow
{
	Q_OBJECT
public :
	explicit ImageIden(QWidget *parent = 0);
	~ImageIden();
	
public slots :
	void loadPicture();
	void buttonQuit();
	
	void btUpPushed();
	void btDownPushed();
	void btLeftPushed();
	void btRightPushed();
	void btSavePushed();
	
	void horizontalAngleSet(); //д��Ƕ�
	void verticalAngleSet();	
	
	void setRefrashImage(bool checked);  

	void doWhenTimeout1(); //��ʱˢ��ͼƬ
	void doWhenTimeout2(); //ͼƬ������ʱ

signals :
	void returned();
private:
	Ui::ImageIden *ui;
	QWSInputMethod *im;	
	QImage *m_getImg;  //load picture from file
	QTimer *timer1;
	QTimer *timer2;
	int m_timeoutCount;

};

#endif 

