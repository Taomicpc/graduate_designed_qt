/****************************************************************************
 ** object : Avoid 
 ** ��ҵ��������˵���ͼ��ʶ��
 ** by luchaodong
 ** class Ui::Avoid : public Ui_Avoid {}
 ** Avoid 
 ****************************************************************************/
#ifndef AVOID_20150403_DEF
#define AVOID_20150403_DEF

#include "ui_avoid.h"

class Avoid : public QTabWidget
{
	Q_OBJECT
public :
	explicit Avoid(QWidget *parent = 0);
	~Avoid();
	
	

public slots :
	void quitAvoid(); //�˳�
	
	void ctrlUp();
	void ctrlDown();
	void ctrlLeft();
	void ctrlRight();
	void ctrlStop();
	void ctrlSpin();

	void ctrlTrack();
	void ctrlVoiceTrack();
	void ctrlInit();
	
	void setSpeed();
	void setDirectionAngle();
	void setSafeDistance();

	void loadProgram();
	void downloadProgram();
	void programEreasure();
private:
	Ui::Avoid *ui;
	QString m_loadFileName;
};


#endif 

