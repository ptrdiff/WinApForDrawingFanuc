#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "mainwindow.h"
#include <list> // библиотека для реализации списка списков
#include <fstream>// библиотека для вывода в файл координат
#include <ctime>
#include"Socket.h"
#include <QtWidgets/QFileDialog>

cv::Mat src;// матрица для исходного изображения 
cv::Mat src_grey;// матрица для изображения в градациях серого
cv::Mat dst;//матрица после размытия 
cv::Mat grany;//матрица после детектора границ 
cv::Mat grany_copy;// копия матрицы grany
int karnelsize = 1;// параметр для размытия
int canny_theshold_min = 0;// нижняя граница для детектора канни 
int canny_theshold_max = 0;// верхняя граница для детектора канни
int speedDraw = 0;

struct Coords {
	short int x;
	short int y;
};


MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::MainWindowClass)
{
	ui->setupUi(this);
	connect(ui->FileName, SIGNAL(clicked()), this, SLOT(Picture_before_updating()));
	connect(ui->Blur, SIGNAL(valueChanged(int)), this, SLOT(Blur_Change(int)));
	connect(ui->Canny_min, SIGNAL(valueChanged(int)), this, SLOT(Canny_min_Change(int)));
	connect(ui->Canny_max, SIGNAL(valueChanged(int)), this, SLOT(Canny_max_Change(int)));
	connect(ui->Button, SIGNAL(clicked()), this, SLOT(printmatrix()));
	connect(ui->Draw_speed, SIGNAL(valueChanged(int)), this, SLOT(Draw_speed_Change(int)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::Picture_before_updating()
{
	ui->Blur->setValue(0);
	ui->Canny_min->setValue(0);
	ui->Canny_max->setValue(0);
	ui->Draw_speed->setValue(0);

	QUrl filename = QFileDialog::getOpenFileUrl();
	std::string filename1 = filename.toString().toStdString();
	filename1 = filename1.substr(8, filename1.size() - 8);
	src = cv::imread(filename1, 1);


	if ((src.cols >= 300) || (src.rows >= 350)) {
		int width, height;
		width = src.cols;
		height = src.rows;
		double cofx = (double)width / 300.;
		double cofy = (double)height / 350.;
		double cof = 1. / (((cofx) > (cofy)) ? (cofx) : (cofy));
		cv::resize(src, src, cv::Size(), cof, cof);
	}
	cv::Mat temp;

	cvtColor(src, src_grey, CV_BGR2GRAY);

	cvtColor(src, temp, CV_BGR2RGB);
	QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	ui->Picture_before->setPixmap(QPixmap::fromImage(dest));


	cv::GaussianBlur(src_grey, dst, cv::Size(1, 1), 0, 0);
	cv::Canny(dst, grany, 0, 0);


	cvtColor(grany, temp, CV_GRAY2RGB);
	QImage dest1((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	ui->Picture_after->setPixmap(QPixmap::fromImage(dest1));

}

void MainWindow::CannyThreshold()
{
	cv::GaussianBlur(src_grey, dst, cv::Size(abs(2 * karnelsize - 1), abs(2 * karnelsize - 1)), 0, 0);
	cv::Canny(dst, grany, canny_theshold_min, canny_theshold_max);
	cv::Mat temp;
	cvtColor(grany, temp, CV_GRAY2RGB);
	QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	ui->Picture_after->setPixmap(QPixmap::fromImage(dest));
}

void MainWindow::Blur_Change(int value) {
	ui->Blur->setValue(value);
	ui->label->setText(QString::number(value));
	karnelsize = ui->Blur->value();
	MainWindow::CannyThreshold();
};

void MainWindow::Canny_min_Change(int value) {
	ui->Canny_min->setValue(value);
	ui->label_2->setText(QString::number(value));
	canny_theshold_min = ui->Canny_min->value();
	MainWindow::CannyThreshold();
};

void MainWindow::Canny_max_Change(int value) {
	ui->Canny_max->setValue(value);
	ui->label_3->setText(QString::number(value));
	canny_theshold_max = ui->Canny_max->value();
	MainWindow::CannyThreshold();
};

void MainWindow::Draw_speed_Change(int value) {
	ui->Draw_speed->setValue(value);
	ui->label_9->setText(QString::number(value));
	speedDraw = ui->Draw_speed->value();
};

void MainWindow::printmatrix() {

	grany_copy = grany.clone();
	for (int i = 0; i < grany_copy.rows; ++i)
		for (int j = 0; j < grany_copy.cols; ++j)
			if (grany_copy.at<uchar>(i, j) > 0)
				grany_copy.at<uchar>(i, j) = 255;

	std::list<std::list<Coords>> Coords_List;
	std::list<Coords> temp;
	short int rows = grany_copy.rows;
	short int coloms = grany_copy.cols;
	short int x;
	short int y;
	short int control_x;
	short int control_y;

	for (short int i = 0; i < rows; ++i) {
		for (short int j = 0; j < coloms; ++j) {
			if (grany_copy.at<uchar>(i, j) == 255) {
				x = j;
				y = i;
				do {
					control_x = x;
					control_y = y;

					temp.push_back({ x , y });
					grany_copy.at<uchar>(y, x) = 0;

					if (((y + 1) < rows) && (grany_copy.at<uchar>(y + 1, x) == 255)) { ++y; continue; }
					if (((x + 1) < coloms) && (grany_copy.at<uchar>(y, x + 1) == 255)) { ++x; continue; }
					if (((y - 1) >= 0) && (grany_copy.at<uchar>(y - 1, x) == 255)) { --y; continue; }
					if (((x - 1) >= 0) && (grany_copy.at<uchar>(y, x - 1) == 255)) { --x; continue; }
					if (((y + 1) < rows) && ((x + 1) < coloms) && (grany_copy.at<uchar>(y + 1, x + 1) == 255)) { ++x; ++y; continue; }
					if (((y - 1) >= 0) && ((x - 1) >= 0) && (grany_copy.at<uchar>(y - 1, x - 1) == 255)) { --x; --y; continue; }
					if (((y - 1) >= 0) && ((x + 1) < coloms) && (grany_copy.at<uchar>(y - 1, x + 1) == 255)) { ++x; --y; continue; }
					if (((y + 1) < rows) && ((x - 1) >= 0) && (grany_copy.at<uchar>(y + 1, x - 1) == 255)) { --x; ++y; continue; }

				} while ((control_x != x) || (control_y != y));
				if (!Coords_List.empty()) {

					if ((abs(Coords_List.back().front().x - temp.front().x) <= 1) && (abs(Coords_List.back().front().y - temp.front().y) <= 1)) {
						Coords_List.back().reverse();
						Coords_List.back().splice(Coords_List.back().end(), temp);
					}
					else if ((abs(Coords_List.back().back().x - temp.back().x) <= 1) && (abs(Coords_List.back().back().y - temp.back().y) <= 1)) {
						temp.reverse();
						Coords_List.back().splice(Coords_List.back().end(), temp);
					}
					else if ((abs(Coords_List.back().front().x - temp.back().x) <= 1) && (abs(Coords_List.back().front().y - temp.back().y) <= 1)) {
						Coords_List.back().reverse();
						temp.reverse();
						Coords_List.back().splice(Coords_List.back().end(), temp);
					}
					else if ((abs(Coords_List.back().back().x - temp.front().x) <= 1) && (abs(Coords_List.back().back().y - temp.front().y) <= 1)) {
						Coords_List.back().splice(Coords_List.back().end(), temp);
					}
					else Coords_List.push_back(temp);

				}
				else Coords_List.push_back(temp);
				temp.clear();
			}
		}
	}

	char outputfilename[] = { "new.txt" };
	std::ofstream outfile(outputfilename);
	outfile << coloms << " ";
	outfile << rows << " ";
	for (std::list<std::list<Coords>>::iterator uter = Coords_List.begin(); uter != Coords_List.end(); ++uter) {
		std::list<Coords> Vnutr_list = *uter;
		for (std::list<Coords>::iterator iter = Vnutr_list.begin(); iter != Vnutr_list.end();) {
				Coords Coordinats = *iter;
				if (iter != Vnutr_list.begin()) outfile << " ";
				outfile << Coordinats.x;
				outfile << " " << Coordinats.y;
				++iter;
			for (int i = 0; i < speedDraw && iter != Vnutr_list.end(); i++) {
				++iter;
				if (iter == Vnutr_list.end()) {
					--iter;
					outfile << " ";
					Coords Coordinats = *iter;
					outfile << Coordinats.x;
					outfile << " " << Coordinats.y;
					++iter;
				}
			}
		}
		outfile << "|" << std::endl;
	}
	outfile.close();

	//socketmain(outputfilename);

	ui->Blur->setValue(0);
	ui->Canny_min->setValue(0);
	ui->Canny_max->setValue(0);
	ui->Draw_speed->setValue(0);
}