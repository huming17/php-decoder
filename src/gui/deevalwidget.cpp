#include <QLocalSocket>
#include <QLocalServer>
#include <QFileDialog>

#include <QProcess>
#include <QTimer>
#include <QDir>

#include <QDebug>

#include "util.h"
#include "deevalwidget.h"
#include "ui_deevalwidget.h"

typedef void (QProcess::*QPROCESS_FINISH_SIGNAL_TYPE)
		(int exitCode, QProcess::ExitStatus exitStatus);

DeEvalWidget::DeEvalWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DeEvalWidget)
{
	ui->setupUi(this);
}

DeEvalWidget::~DeEvalWidget()
{
	delete ui;
}

void DeEvalWidget::on_browFileButton_clicked()
{
	QString path = QFileDialog::getOpenFileName(this,
		QStringLiteral("选择文件"),ui->pathEdit->text());
	if ( path.isEmpty() ) {
		return;
	}
	ui->pathEdit->setText(path);
}

void DeEvalWidget::on_browDirButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this,
		QStringLiteral("选择文件夹"),ui->pathEdit->text());
	if ( path.isEmpty() ) {
		return;
	}
	ui->pathEdit->setText(path);
}

void DeEvalWidget::on_decodeButton_clicked()
{
	ui->progressBar->setValue(0);
	ui->logTextEdit->clear();

	QString text = ui->pathEdit->text();
	QString rootPath;
	if ( QFileInfo(text).isFile() ) {
		rootPath = QFileInfo(text).absoluteDir().absolutePath();
	} else {
		rootPath = QDir(text).absolutePath();
	}
	QString resultDirPath = createTempDir();

	QStringList fileList = getFileList(text);
	for(int i=0;i<fileList.length();i++){
		QString file = fileList.value(i);
		bool checked = ui->phpFileOnlyCheckBox->checkState()==Qt::Checked;
		bool isPhp = (new QFileInfo(file))->suffix()=="php";
		if ( checked && isPhp || !checked ) {
			QString relativePath = QDir(rootPath).relativeFilePath(file);
			ui->logTextEdit->appendPlainText(file+"...");
			if ( decodeFile(file, resultDirPath + "/" + relativePath) ){
				ui->logTextEdit->appendPlainText(QStringLiteral("完成"));
			} else {
				ui->logTextEdit->appendPlainText(QStringLiteral("失败"));
			}
		}
		ui->progressBar->setValue((i+1.0)/fileList.length()*100);
		qApp->processEvents();
	}
	QProcess::startDetached("explorer",QStringList(QDir::toNativeSeparators(resultDirPath)));
}
bool DeEvalWidget::decodeFile(const QString &path, const QString &resultPath)
{
	//创建临时文件
	QString tempFilePath = ".\\deeval\\sandbox\\temp.php";
	if ( !overwriteFile(path,tempFilePath) ) {
		qDebug()<<"copy file error!";
		return false;
	}

	QLocalServer server;
	server.listen("phpdecoder");

	//创建解密php进程
	QProcess process, *pProcess = &process;
	QStringList args;
	args<<tempFilePath<<"-c"<<".\\deeval\\php.ini";
	process.setArguments(args);
	process.setProgram(".\\deeval\\php.exe");
	process.start();

	bool isRunning = process.waitForStarted();
	bool *pIsRunning = &isRunning;
	if ( !isRunning ) {
		qDebug()<<"start process error: "<<process.error()<<process.errorString();
		return false;
	}
	connect(&process,(QPROCESS_FINISH_SIGNAL_TYPE)&QProcess::finished,[=](){
		*pIsRunning = false;
		qDebug()<<"output:"<<pProcess->readAll();
	});

	//解密php进程超时处理
	QTimer timer;
	timer.setInterval(10*1000);
	connect(&timer,&QTimer::timeout,[=](){
		qDebug()<<"php process timeout!";
		pProcess->terminate();
		*pIsRunning = false;
	});
	timer.start();

	//接收解密数据
	QByteArray data;
	while(isRunning) {
		if ( server.waitForNewConnection(50) ) {
			QLocalSocket *socket = server.nextPendingConnection();
			data.clear();
			while ( socket->waitForReadyRead(-1) ) {
				data += socket->readAll();
				qApp->processEvents();
			}
			socket->close();
			delete socket;
			qDebug()<<data;
		}
		qApp->processEvents();
	}
	if ( data.isEmpty() ) {
		qDebug()<<"no data received!";
		return false;
	}

	data = "<?php " + data + "?>";

	if ( ui->prettyCheckBox->checkState()==Qt::Checked ) {
		data = prettyCode(data);
	}

	//写入新文件
	return putFileContents(resultPath,data);
}
