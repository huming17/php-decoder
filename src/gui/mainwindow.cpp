#include "deevalwidget.h"
#include "dezendwidget.h"
#include "about.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->tabWidget->addTab(new DeEvalWidget(this),"DeEval");
	ui->tabWidget->addTab(new DeZendWidget(this),"DeZend");
	ui->tabWidget->addTab(new About(this),"About");
}

MainWindow::~MainWindow()
{
	delete ui;
}
