/****************************************************************************
 ** object : �����˵� -- TABLE1 ������һ��
 ** ��ҵ���
 ** by luchaodong 
 ****************************************************************************/
#ifndef DIALOG_ADD_TABLE1_20150409_DEF
#define DIALOG_ADD_TABLE1_20150409_DEF

#include "ui_dialogAddTable1.h"
//#include "TQInputMethod.h"
//#include <QWSInputMethod>

class DialogAddTable1 : public QDialog
{
	Q_OBJECT
public :
	DialogAddTable1(QWidget *parent = 0);
	~DialogAddTable1();
	QString getItemText(int num) const;
	
public slots :
	void btQuitPushed();
	void btAddConfirmPushed();
	void btRecordFinishedPushed();
	void btRecordStartPushed();
	void leChanged(const QString &text);
signals :
	void cancel();         //ȡ������
	void operateConfirm(); //ȷ��д��
	void startRecord();    //��ʼ¼��
	void finishRecord();   //����¼��
	
private:
	Ui::DialogAddTable1 *ui;
//	QWSInputMethod *im;

};

#endif 

