/****************************************************************************
 ** object : �����˵� -- TABLE2 ������һ��
 ** ��ҵ���
 ** by luchaodong 
 ****************************************************************************/
#ifndef DIALOG_ADD_TABLE2_20150411_DEF
#define DIALOG_ADD_TABLE2_20150411_DEF

#include "ui_dialogAddTable2.h"
//#include "TQInputMethod.h"
//#include <QWSInputMethod>

class DialogAddTable2 : public QDialog
{
	Q_OBJECT
public :
	DialogAddTable2(QWidget *parent = 0);
	~DialogAddTable2();
	QString getItemText(int num) const;
	void setItemText(const QString &string, int item);
	
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
	Ui::DialogAddTable2 *ui;
//	QWSInputMethod *im;

};

#endif 

