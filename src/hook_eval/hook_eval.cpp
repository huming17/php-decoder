#define HOOK_EVAL_EXPORTS
#include <stdio.h>
#include "hook_eval.h"
#include <QLocalSocket>
#include <QDebug>

HOOK_EVAL_API void hook_eval(char* str,unsigned long length)
{
	QByteArray string(str,length);
	QLocalSocket socket;
	socket.connectToServer("phpdecoder");
	if ( socket.waitForConnected(1000) ) {
		qDebug()<<"connected!";
		qDebug()<<socket.write(string);
		qDebug()<<socket.waitForBytesWritten(1000);
		socket.close();
	} else {
		qDebug()<<socket.error()<<socket.errorString();
	}
	qDebug()<<string;
}
