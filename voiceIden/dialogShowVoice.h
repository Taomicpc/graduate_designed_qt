/****************************************************************************
 ** object : �����˵� -- ����ʶ��
 ** ��ҵ���
 ** by luchaodong 
 ****************************************************************************/
#ifndef DIALOG_SHOWVOICE_20150414_DEF
#define DIALOG_SHOWVOICE_20150414_DEF

#include "ui_dialogShowVoice.h"

class DialogShowVoice : public QDialog
{
	Q_OBJECT
public :
	DialogShowVoice(QWidget *parent = 0);
	~DialogShowVoice();

	void setText(QString &string);

public slots :
	void btRecordFinishedPushed();
	void btRecordStartPushed();
signals :
	void startRecord();    //��ʼ¼��
	void finishRecord();   //����¼��
	
private:
	Ui::DialogShowVoice *ui;

};

#endif 

