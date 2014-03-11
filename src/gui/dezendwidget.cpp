#include <QDir>
#include <QFileInfo>

#include <QFileDialog>

#include <QProcess>
#include <QTimer>
#include <QFile>

#include <QDebug>

#include "util.h"
#include "dezendwidget.h"
#include "ui_dezendwidget.h"

typedef void (QProcess::*QPROCESS_FINISH_SIGNAL_TYPE)
		(int exitCode, QProcess::ExitStatus exitStatus);

DeZendWidget::DeZendWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DeZendWidget)
{
	ui->setupUi(this);
	connect(ui->mode1Button,&QRadioButton::toggled,[=](bool checked){
		if ( !checked ) return;
		copyDecodeDll(1);
	});
	connect(ui->mode2Button,&QRadioButton::toggled,[=](bool checked){
		if ( !checked ) return;
		copyDecodeDll(2);
	});
	connect(ui->mode3Button,&QRadioButton::toggled,[=](bool checked){
		if ( !checked ) return;
		copyDecodeDll(3);
	});

	ui->mode1Button->setChecked(true);
	copyDecodeDll(1);
}

DeZendWidget::~DeZendWidget()
{
	delete ui;
}

void DeZendWidget::copyDecodeDll(int mode)
{
	QString nf = QString(".\\dezend\\php5ts.dll");
	QString of = QString(".\\dezend\\php5ts.dll.%1").arg(mode);
	if ( QFile(nf).exists() && QFile(nf).size() == QFile(of).size() ) {
		return;
	}
	QFile::remove(nf);
	QFile::copy(of,nf);
}

void DeZendWidget::on_browFileButton_clicked()
{
	QString path = QFileDialog::getOpenFileName(this,
		QStringLiteral("选择文件"),ui->pathEdit->text());
	if ( path.isEmpty() ) {
		return;
	}
	ui->pathEdit->setText(path);
}

void DeZendWidget::on_browDirButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this,
		QStringLiteral("选择文件夹"),ui->pathEdit->text());
	if ( path.isEmpty() ) {
		return;
	}
	ui->pathEdit->setText(path);
}

void DeZendWidget::on_decodeButton_clicked()
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
bool DeZendWidget::decodeFile(const QString &path, const QString &resultPath)
{
	//创建临时文件
	QString tempFilePath = ".\\dezend\\temp\\temp.php";
	if ( !overwriteFile(path,tempFilePath) ) {
		qDebug()<<"copy file error!";
		return false;
	}

	//创建解密php进程
	QProcess process, *pProcess = &process;
	QStringList args;
	args<<tempFilePath<<"/tab"<<"/indent:1"<<"/path:./dezend/temp/";
	process.setArguments(args);
	process.setProgram(".\\dezend\\php.exe");
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

	//等待解密进程结束
	while ( !process.waitForFinished(50) && process.state()==QProcess::Running ) {
		qApp->processEvents();
	}

	QString decodedFile;
	if ( ui->mode1Button->isChecked() ) {
		decodedFile = ".\\dezend\\temp\\temp.de.php";
	} else {
		decodedFile = ".\\dezend\\main\xe5\xac\x83\xe7\xbc\xb7\xe8\x83\x80\xe6\xa6\xa8.log";
	}
	QByteArray data = getFileContents(decodedFile);
	if ( data.isEmpty() ) {
		qDebug()<<"read decoded file error!";
		return false;
	}
	QFile(tempFilePath).remove();
	QFile(decodedFile).remove();

	if ( ui->prettyCheckBox->checkState()==Qt::Checked ) {
		QProcess process;
		process.setProgram(".\\phpcb.exe");
		process.start();
		if ( process.waitForStarted() ){
			process.write(data);
			process.closeWriteChannel();
			process.waitForFinished();
			data = process.readAll();
		} else {
			qDebug()<<"pretty code failed!";
		}
		qApp->processEvents();
	}

	//写入新文件
	QDir().mkpath(QFileInfo(resultPath).absoluteDir().absolutePath());
	return putFileContents(resultPath,data);
}
