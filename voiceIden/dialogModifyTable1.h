/****************************************************************************
 ** object : �����˵� -- TABLE1 �޸���Ŀ
 ** ��ҵ���
 ** by luchaodong 
 ****************************************************************************/
#ifndef DIALOG_MODIFY_TABLE1_20150412_DEF
#define DIALOG_MODIFY_TABLE1_20150412_DEF

#include "ui_dialogModifyTable1.h"

class DialogModifyTable1 : public QDialog
{
	Q_OBJECT
public :
	DialogModifyTable1(QWidget *parent = 0);
	~DialogModifyTable1();
	QString getItemText(int num) const;
	void setItemText(const QString &string, int item);
	bool isVoiceRecordChecked();

public slots :
	void btQuitPushed();
	void btModifyConfirmPushed();
	void btRecordFinishedPushed();
	void btRecordStartPushed();
	void leChanged(const QString &text);
	void voiceRecordCheckChanged(int);

signals :
	void cancel();         //ȡ������
	void operateConfirm(); //ȷ��д��
	void startRecord();    //��ʼ¼��
	void finishRecord();   //����¼��
	
private:
	Ui::DialogModifyTable1 *ui;

};

#endif 

