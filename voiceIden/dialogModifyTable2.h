/****************************************************************************
 ** object : �����˵� -- TABLE2 �޸���Ŀ
 ** ��ҵ���
 ** by luchaodong 
 ****************************************************************************/
#ifndef DIALOG_MODIFY_TABLE2_20150411_DEF
#define DIALOG_MODIFY_TABLE2_20150411_DEF

#include "ui_dialogModifyTable2.h"

class DialogModifyTable2 : public QDialog
{
	Q_OBJECT
public :
	DialogModifyTable2(QWidget *parent = 0);
	~DialogModifyTable2();
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
	Ui::DialogModifyTable2 *ui;

};

#endif 

