/****************************************************************************
 ** object : VoiceIden 
 ** 毕业设计三级菜单　语音识别
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
	

	//数据表操作函数
	void dialogAddTable1Comfirn();
	void dialogAddTable1Cancel();	
	
	void dialogAddTable2Comfirn();
	void dialogAddTable2Cancel();	


	//录音操作
	void startedRecordVoice();
	void finishedRecordVoice();

signals :
	void returned();
	
	
private:
	Ui::VoiceIden *ui;
	QWSInputMethod *im;	
//bool m_isVoiseSource;  //录音资源是否可用
	DialogAddTable1 *m_dat1;
	DialogAddTable2 *m_dat2;

	void readDatabaseTable1();
	void readDatabaseTable2(QString &tableName);

};


#endif 

