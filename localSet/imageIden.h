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
	


private:
	Ui::ConfigureMainWindow *ui;


};


#endif 

