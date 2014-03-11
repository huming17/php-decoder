#ifndef DEZENDWIDGET_H
#define DEZENDWIDGET_H

#include <QWidget>

namespace Ui {
class DeZendWidget;
}

class DeZendWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DeZendWidget(QWidget *parent = 0);
	~DeZendWidget();

private slots:
	void on_browFileButton_clicked();
	void on_browDirButton_clicked();
	void on_decodeButton_clicked();

private:
	Ui::DeZendWidget *ui;
	bool decodeFile(const QString &path, const QString &resultPath);
	void copyDecodeDll(int mode);
};

#endif // DEZENDWIDGET_H
