/****************************************************************************
 ** object : VoiceIden 
 ** ��ҵ��������˵�������ʶ��
 ** by luchaodong
 ** class Ui::VoiceIden : public Ui_VoiceIden {}
 ** VoiceIden 
 ****************************************************************************/
#ifndef VOICEIDEN_20150403_DEF
#define VOICEIDEN_20150403_DEF

#include "TQInputMethod.h"
#include <QWSInputMethod>
#include "ui_voiceIden.h"
#include "dialogAddTable1.h"
#include "dialogAddTable2.h"
//#include "dialogModifyTable1.h"
#include "dialogModifyTable2.h"

class VoiceIden : public QWidget
{
	Q_OBJECT
	public :
	explicit VoiceIden(QWidget *parent = 0);
	~VoiceIden();
	
public slots :
	void btQuitPushed();  
	void btAddTable1Pushed(); 
	void btDeleteTable1Pushed();
	void btAddTable2Pushed(); 
	void btDeleteTable2Pushed();

	void modifyTable1(int, int);
	void modifyTable2(int, int);
	void updateTable2(int currentRow,int currentColumn,int previousRow,int previousColumn);
	

	//���ݱ��������
	void dialogAddTable1Comfirn();
	void dialogAddTable1Cancel();	
	
	void dialogAddTable2Comfirn();
	void dialogAddTable2Cancel();	

	//void dialogModifyTable1Comfirn();
	//void dialogModifyTable1Cancel();
	
	void dialogModifyTable2Comfirn();
	void dialogModifyTable2Cancel();
	
	//¼������
	void startedRecordVoice();
	void finishedRecordVoice();

signals :
	void returned();
	
	
private:
	Ui::VoiceIden *ui;
	QWSInputMethod *im;	
//bool m_isVoiseSource;  //¼����Դ�Ƿ����
	DialogAddTable1 *m_dat1;
	DialogAddTable2 *m_dat2;
	//DialogModifyTable1 *m_dmt1;
	DialogModifyTable2 *m_dmt2;

	void readDatabaseTable1();
	void readDatabaseTable2(QString &tableName);

};


#endif 

