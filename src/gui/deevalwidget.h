#ifndef DEEVALWIDGET_H
#define DEEVALWIDGET_H

#include <QWidget>

namespace Ui {
class DeEvalWidget;
}

class DeEvalWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DeEvalWidget(QWidget *parent = 0);
	~DeEvalWidget();

private slots:
	void on_browFileButton_clicked();
	void on_browDirButton_clicked();
	void on_decodeButton_clicked();

private:
	Ui::DeEvalWidget *ui;
	bool decodeFile(const QString &path, const QString &resultPath);
};

#endif // DEEVALWIDGET_H
