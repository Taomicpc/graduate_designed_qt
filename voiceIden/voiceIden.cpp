/****************************************************************************
 ** object : VoiceIden 
 ** 毕业设计三级菜单　远程设置
 ** by luchaodong
 ** class Ui::VoiceIden : public Ui_VoiceIden {}
 ** VoiceIden 
 ****************************************************************************/
/*
 数据库操作使用语句拼接，主要注意参数是否需要加 '' 
 luchaodong 20150412
*/

#include <QtGui>
#include <QPixmap>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget> 
#include <QPalette>
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>

#include "voiceIden.h"
#include "dialogAddTable1.h"
#include "dialogAddTable2.h"
#include "dialogModifyTable1.h"
#include "dialogModifyTable2.h"
#include "dialogShowVoice.h"

using namespace std;

QSqlDatabase g_db;

VoiceIden::VoiceIden(QWidget *parent):
	QWidget(parent),
	ui(new Ui::VoiceIden),//Ui namespace ,not this 
	im(new TQInputMethod), 
	m_dat1(0), m_dat2(0), m_dmt1(0), m_dmt2(0),
	m_isVoiseSource(false), m_currentTable2Name("")
{
	ui->setupUi(this);

	ui->tableWidgetOne->setColumnWidth(0, 50);
	ui->tableWidgetOne->setColumnWidth(1, 150);
	ui->tableWidgetOne->setColumnWidth(2, 200);
	ui->tableWidgetOne->setColumnWidth(3, 100);

	ui->tableWidgetTwo->setColumnWidth(0, 50);
	ui->tableWidgetTwo->setColumnWidth(1, 50);
	ui->tableWidgetTwo->setColumnWidth(2, 150);
	ui->tableWidgetTwo->setColumnWidth(3, 200);
	ui->tableWidgetTwo->setColumnWidth(4, 100);

	//input
	QWSServer::setCurrentInputMethod(im);
	((TQInputMethod*)im)->setVisible(false);

	//设置背景图
	QRect screen_size = QApplication::desktop()->screenGeometry(); //get window size
	QPixmap pix("/opt/gb_ms/picture/background_1.jpg", 0, Qt::AutoColor);
	pix = pix.scaled(screen_size.width(), screen_size.height(), Qt::IgnoreAspectRatio); //photo size
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pix));
	setPalette(palette);

//按钮背景
	ui->buttonQuit->setText("");
	ui->buttonQuit->setFixedSize(81,32);
	ui->buttonQuit->setIconSize(QSize(81,32));
	pix.load("/opt/gb_ms/picture/return.png", 0, Qt::AutoColor);
	pix = pix.scaled(81, 32, Qt::IgnoreAspectRatio);
	ui->buttonQuit->setIcon(QIcon(pix));
	
	//信号 槽
	connect(ui->buttonQuit, SIGNAL(clicked()),
		this, SLOT(btQuitPushed()));
	
	connect(ui->buttonAddTable1, SIGNAL(clicked()),
		this, SLOT(btAddTable1Pushed()));

	connect(ui->buttonDeleteTable1, SIGNAL(clicked()),
		this, SLOT(btDeleteTable1Pushed()));

	connect(ui->buttonAddTable2, SIGNAL(clicked()),
		this, SLOT(btAddTable2Pushed()));

	connect(ui->buttonDeleteTable2, SIGNAL(clicked()),
		this, SLOT(btDeleteTable2Pushed()));

	connect(ui->buttonVoiceShow, SIGNAL(clicked()),
		this, SLOT(btVoiceShowPushed()));

	connect(ui->tableWidgetTwo, SIGNAL(cellDoubleClicked(int, int)),
		this, SLOT(modifyTable2(int, int)));

	connect(ui->tableWidgetOne, SIGNAL(cellDoubleClicked(int, int)),
		this, SLOT(modifyTable1(int, int)));
	connect(ui->tableWidgetOne, SIGNAL(currentCellChanged(int,int,int,int)),
		this, SLOT(updateTable2(int, int,int, int)));
	
	connect(ui->tableWidgetTwo, SIGNAL(currentCellChanged(int,int,int,int)),
		this, SLOT(updateTable2_2(int, int,int, int)));
	
	connect(ui->lineEditSelectOne, SIGNAL(textChanged(const QString&)),
		this, SLOT(setTableOneSelectItem(const QString&)));

	connect(ui->lineEditSelectTwo, SIGNAL(textChanged(const QString&)),
		this, SLOT(setTableTwoSelectItem(const QString&)));


//数据库建立连接
	g_db = QSqlDatabase::addDatabase("QSQLITE", "voiceIden");
	g_db.setDatabaseName("voice.db");
	if ( !g_db.open() )
	{
		QMessageBox::critical(NULL, QObject::trUtf8("无法打开数据库\n"),
			QObject::trUtf8("退出请按下yes"), QMessageBox::Yes,
				QMessageBox::No);
	}
	else
	{
		//read from database
		readDatabaseTable1();
		if (ui->tableWidgetOne->item(0, 0))
		{
			m_currentTable2Name = ui->tableWidgetOne->item(0, 1)->text();
			readDatabaseTable2(m_currentTable2Name);
		}
	}


}


VoiceIden::~VoiceIden()
{
	//disconnect with database 
	g_db.close();
	QString dbName = QSqlDatabase::database().connectionName();
	QSqlDatabase::removeDatabase(dbName);

	delete ui;
	delete im;
	if (m_dat1 != 0) delete m_dat1;
	if (m_dat2 != 0) delete m_dat2;
	if (m_dmt1 != 0) delete m_dmt1;
	if (m_dmt2 != 0) delete m_dmt2;
}


void VoiceIden::btQuitPushed()
{
	emit returned();
	close();
}


//弹框，向一级表添加新一项
void VoiceIden::btAddTable1Pushed()
{
	if ( !m_dat1 )
	{
		m_dat1 = new DialogAddTable1;
		
		connect(m_dat1, SIGNAL(operateConfirm()),
			this, SLOT(dialogAddTable1Comfirn()));
		connect(m_dat1, SIGNAL(cancel()),
			this, SLOT(dialogAddTable1Cancel()));
		
		connect(m_dat1, SIGNAL(startRecord()),
			this, SLOT(startedRecordVoice()));
		connect(m_dat1, SIGNAL(finishRecord()),
			this, SLOT(finishedRecordVoice()));
	}
	m_dat1->move(1,1);
	m_dat1->show();
	m_dat1->raise();
	m_dat1->activateWindow();
}


void VoiceIden::btDeleteTable1Pushed()
{
	int r = QMessageBox::warning(this, tr("Confirm To Delete"),
		tr("This execution will delete an item \n"
			"which you have choose from the database\n"
			"Are you sure ?"), QMessageBox::Yes | QMessageBox::No);

	if (r == QMessageBox::No)
		return;

	int iRow = ui->tableWidgetOne->currentRow();
	if (iRow < 0 || iRow >= ui->tableWidgetOne->rowCount())
	{
		QMessageBox::warning(this, tr("Edit Error"),
			tr("No Item has been chose!"), QMessageBox::Yes);
		return;
	}

	cout << "delete one item from WidgettableOne and database's table" << endl;
	QSqlQuery query(g_db);
	
	//读取 table1 语音识别内容
	QString content = ui->tableWidgetOne->item(iRow, 1)->text();
	
	//删除一级数据表中的项目
	QString queryString = tr("delete FROM table1 where contentIden='%1'").arg(content);
	if ( !query.exec(queryString) ) qDebug() << query.lastError();
	queryString.clear();
	
	//删除对应二级表
	queryString = tr("DROP TABLE IF EXISTS %1").arg(content);
	if ( !query.exec(queryString) ) qDebug() << query.lastError();

	//删除tablewidget 中显示项目	
	ui->tableWidgetOne->removeRow(iRow);
}


//双击，弹框，更新一级表中一项
void VoiceIden::modifyTable1(int r, int c)
{
	Q_UNUSED(c);

	if ( !m_dmt1 )
	{
		m_dmt1 = new DialogModifyTable1;
		
		connect(m_dmt1, SIGNAL(operateConfirm()),
  			this, SLOT(dialogModifyTable1Comfirn()));
  		connect(m_dmt1, SIGNAL(cancel()),
  			this, SLOT(dialogModifyTable1Cancel()));
	
		connect(m_dmt1, SIGNAL(startRecord()),
			this, SLOT(startedRecordVoice()));
		connect(m_dmt1, SIGNAL(finishRecord()),
			this, SLOT(finishedRecordVoice()));
		
	}
	//读取当前文本
	//int iRow = ui->tableWidgetOne->currentRow();  //-1 
	m_dmt1->setItemText(ui->tableWidgetOne->item(r,1)->text(), 0);
	m_dmt1->setItemText(ui->tableWidgetOne->item(r,2)->text(), 1);

	m_dmt1->move(420,1);
	m_dmt1->show();
	m_dmt1->raise();
	m_dmt1->activateWindow();
}


//弹框，向二级表添加新一项
void VoiceIden::btAddTable2Pushed()
{
	if ( !m_dat2 )
	{
		m_dat2 = new DialogAddTable2;
		
		connect(m_dat2, SIGNAL(operateConfirm()),
			this, SLOT(dialogAddTable2Comfirn()));
		connect(m_dat2, SIGNAL(cancel()),
			this, SLOT(dialogAddTable2Cancel()));
		
		connect(m_dat2, SIGNAL(startRecord()),
			this, SLOT(startedRecordVoice()));
		connect(m_dat2, SIGNAL(finishRecord()),
			this, SLOT(finishedRecordVoice()));
	}
	m_dat2->move(1,1);
	//m_dat2->setItemText(m_currentTable2Name, 0);
	m_dat2->show();
	m_dat2->raise();
	m_dat2->activateWindow();
}


void VoiceIden::btDeleteTable2Pushed()
{
	int r = QMessageBox::warning(this, tr("Confirm To Delete"),
		tr("This execution will delete an item \n"
			"which you have choose from the database\n"
			"Are you sure ?"), QMessageBox::Yes | QMessageBox::No);

	if (r == QMessageBox::No)
		return;

	int iRow = ui->tableWidgetTwo->currentRow();
	if (iRow < 0 || iRow >= ui->tableWidgetTwo->rowCount())
	{
		QMessageBox::warning(this, tr("Edit Error"),
			tr("No Item has been chose!"), QMessageBox::Yes);
		return;
	}

	cout << "delete one item from WidgettableTwo and database's table" << endl;
	QSqlQuery query(g_db);
	QString  content = ui->tableWidgetTwo->item(iRow, 2)->text();

	QString queryString = tr("delete FROM %1 where contentIden='%2'").arg(m_currentTable2Name).arg(content);
	//qDebug() << queryString ;
	if ( !query.exec(queryString) ) qDebug() << query.lastError();
	
	ui->tableWidgetTwo->removeRow(iRow);
}


void VoiceIden::btVoiceShowPushed()
{
	cout << "start voice iden" << endl;
	DialogShowVoice dsv;
	connect(&dsv, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	connect(&dsv, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	
	dsv.setText(m_currentTable2Name);	
	dsv.exec();
	//挂起
	//取消连接等
	//{}
	disconnect(&dsv, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(&dsv, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	
}


//双击，弹框，更新二级表中一项
void VoiceIden::modifyTable2(int r, int c)
{
	Q_UNUSED(c);
	if ( !m_dmt2 )
	{
		m_dmt2 = new DialogModifyTable2;
		
		connect(m_dmt2, SIGNAL(operateConfirm()),
  			this, SLOT(dialogModifyTable2Comfirn()));
  		connect(m_dmt2, SIGNAL(cancel()),
  			this, SLOT(dialogModifyTable2Cancel()));
	
		connect(m_dmt2, SIGNAL(startRecord()),
			this, SLOT(startedRecordVoice()));
		connect(m_dmt2, SIGNAL(finishRecord()),
			this, SLOT(finishedRecordVoice()));
		
	}
	//int iRow = ui->tableWidgetTwo->currentRow();  //-1 
	m_dmt2->setItemText(ui->tableWidgetTwo->item(r,0)->text(), 0);
	m_dmt2->setItemText(ui->tableWidgetTwo->item(r,2)->text(), 1);
	m_dmt2->setItemText(ui->tableWidgetTwo->item(r,3)->text(), 2);
	
	m_dmt2->move(1, 1);
	m_dmt2->show();
	m_dmt2->raise();
	m_dmt2->activateWindow();
}


//根据一级表中数据当前选项变化更新对应二级表
void VoiceIden::updateTable2(int currentRow,int currentColumn,int previousRow,int previousColumn)
{
	Q_UNUSED(previousRow);
	Q_UNUSED(previousColumn);
	Q_UNUSED(currentColumn);
	
//	ui->tableWidgetTwo->clear();
	//更新当前二级表名，显示
	m_currentTable2Name = ui->tableWidgetOne->item(currentRow, 1)->text();
	//qDebug() << table2Name;
	readDatabaseTable2(m_currentTable2Name);

	((TQInputMethod*)im)->setVisible(false);
}

void VoiceIden::updateTable2_2(int currentRow,int currentColumn,int previousRow,int previousColumn)
{
	Q_UNUSED(previousRow);
	Q_UNUSED(previousColumn);
	Q_UNUSED(currentColumn);
	Q_UNUSED(currentRow);
	

	((TQInputMethod*)im)->setVisible(false);
}


//根据输入框搜索定位到当前项目
void VoiceIden::setTableOneSelectItem(const QString& text)
{
	if (text.length() <= 0)
		return;
	
	QSqlQuery query(g_db);
	//输入name 查找 
	QString  queryString = tr("SELECT * FROM table1 where contentIden='%1'").arg(text);
	//qDebug() << queryString; 

	query.exec(queryString);
	//record 停留在第一条记录之前，需要执行next或者first 到第一条记录
	if (query.next())  //注意这里
	{		
		int thisId = query.record().indexOf("id");
		int row = query.value(thisId).toInt() -1 ;  //对应显示table上的编号，0开始
		ui->tableWidgetOne->setCurrentCell(row, 0);
	}	

}


void VoiceIden::setTableTwoSelectItem(const QString& text)
{
	if (text.length() <= 0)
		return;
	QSqlQuery query(g_db);
	//输入name 查找 
	QString  queryString = tr("SELECT * FROM %1 where contentIden='%2'").arg(m_currentTable2Name).arg(text);
	//qDebug() << queryString; 

	query.exec(queryString);
	//record 停留在第一条记录之前，需要执行next或者first 到第一条记录
	if (query.next())  //注意这里
	{		
		int thisId = query.record().indexOf("id");
		int row = query.value(thisId).toInt() -1 ;  //对应显示table上的编号，0开始
		ui->tableWidgetTwo->setCurrentCell(row, 0);
	}	


}


//数据表1 添加对话框确定写入触发
void VoiceIden::dialogAddTable1Comfirn()
{
	QSqlQuery query(g_db);
	//数据写入
	//插入数据表格 1
	query.prepare("insert into table1 (grand, contentIden, contentCom, code) values(1,?,?,'CODE-XXX')");
	query.bindValue(0, m_dat1->getItemText(0));
	query.bindValue(1, m_dat1->getItemText(1));
	if ( !query.exec() ) 
	{		
		qDebug() << query.lastError();
		cout << "Cant Insert to table1,  create table1" << endl;
		//新建第一个一级数据表格
		query.exec("create table table1 (id integer primary key,"
		   	"grand integer, contentIden varchar(20), contentCom varchar(20), code varchar(20))");
		query.prepare("insert into table1 (grand, contentIden, contentCom, code) values(1,?,?,'CODE-XXX')");
		query.bindValue(0, m_dat1->getItemText(0)); //插入带“”
		query.bindValue(1, m_dat1->getItemText(1));
		query.exec();
	}
	
	//新建对应数据表格2 ,表格名对应表格1数据项目
	QString  queryString = tr("create table %1 (id integer primary key,"
		      "id1 integer, grand integer, contentIden varchar(20), contentCom varchar(20), code varchar(20))").arg(m_dat1->getItemText(0)); 
	if ( !query.exec(queryString) )
	{		
		qDebug() << query.lastError();
	}
	queryString.clear();

/*
	queryString = tr("insert into %1 (name1, hecheng, name2) values('%2','aa','bb')").arg(m_dat1->getItemText(0)).arg(m_dat1->getItemText(0));
	if ( !query.exec(queryString) )
	{		
		qDebug() << query.lastError();
	}

*/	
	//更新表格1显示
	int iRow = ui->tableWidgetOne->rowCount();
	ui->tableWidgetOne->setRowCount(iRow+1);
	ui->tableWidgetOne->setItem(iRow, 0,  
		new QTableWidgetItem("1"));  //分级
	ui->tableWidgetOne->setItem(iRow, 1,
		new QTableWidgetItem(m_dat1->getItemText(0))); //识别内容
	ui->tableWidgetOne->setItem(iRow, 2,
		new QTableWidgetItem(m_dat1->getItemText(1))); //合成内容
	ui->tableWidgetOne->setItem(iRow, 3,
		new QTableWidgetItem("CODE-XXX"));            //code


	//对话框撤消
	disconnect(m_dat1, SIGNAL(operateConfirm()),
		this, SLOT(dialogAddTable1Comfirn()));
	disconnect(m_dat1, SIGNAL(cancel()),
		this, SLOT(dialogAddTable1Cancel()));
	disconnect(m_dat1, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dat1, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dat1;
	m_dat1 = 0;
	cout << "table1 add one item" << endl;
}


//数据表1 添加对话框取消操作对话框
void VoiceIden::dialogAddTable1Cancel()
{
	disconnect(m_dat1, SIGNAL(operateConfirm()),
		this, SLOT(dialogAddTable1Comfirn()));
	disconnect(m_dat1, SIGNAL(cancel()),
		this, SLOT(dialogAddTable1Cancel()));
	disconnect(m_dat1, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dat1, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dat1;
	m_dat1 = 0;
}


//数据表2 添加对话框确定写入触发
void VoiceIden::dialogAddTable2Comfirn()
{

	QSqlQuery query(g_db);
	//数据表名
//	int grand = 0;
	
	//插入数据表格 2
	QString  queryString = tr("insert into %1 (id1, grand, contentIden, contentCom, code) values(%2, 2,'%3','%4','CODE-YYY')").arg(m_currentTable2Name).arg(m_dat1->getItemText(0).toInt()).arg(m_dat2->getItemText(1)).arg(m_dat2->getItemText(2)); 
	
	if ( !query.exec(queryString) )
	{		
		qDebug() << query.lastError();
	}

	//更新表格2显示
	int iRow = ui->tableWidgetTwo->rowCount();
	ui->tableWidgetTwo->setRowCount(iRow+1);
	ui->tableWidgetTwo->setItem(iRow, 0,
		new QTableWidgetItem(m_dat2->getItemText(0))); //id_1
	ui->tableWidgetTwo->setItem(iRow, 1,
		new QTableWidgetItem("2"));   //分级
	ui->tableWidgetTwo->setItem(iRow, 2,
		new QTableWidgetItem(m_dat2->getItemText(1)));
	ui->tableWidgetTwo->setItem(iRow, 3,
		new QTableWidgetItem(m_dat2->getItemText(2)));
	ui->tableWidgetTwo->setItem(iRow, 4,
		new QTableWidgetItem("CODE-YYY"));


	//对话框撤消
	disconnect(m_dat2, SIGNAL(operateConfirm()),
		this, SLOT(dialogAddTable2Comfirn()));
	disconnect(m_dat2, SIGNAL(cancel()),
		this, SLOT(dialogAddTable2Cancel()));
	disconnect(m_dat2, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dat2, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dat2;
	m_dat2 = 0;
	cout << "table2 add one item" << endl;
}


//数据表2 添加对话框取消操作对话框
void VoiceIden::dialogAddTable2Cancel()
{
	disconnect(m_dat2, SIGNAL(operateConfirm()),
		this, SLOT(dialogAddTable2Comfirn()));
	disconnect(m_dat2, SIGNAL(cancel()),
		this, SLOT(dialogAddTable2Cancel()));
	disconnect(m_dat2, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dat2, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dat2;
	m_dat2 = 0;
}


void VoiceIden::dialogModifyTable1Comfirn()
{

	QSqlQuery query(g_db);
	//数据写入
	int iRow = ui->tableWidgetOne->currentRow();
	QString table2OldName = ui->tableWidgetOne->item(iRow,1)->text();
	QString table2NewName = m_dmt1->getItemText(0);
	
	//更新数据表格1
	QString  queryString = tr("update table1 set contentIden='%1', contentCom='%2' where contentIden='%3'").arg(table2NewName).arg(m_dmt1->getItemText(1)).arg(table2OldName); 
	
	if ( !query.exec(queryString) )
	{		
		qDebug() << query.lastError();
	}

	if (m_dmt1->isVoiceRecordChecked())  //选择更新语音
	{
		cout << "update voice record" << endl;
	
	
	}

	queryString.clear();
	//关联数据表二处理 对应更新
	/////修改表名
	queryString = tr("alter table %1 rename to %2").arg(table2OldName).arg(table2NewName); 
	if ( !query.exec(queryString) )
	{		
		qDebug() << query.lastError();
	}
	queryString.clear();
	//更新表格2项目
	//queryString = tr("update %1 set name1='%2'").arg(table2NewName).arg(table2NewName); 
	//if ( !query.exec(queryString) )
	//{		
	//	qDebug() << query.lastError();
	//}


	ui->tableWidgetOne->setItem(iRow, 0,
	         new QTableWidgetItem("1"));
	
	ui->tableWidgetOne->setItem(iRow, 1,
	         new QTableWidgetItem(m_dmt1->getItemText(0)));

	ui->tableWidgetOne->setItem(iRow, 2,
	         new QTableWidgetItem(m_dmt1->getItemText(1)));

	ui->tableWidgetOne->setItem(iRow, 3,
	         new QTableWidgetItem("CODE-XXX"));

	
	disconnect(m_dmt1, SIGNAL(operateConfirm()),
  		this, SLOT(dialogModifyTable1Comfirn()));
  	disconnect(m_dmt1, SIGNAL(cancel()),
  		this, SLOT(dialogModifyTable1Cancel()));
	
	disconnect(m_dmt1, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dmt1, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dmt1;
	m_dmt1 = 0;
}


void VoiceIden::dialogModifyTable1Cancel()
{
	disconnect(m_dmt1, SIGNAL(operateConfirm()),
  		this, SLOT(dialogModifyTable1Comfirn()));
  	disconnect(m_dmt1, SIGNAL(cancel()),
  		this, SLOT(dialogModifyTable1Cancel()));
	
	disconnect(m_dmt1, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dmt1, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dmt1;
	m_dmt1 = 0;
}



void VoiceIden::dialogModifyTable2Comfirn()
{

	QSqlQuery query(g_db);
	//数据写入
	int iRow = ui->tableWidgetTwo->currentRow();
	QString table2Name = m_currentTable2Name;
	QString content =  ui->tableWidgetTwo->item(iRow,2)->text();  //区分项
	
	//插入数据表格 2
	QString  queryString = tr("update %1 set id1=%2, contentIden='%3', contentCom='%4'where contentIden=%5").arg(table2Name).arg(m_dmt2->getItemText(0).toInt()).arg(m_dmt2->getItemText(1)).arg(m_dmt2->getItemText(2)).arg(content); 
	
	if ( !query.exec(queryString) )
	{		
		qDebug() << query.lastError();
	}

	if (m_dmt2->isVoiceRecordChecked())  //选择更新语音
	{
		cout << "update voice record" << endl;
	
	
	}

	//ui->tableWidgetTwo->setItem(iRow, 0,
	//         new QTableWidgetItem(m_dmt2->getItemText(0)));
	
	ui->tableWidgetTwo->setItem(iRow, 0,
	         new QTableWidgetItem(m_dmt2->getItemText(0)));
	ui->tableWidgetTwo->setItem(iRow, 1,
	         new QTableWidgetItem("2"));
	ui->tableWidgetTwo->setItem(iRow, 2,
	         new QTableWidgetItem(m_dmt2->getItemText(1)));
	ui->tableWidgetTwo->setItem(iRow, 3,
	         new QTableWidgetItem(m_dmt2->getItemText(2)));
	ui->tableWidgetTwo->setItem(iRow, 4,
	         new QTableWidgetItem("CODE-YYY"));


	
	disconnect(m_dmt2, SIGNAL(operateConfirm()),
  		this, SLOT(dialogModifyTable2Comfirn()));
  	disconnect(m_dmt2, SIGNAL(cancel()),
  		this, SLOT(dialogModifyTable2Cancel()));
	
	disconnect(m_dmt2, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dmt2, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dmt2;
	m_dmt2 = 0;
}


void VoiceIden::dialogModifyTable2Cancel()
{
	disconnect(m_dmt2, SIGNAL(operateConfirm()),
  		this, SLOT(dialogModifyTable2Comfirn()));
  	disconnect(m_dmt2, SIGNAL(cancel()),
  		this, SLOT(dialogModifyTable2Cancel()));
	
	disconnect(m_dmt2, SIGNAL(startRecord()),
		this, SLOT(startedRecordVoice()));
	disconnect(m_dmt2, SIGNAL(finishRecord()),
		this, SLOT(finishedRecordVoice()));
	delete m_dmt2;
	m_dmt2 = 0;
}


//启动录音操作
void VoiceIden::startedRecordVoice()
{
	cout << "Voice record start" << endl;
	m_isVoiseSource = false;



}


//结束录音操作
void VoiceIden::finishedRecordVoice()
{
	cout << "Voice record finish" << endl;
	m_isVoiseSource = true;



}


void VoiceIden::readDatabaseTable1()
{
	QSqlQuery query(g_db);
	query.prepare("SELECT * FROM table1");
	if( query.exec())
	{
		int iRow = 0;
		//table's item 
		int grand = query.record().indexOf("grand");
		int contentIden = query.record().indexOf("contentIden");
		int contentCom = query.record().indexOf("contentCom");
		int theCode = query.record().indexOf("code");

		while( query.next())
		{
			ui->tableWidgetOne->setRowCount(iRow+1);

			ui->tableWidgetOne->setItem(iRow, 0,
				new QTableWidgetItem(query.value(grand).toString()));

			ui->tableWidgetOne->setItem(iRow, 1,
				new QTableWidgetItem(query.value(contentIden).toString()));
			
			ui->tableWidgetOne->setItem(iRow, 2,
				new QTableWidgetItem(query.value(contentCom).toString()));
			
			ui->tableWidgetOne->setItem(iRow, 3,
				new QTableWidgetItem(query.value(theCode).toString()));
			
			iRow++;
		}
	}
}


void VoiceIden::readDatabaseTable2(QString &tableName)
{
	QSqlQuery query(g_db);
	QString  queryString = tr("SELECT * FROM %1").arg(tableName);
	qDebug() << queryString;
	ui->tableWidgetTwo->setRowCount(0);
	if ( query.exec(queryString) )
	{
		int iRow = 0;
		int id1 = query.record().indexOf("id1");
		int grand = query.record().indexOf("grand");
		int contentIden = query.record().indexOf("contentIden");
		int contentCom = query.record().indexOf("contentCom");
		int theCode = query.record().indexOf("code");

		while (query.next())
		{
			
			ui->tableWidgetTwo->setRowCount(iRow+1);
			ui->tableWidgetTwo->setItem(iRow, 0,
				new QTableWidgetItem(query.value(id1).toString()));

			ui->tableWidgetTwo->setItem(iRow, 1,
				new QTableWidgetItem(query.value(grand).toString()));

			ui->tableWidgetTwo->setItem(iRow, 2,
				new QTableWidgetItem(query.value(contentIden).toString()));
			
			ui->tableWidgetTwo->setItem(iRow, 3,
				new QTableWidgetItem(query.value(contentCom).toString()));
			
			ui->tableWidgetTwo->setItem(iRow, 4,
				new QTableWidgetItem(query.value(theCode).toString()));
			
			iRow++;
		}
	}
}

