#ifndef __CALLINKGAME_H_
#define __CALLINKGAME_H_

#include <vector>
#include "CMouseControl.h"

// todo: 实现从index想坐标点的转换

namespace jpp {
	struct Point {
		Point(int x = 0, int y = 0) : x_(x), y_(y) {};
		int x_;
		int y_;
	};
}

class CalPopGame {
public:
	// TODO: 参数校验
	CalPopGame(const std::vector<std::vector<int>>& board, int rows, int column, int timeout = 0);
	
	~CalPopGame();

	//bool cal(std::pair<jpp::Point, jpp::Point> &pos);

	bool cal(std::vector<std::pair<jpp::Point, jpp::Point>> &ret);

private:
	bool isOK(int i, int j);

	// 搜索第i行,第j列

	// 返回可以消除掉方块数量
	int deleteOK(int i, int j);

	int deleteRow(int i);

	int deleteColum(int i);

private:
	std::vector<std::vector<int>> board_;
	int rows_;
	int colums_;
	//MouseControl mouseControl_;
};

#endif
