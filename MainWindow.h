#pragma once
#include <QtWidgets/QDialog>
#include "ui_MainWindow.h"

class Ewnd;
class MainWindow :public QWidget
{
	Q_OBJECT
public:
	MainWindow(Ewnd* pEwnd, QWidget* parent = Q_NULLPTR);
	~MainWindow();
private slots:
	void on_listMenu(const QPoint& pos);
	void on_LoadWindow();
	void on_ButtonClicked(bool);
private:
	Ui::MainWindow ui;
	Ewnd* m_pEwnd = nullptr;
};