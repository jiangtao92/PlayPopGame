#include "stdafx.h"
#include "config.h"
#include "CalPopGame.h"
#include <queue>

#include <iostream>
using namespace std;

const static int MoveMent[2][2] = { {1, 0}, {0, 1} };

CalPopGame::CalPopGame(const std::vector<std::vector<int>>& board, int rows, int column, int timeout)
	: board_(board)
	, rows_(rows)
	, colums_(column)
{

}

CalPopGame::~CalPopGame() {
}

bool CalPopGame::cal(std::vector<std::pair<jpp::Point, jpp::Point>> &ret)
{
	int maxCount = -1;
	pair<jpp::Point, jpp::Point> maxPoint;

	for (int i = rows_ - 1; i >= 0; i--)
	{
		for (int j = 0; j < colums_; j++) {

			// 已经被消除掉,就尝试其他
			if (board_[i][j] == -1)
				continue;

			for (int k = 0; k < sizeof(MoveMent) / sizeof(MoveMent[0]); k++) {

				int tmpx = i + MoveMent[k][0];
				int tmpy = j + MoveMent[k][1];

				if (tmpx >= rows_ || tmpy >= colums_)
					continue;
				if (board_[i][j] == board_[tmpx][tmpy])
					continue;

				std::swap(board_[i][j], board_[tmpx][tmpy]);

				int count = 0;
				if (k == 0) {
					count += deleteRow(j);
					count += deleteColum(i);
					count += deleteColum(tmpx);
				}
				else {
					count += deleteColum(i);
					count += deleteRow(j);
					count += deleteRow(tmpy);
				}

				//cout << "count " << count << endl;

				if (count > maxCount) {
					maxPoint = make_pair(jpp::Point{ i, j }, jpp::Point{ tmpx, tmpy });
					maxCount = count;
				}
				//ret.push_back(make_pair(jpp::Point{ i, j }, jpp::Point{ tmpx, tmpy }));
				// 还原
				std::swap(board_[i][j], board_[tmpx][tmpy]);
			}
		}
	}

	// 将最大值放入其中
	if (maxCount > 0) {
		ret.push_back(maxPoint);
	}

	//cout << maxCount << endl;

	return !ret.empty();
}

// 只要查找第一个点然后返回就可以了
bool CalPopGame::isOK(int xpos, int ypos)
{
	// 水平方向
	int count = 1;
	for (int i = 1; i < colums_; i++)
	{
		if (board_[xpos][i - 1] != -1 && board_[xpos][i - 1] == board_[xpos][i])
			count++;
		else
			count = 1;
		if (count >= 3)
			return true;
	}
	// 垂直方向
	count = 1;
	for (int i = 1; i < rows_; i++)
	{
		if (board_[i - 1][ypos] != -1 && board_[i - 1][ypos] == board_[i][ypos])
			count++;
		else
			count = 1;
		if (count >= 3)
			return true;
	}
	return false;
}

int CalPopGame::deleteOK(int xpos, int ypos)
{
	int ret = 0;

	// 水平方向
	int index = 0;
	for (int i = 1; i < colums_; i++)
	{
		if (board_[xpos][i] == -1)
		{
			index = i;
		}
		else if (board_[xpos][i] != board_[xpos][index])
		{
			if (i - index >= 3) {
				//while (index < i) {
				//	board_[xpos][index++] = -1;
				//}
				ret += i - index;
			}
			index = i;
		}
	}

	if (colums_ - index >= 3) {
		//while (index < colums_) {
		//	board_[xpos][index++] = -1;
		//}
		ret += colums_ - index;
	}


	// 垂直方向
	index = 0;
	for (int i = 1; i < rows_; i++)
	{
		if (board_[i][ypos] == -1)
		{
			index = i;
		}
		else if (board_[i][ypos] != board_[index][ypos])
		{
			if (i - index >= 3) {
				//while (index < i) {
				//	board_[index++][ypos] = -1;
				//}
				ret += i - index;
			}
			index = i;
		}
	}

	if (rows_ - index >= 3) {
		//while (index < rows_) {
		//	board_[index++][ypos] = -1;
		//}
		ret += rows_ - index;
	}

	return ret;
}

int CalPopGame::deleteRow(int ypos)
{
	int ret = 0;
	// 垂直方向
	int index = 0;
	for (int i = 1; i < rows_; i++)
	{
		if (board_[i][ypos] == -1)
		{
			index = i;
		}
		else if (board_[i][ypos] != board_[index][ypos])
		{
			if (i - index >= 3) {
				//while (index < i) {
				//	board_[index++][ypos] = -1;
				//}
				ret += i - index;
			}
			index = i;
		}
	}

	if (rows_ - index >= 3) {
		//while (index < rows_) {
		//	board_[index++][ypos] = -1;
		//}
		ret += rows_ - index;
	}

	return ret;
}

int CalPopGame::deleteColum(int xpos)
{
	int ret = 0;

	// 水平方向
	int index = 0;
	for (int i = 1; i < colums_; i++)
	{
		if (board_[xpos][i] == -1)
		{
			index = i;
		}
		else if (board_[xpos][i] != board_[xpos][index])
		{
			if (i - index >= 3) {
				//while (index < i) {
				//	board_[xpos][index++] = -1;
				//}
				ret += i - index;
			}
			index = i;
		}
	}

	if (colums_ - index >= 3) {
		//while (index < colums_) {
		//	board_[xpos][index++] = -1;
		//}
		ret += colums_ - index;
	}

	return ret;
}