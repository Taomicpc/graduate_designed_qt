/****************************************************************************
 ** object : ImageIden 
 ** ��ҵ��������˵���ͼ��ʶ��
 ** by luchaodong
 ** class Ui::ImageIden : public Ui_ImageIden {}
 ** ImageIden 
 ****************************************************************************/
#ifndef IMAGEIDE_20150402_DEF
#define IMAGEIDE_20150402_DEF

#include "ui_imageIden.h"

class ImageIden : public QMainWindow
{
	Q_OBJECT
public :
	explicit ImageIden(QWidget *parent = 0);
	~ImageIden();
	
public slots :
	void loadPicture();
	void buttonQuit();
	void setModelFace();
	void setModelDigit();

	
	void startIden();
	void startCatch();
	void startTrain();
	void deleteFaceID();
signals :
	void returned();
private:
	Ui::ImageIden *ui;
	QImage *m_getImg;  //load picture from file
	QString m_inputId; //��������ID
	int m_digit;	   //ʶ������
	
	enum IdentModelType
	{
		FACEMODEL  = 1,
		DIGITMODEL = 2
	};

	int m_currentModel;

};

#endif 

