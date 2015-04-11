/****************************************************************************
 ** object : LocalSet 
 ** ��ҵ��������˵���ͼ��ʶ��
 ** by luchaodong
 ** class Ui::LocalSet : public Ui_LocalSet {}
 ** LocalSet 
 ****************************************************************************/
#ifndef LOCALSET_20150403_DEF
#define LOCALSET_20150403_DEF

#include "ui_localSet.h"
#include <QWSInputMethod> 
#include "TQInputMethod.h" 

class LocalSet : public QWidget
{
	Q_OBJECT
public :
	explicit LocalSet(QWidget *parent = 0);
	~LocalSet();
	
	

public slots :
	void buttonQuitPushed();
	void RemoteSetCheckBoxStatusChanged(int state); //Զ�����ÿ�������

signals :
	void returned();
	
	
private:
	Ui::LocalSet *ui;
	QWSInputMethod *im;
	void initRemoteSetStatus(); //��ȡ����Զ�̿��س�ʼ״̬

};


#endif 

