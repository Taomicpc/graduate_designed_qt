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

class LocalSet : public QMainWindow
{
	Q_OBJECT
public :
	explicit LocalSet(QWidget *parent = 0);
	~LocalSet();
	
	

public slots :
	


private:
	Ui::LocalSet *ui;


};


#endif 

