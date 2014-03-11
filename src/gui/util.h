#ifndef UTIL_H
#define UTIL_H

#include <QStringList>
QStringList getFileList(const QString &path);
bool overwriteFile(QString from, QString to);
QByteArray prettyCode(QByteArray code);
bool putFileContents(QString filename, QByteArray data);
QByteArray getFileContents(QString filename);
QString createTempDir();

#endif // UTIL_H
