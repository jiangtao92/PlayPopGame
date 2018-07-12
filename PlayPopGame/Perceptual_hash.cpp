#include "stdafx.h"
#include "Perceptual_hash.h"

#include "config.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <atlimage.h>
#include <windows.h>

#include <iostream>

using namespace cv;
using namespace std;

static LPCTSTR programTitle = _T("�Զ�����ɫ��");

// ʹ�� ��֪��ϣ�㷨Ѱ����ͬ�ķ���
//////////////////////////////////////////////////////////////////////////
Vec3b calAvgColor(Mat & img)
{
	if (img.rows * img.cols == 0)
		return 0;

	int a, b, c;
	a = b = c = 0;

	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			a += img.at<Vec3b>(row, col)[0];
			b += img.at<Vec3b>(row, col)[1];
			c += img.at<Vec3b>(row, col)[2];
		}
	}
	Vec3b avg{ 0,0,0 };
	avg[0] = a / (img.rows * img.cols);
	avg[1] = b / (img.rows * img.cols);
	avg[2] = c / (img.rows * img.cols);

	return avg;
}

bool isSimilarColor(Vec3b c1, Vec3b c2)
{
	const static int diff = 15;
	int dx = c1[0] - c2[0];
	int dy = c1[1] - c2[1];
	int dz = c1[2] - c2[2];

	if (abs(dx) <= diff \
		&& abs(dy) <= diff \
		&& abs(dz) <= diff)
	{
		return true;
	}
	return false;
}

int calAvg(Mat & img)
{
	if (img.rows * img.cols == 0)
		return 0;

	int avg = 0;
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			avg += img.at<uchar>(row, col);
		}
	}
	return avg / (img.rows * img.cols);
}

uint64 calHash(Mat & img, int avg)
{
	uint64 ret = 0;
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			if (img.at<uchar>(row, col) >= avg)
			{
				ret |= 1i64 << (row * 8 + col);
			}
		}
	}
	return ret;
}

int haming(uint64 d1, uint64 d2)
{
	int h = 0;
	uint64 d = d1 ^ d2;

	while (d) {
		h++;
		d &= d - 1;
	}

	return h;
}

void CImageToMat(CImage& cimage, Mat& mat)
{
	if (cimage.IsNull())
	{
		return;
	}


	int nChannels = cimage.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = cimage.GetWidth();
	int nHeight = cimage.GetHeight();


	//�ؽ�mat  
	if (1 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC3);
	}

	//��������  
	uchar* pucRow;                                  //ָ������������ָ��  
	uchar* pucImage = (uchar*)cimage.GetBits();     //ָ����������ָ��  
	int nStep = cimage.GetPitch();                  //ÿ�е��ֽ���,ע���������ֵ�����и�  


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * nChannels + nCha] = *(pucImage + nRow * nStep + nCol * nChannels + nCha);
				}
			}
		}
	}
}

bool calGameBoard(Mat &img, std::vector<std::vector<int>> &gameBoard)
{
	if (img.empty()) {
		cerr << "img empty" << endl;
		return false;
	}

	vector<Vec3b> containerHashCode;

	gameBoard = std::vector<std::vector<int> >(Rows, std::vector<int>(Colunms, 0));

	for (int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Colunms; j++)
		{
			// 1. ���ŵ� 8 * 8 ��С
			// todo: hard code ȥ������Ҫ�ĵط�
			Mat s;
			resize(img(Rect{ StartPosX + (ItemWidth * j) + 5, StartPosY + (ItemHeight * i) + 5, ItemWidth - 5, ItemHeight - 31}),
				s,
				Size{ 8, 8 });

			// 2.1 ��ȡƽ����ɫֵ (TODO: ��ȡͼƬ����Ҫ��ɫ�㷨��Ҫ�о�һ��)
			Vec3b avgColor = calAvgColor(s);
			//// 2. ����ɻҶ�
			//Mat gray_s;
			//cvtColor(s, gray_s, CV_RGB2GRAY);
			//// 3. ͳ�ƻҶ�ƽ��ֵ
			//int avg = calAvg(gray_s);
			//// 4. ��������һ������ֵ
			//uint64 hashCode = calHash(gray_s, avg);

			auto pos = std::find_if(begin(containerHashCode), end(containerHashCode), [avgColor](Vec3b & value) {
					if (isSimilarColor(avgColor, value)) { // ��ɫ��ͬ
						return true;
					}
				return false;
			});

			if (pos == end(containerHashCode)) {
				containerHashCode.push_back(avgColor);
				gameBoard[i][j] = containerHashCode.size() - 1;
			}
			else {
				gameBoard[i][j] = std::distance(begin(containerHashCode), pos);
			}
		}
	}

	return true;
}

bool PerceptualHash::getGameBoard(std::vector<std::vector<int>> &gameBoard)
{
	HWND hwnd = FindWindow(NULL, programTitle);
	if (NULL == hwnd) {
		_tprintf(_T("FindWindow failed, error code %d"), GetLastError());
		return false;
	}

	RECT rect;
	GetWindowRect(hwnd, &rect);
	int w = rect.right - rect.left, h = rect.bottom - rect.top;
	//_tprintf(_T("width = %d, height = %d\n"), w, h);

	// ��ȡ���ڵ��豸�����ģ�Device Contexts��
	HDC hdcWindow = GetDC(hwnd); // Ҫ��ͼ�Ĵ��ھ����Ϊ����ȫ��
								 // ��ȡ�豸�����Ϣ�ĳߴ��С
								 //int nBitPerPixel = GetDeviceCaps(hdcWindow, BITSPIXEL);
	int nBitPerPixel = 24; // 3 ͨ����ɫ
						   // ����Ϸ���ڽ�ͼ
	CImage image;
	image.Create(w, h, nBitPerPixel);
	BitBlt(
		image.GetDC(),			// ���浽��Ŀ�� ͼƬ���� ������
		0, 0,					// ��ʼ x, y ����
		w, h,					// ��ͼ���
		hdcWindow,				// ��ȡ����� �����ľ��
		0, 0,					// ָ��Դ�����������Ͻǵ� X, y �߼�����
		SRCCOPY);
	image.ReleaseDC();

	// convert to Mat struct
	Mat img;
	CImageToMat(image, img);
	imwrite("./ScreenShot.bmp", img);

	//Mat img;
	//img = imread("ScreenShot.bmp", IMREAD_UNCHANGED);

	// ��ȡ��Ϸ����
	return calGameBoard(img, gameBoard);
}