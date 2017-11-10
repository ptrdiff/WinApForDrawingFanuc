#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QWidget>
#include "ui_mainwindow.h"

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private slots:
	void Picture_before_updating(); // слот для обновления картинки до
	void CannyThreshold();
	void Blur_Change(int);
	void Canny_min_Change(int);
	void Canny_max_Change(int);
	void printmatrix();
	void Draw_speed_Change(int);

private:
	Ui::MainWindowClass *ui;
};
#endif // MAINWINDOW_H
