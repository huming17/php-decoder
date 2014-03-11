#include <QApplication>
#include <QStringList>
#include <QDir>
#include <QFileInfo>

#include <QProcess>
#include <QDateTime>

#include <QDebug>

#include "util.h"

QStringList getFileList(const QString &path)
{
	QFileInfo fileinfo(path);
	if ( fileinfo.isFile() ) {
		return QStringList(path);
	} else if ( fileinfo.isDir() ) {
		QStringList fileList;
		QDir dir(fileinfo.absoluteFilePath());
		dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
		QFileInfoList fileinfoList = dir.entryInfoList();
		QFileInfoList::ConstIterator i;
		for( i=fileinfoList.cbegin();i!=fileinfoList.cend();i++ ) {
			QFileInfo fileinfo = *i;
			if ( fileinfo.isDir() ){
				fileList.append(getFileList(fileinfo.absoluteFilePath()));
			} else if( fileinfo.isFile() ) {
				fileList.append(fileinfo.absoluteFilePath());
			}
		}
		qApp->processEvents();
		return fileList;
	} else {
		return QStringList();
	}
}

bool overwriteFile(QString from, QString to)
{
	if ( QFile::exists(to) ) {
		if ( !QFile::remove(to) ) {
			return false;
		}
	}

	return QFile::copy(from,to);
}

QByteArray prettyCode(QByteArray code){
	QProcess process;
	process.setProgram(".\\phpcb.exe");
	process.start();
	if ( process.waitForStarted() ){
		process.write(code);
		process.closeWriteChannel();
		process.waitForFinished();
		code = process.readAll();
	} else {
		qDebug()<<"pretty code failed!";
	}
	qApp->processEvents();
	return code;
}

bool putFileContents(QString filename, QByteArray data)
{
	QFile file(filename);
	if ( !file.open(QIODevice::WriteOnly) ) {
		return false;
	}
	qint64 written = file.write(data);
	file.close();
	return data.length() == written;
}

QByteArray getFileContents(QString filename)
{
	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	return file.readAll();
}

QString createTempDir()
{
	QString tempSubDir = QString::number(QDateTime::currentMSecsSinceEpoch());
	QDir temp = QDir::temp();
	temp.mkdir(tempSubDir);
	temp.cd(tempSubDir);
	return temp.absolutePath();
}
