#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <iostream> // вывод в консоль 
#include <list> // библиотека для реализации списка списков
#include <vector>//библеотека реализации вектора
#include <fstream>// библиотека для вывода в файл координат

//-----------------------------------------------------------------------------------------------------------
// глобальные переменные 
cv::Mat src;// матрица для исходного изображения 
cv::Mat src_grey;// матрица для изображения в градациях серого
cv::Mat dst;//матрица после размытия 
cv::Mat grany;//матрица после детектора границ 
cv::Mat grany_copy;// копия матрицы grany
int karnelsize = 1;// параметр для размытия
int canny_theshold_min = 0;// нижняя граница для детектора канни 
int canny_theshold_max = 0;// верхняя граница для детектора канни
int noize_filter = 0;// нижняя граница для длины контуров 
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
//создание структуры в которой будем хранить координаты
struct Coords {
	short int x;
	short int y;
};
//-----------------------------------------------------------------------------------------------------------

void printmatrix(int, void*);

//-----------------------------------------------------------------------------------------------------------
// ползунок для детектирования границ
void CannyThreshold(int, void*)
{   //размытие гаусса
	cv::GaussianBlur(src_grey, dst, cv::Size(abs(2 * karnelsize - 1), abs(2 * karnelsize - 1)), 0, 0);

	// Детектор границ канни 
	cv::Canny(dst, grany, canny_theshold_min, canny_theshold_max);

	printmatrix(noize_filter,nullptr);

	cv::imshow("Result", grany_copy);
	cv::imshow("Result_old", grany);
}
//-----------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------
//функция для ползунка размытия методом Гаусса 
void GaussinBlurTrackbar(int, void*) {

	// размытие Гаусса
	cv::GaussianBlur(src_grey, dst, cv::Size(abs(2 * karnelsize - 1), abs(2 * karnelsize - 1)), 0, 0);

	//детектор границ
	cv::Canny(dst, grany, canny_theshold_min, canny_theshold_max);

	printmatrix(noize_filter, nullptr);

	//вывод изображения после размытия и детектора границ
	cv::imshow("Result", grany_copy);
	cv::imshow("Result_old", grany);

}
//-----------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------
//функция для вывода созданного нами изображения в консоль в виде бинарной матрицы
void printmatrix(int, void*) {
// создаем ещё одну матрицу в которую копируем матрицу после детектора границ 
//и затем пробегаем по ней подкручивая значения до 255(?)
	grany_copy = grany.clone();
	for (int i = 0; i < grany_copy.rows; ++i)
		for (int j = 0; j <grany_copy.cols; ++j)
			if (grany_copy.at<uchar>(i, j) > 0)
				grany_copy.at<uchar>(i, j) = 255;

	std::vector<std::list<Coords>> Coords_List;
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

	for (std::vector<std::list<Coords>>::iterator uter = Coords_List.begin(); uter != Coords_List.end(); ++uter) {
		std::list<Coords> Vnutr_list = *uter;
		if (Vnutr_list.size() > noize_filter){
			for (std::list<Coords>::iterator iter = Vnutr_list.begin(); iter != Vnutr_list.end(); ++iter) {
				Coords Coordinats = *iter;
				grany_copy.at<uchar>(Coordinats.y, Coordinats.x) = 255;
			}
		}
	}

	// размытие Гаусса
	cv::GaussianBlur(src_grey, dst, cv::Size(abs(2 * karnelsize - 1), abs(2 * karnelsize - 1)), 0, 0);

	//детектор границ
	cv::Canny(dst, grany, canny_theshold_min, canny_theshold_max);

	cv::imshow("Result", grany_copy);
	cv::imshow("Result_old", grany);
}
//-----------------------------------------------------------------------------------------------------------



int main(int argc, char** argv)
{
	char filename[] = "1.jpg";
	//char filename[] = "Big.png";

	src = cv::imread(filename, 1);// загрузка в матрицу изображения
								  //>0 - трехканальное, <0 - исходное кол-во каналов, =0 - оттенки серого


	if (!src.data)
	{
		return -1;
	}

	//Создание окон для вывода
	cv::namedWindow("Before", CV_WINDOW_FREERATIO);
	cv::namedWindow("Result", CV_WINDOW_FREERATIO);
	cv::namedWindow("Tools", CV_WINDOW_NORMAL);

	//преобразование избражения в оттенки серого
	cv::cvtColor(src, src_grey, CV_BGR2GRAY); // в opencv не RGB, а BGR

	std::cout << src_grey.cols << "x" << src_grey.rows << std::endl; //выводим начальный размер

	if ((src.cols >= 600) || (src.rows >= 600)) {
		int width, height;
		width = src_grey.cols;
		height = src_grey.rows;

		double cofx = width / 600;
		double cofy = height / 600;
		double cof = 1 / (((cofx) > (cofy)) ? (cofx) : (cofy));
		std::cout << "cof" << cof << std::endl;

		cv::resize(src_grey, src_grey, cv::Size(), cof, cof);
		std::cout << src_grey.cols << "x" << src_grey.rows << std::endl;// вывод нового размера
	}

	int width = src_grey.cols;
	int height = src_grey.rows;

	cvResizeWindow("Before", width, height);
	cvResizeWindow("Result", width, height);
	cvResizeWindow("Tools", 400, 150);

	cv::moveWindow("Before", 50, 50);
	cv::moveWindow("Result", width + 50, 50);
	cv::moveWindow("Tools", width * 2 + 50, 50);


	cv::GaussianBlur(src_grey, dst, cv::Size(1, 1), 0, 0);
	cv::Canny(dst, grany, 0, 0);

	cv::imshow("Before", src_grey);
	cv::imshow("Result", grany);

	//создание ползунков
	cv::createTrackbar("Blur", "Tools", &karnelsize, 51, GaussinBlurTrackbar);
	cv::createTrackbar("Canny min", "Tools", &canny_theshold_min, 100, CannyThreshold);
	cv::createTrackbar("Canny max", "Tools", &canny_theshold_max, 300, CannyThreshold);
	cv::createTrackbar("Noize filter", "Tools", &noize_filter, 300,printmatrix);
	//ожидание нажатия клавиши
	cv::waitKey(0);


	return 0;
}