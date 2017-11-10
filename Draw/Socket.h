#pragma once
// ������ ���������� TCP �������

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>	//���������� ��� ������ � �������
#include <queue>	//���������� ��� ��������� �������
#include <ctime>
#include <cstdlib>

//#include "Header.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define MATHPI 3.141592653589793238462643383279502 
#define DEFAULT_BUFLEN 36
#define SENDBUF 36 //����� ������ ��� ��������


struct currentCoord {
	int count = 0;
	int time = 0;
};


//������� ��������:
//=======================================================================================================================
inline int sendany(SOCKET my_sock, char str[]) {
	int iResult = send(my_sock, str, (int)strlen(str), 0); //�������� ������ �� ������
														   //std::cout << "Bytes sent: " << iResult << std::endl;
	if (iResult == SOCKET_ERROR) { //���� ���-�� ����� �� ���, ��
								   //std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;//����� �� ������,
		closesocket(my_sock);//��������� �����
							 //WSACleanup();//� ��������� �� �� �����
		return 1;
	}
	return 0;
}
//=======================================================================================================================
//������� ������:
//=======================================================================================================================
inline int recvspam(SOCKET sockrecv, char recvbuf[DEFAULT_BUFLEN], int recvbuflen, currentCoord inRobot, int numOfCoord, int currentTimeDrawing) {

	struct timeval timeout;
	timeout.tv_sec = 100;
	timeout.tv_usec = 0;
	setsockopt(sockrecv, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //��� unix-������ ������ (char*)


	int sumbytes = 0;
	int x, y, z, w, p, r, seg;
	int tmp;


	int iResult = recv(sockrecv, recvbuf, recvbuflen, 0);

	inRobot.time = clock() - currentTimeDrawing; //������ ����� ��������� �����

	if (iResult == -1) {
		if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
			return -15;
		}

	}
	if (iResult > 0) {
		sumbytes += iResult;
		int count = 0;
		inRobot.count = numOfCoord; //���������� ����� ������� ����� � ���������
		inRobot.time = clock() - currentTimeDrawing; //������ ����� ��������� �����
													 ////std::cout << "Num of point: " << inRobot.count << " Drawing time:" << inRobot.time << " ";
		for (int i = 0; i < iResult; ++i) {

			if (recvbuf[i] == ' ') {
				++count;
				tmp = atoi(&recvbuf[i]);
				switch (count % 8) {
				case 1: {
					x = tmp;
					////std::cout << "x: " << x << ' ';
					break;
				}
				case 2: {
					y = tmp;
					////std::cout << "y: " << y << ' ';
					break;
				}
				case 3: {
					z = tmp;
					////std::cout << "z: " << z << ' ';
					break;
				}
				case 4: {
					w = tmp;
					////std::cout << "w: " << w << ' ';
					break;
				}
				case 5: {
					p = tmp;
					////std::cout << "p: " << p << ' ';
					break;
				}
				case 6: {
					r = tmp;
					////std::cout << "r: " << r << ' ';
					break;
				}
				case 7: {
					seg = tmp;
					////std::cout << "segtime: " << seg << ' ';
					break;
				}
				default: {
					break;
				}
				}
			}
		}
		//std::cout << std::endl;
	}
	else if (iResult == 0) {
		//std::cout << "Connection closed" << std::endl;
		return 1;
	}
	else {
		//std::cout << "Recv failed with error: " << WSAGetLastError() << std::endl;
		return 1;
	}

	//std::cout << "Bytes received: " << sumbytes << std::endl;
	//std::cout << "_____________________________________________" << std::endl;
	return 0;
}
//=======================================================================================================================
//������� ����������� � ������
//=======================================================================================================================
inline int conSocket(SOCKET my_sock, SOCKET sockrecv, const char* serveraddr, int ports, int portr) {
	// ���������� ��������� sockaddr_in
	// �������� ������ � ����� �������

	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(ports);
	dest_addr.sin_addr.s_addr = inet_addr(serveraddr);
	// ����� ������� ������� � �������� ���������� ���������� 
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr))) {
		//std::cout << "Connect error " << WSAGetLastError() << std::endl;
		//system("pause");
		return -2;
	}
	else {

		//std::cout << "Connection with robot via IP adress: " << serveraddr << " installed sucsessfully" << std::endl;
	}
	//-------------------------------------------------------------------------
	//���������� � ������� ��� ��Ȩ��
	//-------------------------------------------------------------------------
	sockaddr_in recv_addr;
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_port = htons(portr);
	recv_addr.sin_addr.s_addr = inet_addr(serveraddr);
	// ����� ������� ������� � �������� ���������� ����������
	if (connect(sockrecv, (sockaddr *)&recv_addr, sizeof(recv_addr))) {
		//std::cout << "Connect error " << WSAGetLastError() << std::endl;
		//system("pause");
		return -3;
	}
	else {
		//std::cout << "Connection with robot TO RECV via IP adress: " << serveraddr << " and port: " << portr << " installed sucsessfully" << std::endl;
	}
	return 0;
}
//=======================================================================================================================



int socketmain(char fname[1024])
{
	setlocale(LC_CTYPE, "Russian");

	//==================================================================================
	int numOfCoord = 0;

	std::ifstream inFile(fname);//�������� �� ���������� �������� �����
	if (!inFile) {
		//std::cout << "Can not open file " << fname << std::endl;
		return 1001;
	}
	//----------------------------------------------------------------------------------
	char sets[] = "sets.txt";
	std::ifstream settings(sets);//�������� �� ���������� �������� �����
	if (!settings) {
		//std::cout << "Can not open file " << sets << std::endl;
		return 1002;
	}
	//----------------------------------------------------------------------------------
	int desk_ax, desk_ay, desk_az, desk_bx, desk_by, desk_bz, desk_cx, desk_cy, desk_cz;
	int segtime;//����� ����������� �� ����� ����� � ������ � ��
	int syscoord, toolcoord;//����������, ���������� �� ����� ������� ���������

							//�������� ���������� ���� ����� �� �����
	settings >> desk_ax >> desk_ay >> desk_az >> desk_bx >> desk_by >> desk_bz >> desk_cx >> desk_cy >> desk_cz;
	//������� ��� ������ � ����������, ������������ ����� �� ���� ����
	//-------------------------------------------------
	/*if ((desk_ax == desk_bx) && (desk_bx == desk_cx)) {
	++desk_ax;
	}
	if ((desk_ay == desk_by) && (desk_by == desk_cy)) {
	++desk_ay;
	}
	if ((desk_az == desk_bz) && (desk_bz == desk_cz)) {
	++desk_az;
	}*/
	//-------------------------------------------------
	int ports, portr; //�������������� ��� ����������, ���� ���������� ����� ��� �������� � ��������
	std::string tmp_addr_string;//�������������� ������, ���� ������� IP �������
	settings >> tmp_addr_string; //��������� IP �������
	const char *serveraddr = tmp_addr_string.c_str(); //��������� ��� �� string � const char
													  //std::cout << serveraddr << std::endl;
	settings >> ports >> portr >> segtime >> syscoord >> toolcoord; //��������� �� ����� ������ ������, ����� �������� � ������� ���������
																	//==================================================================================

																	// ��� 1 - ������������� ���������� Winsock
	char buff[1024];
	//std::cout << "Socket client over TCP/IP" << std::endl;

	if (WSAStartup(0x202, (WSADATA *)&buff[0])) //�������� ��������� ���������� ������ � ��������, ������������� WinsockAPI
	{
		//std::cout << "WSAStart error %d" << WSAGetLastError() << std::endl;
		return -1;
	}

	// ��� 2 - �������� ������ ��� �������� ���������
	//-------------------------------------------------------------------------
	SOCKET my_sock; //������ �����
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		//std::cout << "Socket() error " << WSAGetLastError() << std::endl;

		return -1;
	}
	//-------------------------------------------------------------------------
	//�������� ������ ��� ��Ȩ��
	//-------------------------------------------------------------------------
	SOCKET sockrecv; //������ �����
	sockrecv = socket(AF_INET, SOCK_STREAM, 0);
	if (sockrecv < 0)
	{
		//std::cout << "Socket() error " << WSAGetLastError() << std::endl;
		return -2;
	}
	//-------------------------------------------------------------------------

	// ��� 3 - ��������� ���������� ��� �������� ���������
	//-------------------------------------------------------------------------
	conSocket(my_sock, sockrecv, serveraddr, ports, portr);
	//-------------------------------------------------------------------------
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	std::queue<int> resbuf;     // ������� ������ ������� ���� int
								//int sockerr;//���� ������ ������, ���� �� ������������
	char sendbuf[SENDBUF];//����� ��� ��������

						  //����� ������� ���������
						  //=======================================================================================================================
						  /*
						  bool ex = true;
						  std::cout << "Select coordinate system:" << std::endl << "0 = JOINT | 1 = JOGFRAME | 2 = WORLDFRAME | 3 = TOOLFRAME | 4 = USER FRAME" << std::endl << "Enter num of coordinate system: ";
						  while (ex) {
						  try {
						  std::cin >> send1;//��������� ���������� � send1
						  char *sendbuf = &send1[0u]; //������� �����
						  for (int i = 0; i < (strlen(sendbuf)); ++i) { //�������� �� ��������
						  char x = sendbuf[i];
						  if (x >= '0' &&  x <= '4') {
						  send(my_sock, sendbuf, (int)strlen(sendbuf), 0); //��������� ���� ����� ������ ��������
						  sendbuf[0] = ' '; //������ ������ � ������ ������ �� ������
						  send(my_sock, sendbuf, 1, 0); //���������� ������
						  ex = false;
						  }
						  else {
						  throw "Invalid symbol";
						  }
						  }
						  }
						  catch (...) {
						  std::cout << "Please, input only allowed values" << std::endl << "Select coordinate system:";
						  }
						  }
						  */
	char coord[2];
	sprintf_s(coord, "%d", syscoord);
	sendany(my_sock, coord);//���������� �����

							//=======================================================================================================================

	int image_ax, image_ay, image_bx, image_by, image_cx, image_cy; //���������� ������� ����� ��� �������� �������� �� �����

	int image_width = 0, image_high = 0;//������ � ������ ��������

	int center_x, center_y, center_z; //���������� ������ �����/�����������

									  //����������� ���������� ������ �����:
	center_x = (desk_ax + desk_cx) / 2;
	center_y = (desk_ay + desk_cy) / 2;
	center_z = (desk_az + desk_cz) / 2;

	settings.close();
	inFile >> image_width >> image_high;//��������� ������ � ������ ��������
										//std::cout << image_width << " " << image_high << std::endl;
										//std::cout << "_____________________________________________" << std::endl;
										//��������� �������� 3� ������� �����:
	image_ax = center_x + (int)(image_width / 2);
	image_ay = center_y + (int)(image_high / 2);
	image_bx = center_x + (int)(image_width / 2);
	image_by = center_y - (int)(image_high / 2);
	image_cx = center_x - (int)(image_width / 2);
	image_cy = center_y - (int)(image_high / 2);

	int image_x, image_y;//X � Y ����� �� ��������
	int Xr, Yr;//X � Y ����� ��� ������
			   //================================================================================================
			   /*
			   Q,W  - ������� �������� ���������
			   Q1,Q2,Q3 - ���������� ���������� �� ���������� �������
			   W1,W2,W3 - ���������� ���������� �� ���������� �������

			   n - ������ �������
			   nx,ny,nz - ���������� �������

			   */
	double Q1 = desk_ax - desk_bx;
	double Q2 = desk_ay - desk_by;
	double Q3 = desk_az - desk_bz;
	double W1 = desk_cx - desk_bx;
	double W2 = desk_cy - desk_by;
	double W3 = desk_cz - desk_bz;
	double nx = Q2 * W3 - Q3 * W2;//������� ������� � ���������
	nx = -nx;
	double ny = Q3 * W1 - Q1 * W3;
	ny = -ny;
	double nz = Q1 * W2 - Q2 * W1;
	nz = -nz;
	int w = -(int)((acos(nz / sqrt(nz * nz + ny * ny))) * 180.0 / MATHPI);
	int pp = 180 - (int)((acos(nz / sqrt(nz * nz + nx * nx))) * 180.0 / MATHPI);

	int UW = w;
	int UP = pp;
	int uz = 0;

	double A = desk_ay * (desk_bz - desk_cz) + desk_by * (desk_cz - desk_az) + desk_cy * (desk_az - desk_bz);
	double B = desk_az * (desk_bx - desk_cx) + desk_bz * (desk_cx - desk_ax) + desk_cz * (desk_ax - desk_bx);
	double C = desk_ax * (desk_by - desk_cy) + desk_bx * (desk_cy - desk_ay) + desk_cx * (desk_ay - desk_by);
	double D = desk_ax * (desk_by * desk_cz - desk_cy * desk_bz) + desk_bx * (desk_cy * desk_az - desk_ay * desk_cz) + desk_cx * (desk_ay * desk_bz - desk_by * desk_az);
	double Z;

	double mod = sqrt(nx * nx + ny * ny + nz * nz);//����� ������� �������

	int dx = (int)(nx / mod * 20);//��������
	int dy = (int)(ny / mod * 20);
	int dz = (int)(nz / mod * 20);

	//--------------------------------------
	int startTimeDrawing = clock();
	//int currentTimeDrawing = clock();
	currentCoord inBuf;
	currentCoord inRobot;
	//=======================================================================================================================
	while (inFile.eof() == false) {

		for (int i = 0; (inFile.get() != '|' && inFile.eof() == false); ++i) {
			inFile >> image_x >> image_y; //���� ������� x � y � �������� �� � � ���������� ��� ����������� �������.
										  //std::cout << image_x << " " << image_y << " | ";
			int tempr_x, tempr_y;
			tempr_x = image_width - image_x;//����������� ��������� ��������� �� ����� �� 0 � ����� �������
			tempr_y = image_high - image_y;//����������� ��������� ��������� �� ����� �� 0 � ����� �������
										   //--------------------------------------
			std::swap(tempr_x, tempr_y);
			Xr = (int)((tempr_x + image_cx)*(nz / sqrt(nz * nz + ny * ny)));//���������� � ������������ ����������� �� x ��������� �������� ��� ����, ����� ������� ��� �� �����,
																			// � �� ����� ������ ������, �.�. x=0 � ���������
			Yr = (int)((tempr_y + image_cy)*(nz / sqrt(nz * nz + nx * nx)));//�������� �� ������������ ��������� �� y ��������� �������� ��� ����, ����� ������� ��� ��-��������, � �� ������
																			//� ������� ������������� �������� y
																			//--------------------------------------
			if (C == 0)
			{
				Z = (int)desk_bz;
			}
			else {
				Z = (int)(-((A * Xr) + (B * Yr) - D) / C);
			}
			//-------------------------------------------------------------------------------------------

			//std::cout << Xr << " " << Yr << " " << Z << " " << UW << " " << UP << " " << uz << " | ";

			if (i == 0) {	//���� ������ ����� �������, �� ������������� ��������� ���������� ��� �������� �������;
				int T = 1;//���� ��� ����� ���� ��������
				resbuf.push(Xr + dx);
				resbuf.push(Yr + dy);
				resbuf.push((int)Z + dz);
				resbuf.push(UW);
				resbuf.push(UP);
				resbuf.push(uz);
				resbuf.push(segtime);
				resbuf.push(T);
				resbuf.push(0);
			}
			//-------------------------------------------------------------------------------------------
			int T = 0;//���� ��� ����� ���� ��������
			resbuf.push(Xr);
			resbuf.push(Yr);
			resbuf.push((int)Z);
			resbuf.push(UW);
			resbuf.push(UP);
			resbuf.push(uz);
			resbuf.push(segtime);
			resbuf.push(T);
			resbuf.push(0);
			//-------------------------------------------------------------------------------------------
			if (inFile.peek() == '|') {	//���� ��������� ����� ������� (������ ���� �����������),
										//�� ������������� ��������� ���������� ��� �������� �������;
				int T = 1;//���� ��� ����� ���� ��������
				resbuf.push(Xr + dx);
				resbuf.push(Yr + dy);
				resbuf.push((int)Z + dz);
				resbuf.push(UW);
				resbuf.push(UP);
				resbuf.push(uz);
				resbuf.push(segtime);
				resbuf.push(T);
				resbuf.push(0);
			}
			//-------------------------------------------------------------------------------------------

			for (size_t j = 0; j < resbuf.size(); ++j) {
				int tmpX = resbuf.front();
				resbuf.pop();
				int tmpY = resbuf.front();
				resbuf.pop();
				int tmpZ = resbuf.front();
				resbuf.pop();
				int tmpW = resbuf.front();
				resbuf.pop();
				int tmpP = resbuf.front();
				resbuf.pop();
				int tmpR = resbuf.front();
				resbuf.pop();
				int tmpSeg = resbuf.front();
				resbuf.pop();
				int tmpType = resbuf.front();
				resbuf.pop();
				int tmpT = resbuf.front();
				resbuf.pop();
				sprintf_s(sendbuf, "%d %d %d %d %d %d %d %d %d", tmpX, tmpY, tmpZ, tmpW, tmpP, tmpR, tmpSeg, tmpType, tmpT);
				sendany(my_sock, sendbuf);//���������� �����
										  //-------------------------------------------------------------------------

										  //currentTimeDrawing = clock();
				++numOfCoord;
				if (recvspam(sockrecv, recvbuf, recvbuflen, inRobot, numOfCoord, startTimeDrawing) == -15) {
					//closesocket(my_sock); //1!
					//closesocket(sockrecv);//2!
					//WSACleanup();			//3!

					while (recvspam(sockrecv, recvbuf, recvbuflen, inRobot, numOfCoord, startTimeDrawing) == -15) {//���������� ���� �� ����� ������������� ������ � �������� �� �������
																												   //���� ������ ���������� ������ �� ����� ��������� � ��������� ����� � ����
																												   //���� ��� ����� ������ � ��������/�������� ������ � ������� (1-5!)

																												   //std::cout << "Lost connection, try to reconnect.." << std::endl;
																												   //sockerr = conSocket(my_sock, sockrecv, serveraddr, ports, portr);//5!

					}
				}

				//-------------------------------------------------------------------------------------------			
			} //�������� for'a ��������
			Sleep(segtime);
		}//�������� for'a ������ �������
	}//�������� �������� while'a
	 //-------------------------------------------------------------------------
	inFile.close();	//��������� ����, ������ ����� �����, � ���������� ��������� ��� ���������� ������ (����������� �������� = 1)

	int T = 0;//���� ��� ����� ���� ��������
	resbuf.push(Xr + dx);
	resbuf.push(Yr + dy);
	resbuf.push((int)Z + dz);
	resbuf.push(UW);
	resbuf.push(UP);
	resbuf.push(uz);
	resbuf.push(T);
	resbuf.push(segtime);
	resbuf.push(1);

	for (size_t j = 0; j < resbuf.size(); ++j) {
		int tmpX = resbuf.front();
		resbuf.pop();
		int tmpY = resbuf.front();
		resbuf.pop();
		int tmpZ = resbuf.front();
		resbuf.pop();
		int tmpW = resbuf.front();
		resbuf.pop();
		int tmpP = resbuf.front();
		resbuf.pop();
		int tmpR = resbuf.front();
		resbuf.pop();
		int tmpSeg = resbuf.front();
		resbuf.pop();
		int tmpType = resbuf.front();
		resbuf.pop();
		int tmpT = resbuf.front();
		resbuf.pop();
		sprintf_s(sendbuf, "%d %d %d %d %d %d %d %d %d", tmpX, tmpY, tmpZ, tmpW, tmpP, tmpR, tmpSeg, tmpType, tmpT);
		sendany(my_sock, sendbuf);//���������� �����
	}
	//-------------------------------------------------------------------------
	if (recvspam(sockrecv, recvbuf, recvbuflen, inRobot, numOfCoord, startTimeDrawing) == -15) {

		//closesocket(my_sock); //1!
		//closesocket(sockrecv);//2!
		//WSACleanup();			//3!

		while (recvspam(sockrecv, recvbuf, recvbuflen, inRobot, numOfCoord, startTimeDrawing) == -15) {//���������� ���� �� ����� ������������� ������ � �������� �� �������
																									   //���� ������ ���������� ������ �� ����� ��������� � ��������� ����� � ����
																									   //���� ��� ����� ������ � ��������/�������� ������ � ������� (1-5!)

																									   //std::cout << "Lost connection, try to reconnect.." << std::endl;
																									   //sockerr = conSocket(my_sock, sockrecv, serveraddr, ports, portr);//5!

		}
	}
	startTimeDrawing = clock() - startTimeDrawing;
	//std::cout << "Clear time of drawing: " << startTimeDrawing / 1000 << "," << startTimeDrawing % 1000 << " seconds" << std::endl;;
	// shutdown the connection since no more data will be sent
	int iResult;
	if ((iResult = shutdown(my_sock, SD_SEND)) == SOCKET_ERROR) {
		printf("Shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		closesocket(sockrecv);
		WSACleanup();
		return 1;
	}
	//=======================================================================================================================
	//closesocket(my_sock);
	//closesocket(sockrecv);
	WSACleanup();
	return 0;
}

