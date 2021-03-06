// PlayPopGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Perceptual_hash.h"
#include "CalPopGame.h"
#include "CMouseControl.h"
#include "config.h"

#include <vector>
#include <iostream>
#include <thread>

using namespace std;

static LPCTSTR programTitle = _T("对对碰角色版");

int main()
{
	vector<vector<int>> gameBoard;

	MouseControl mouseControl;

	if (!mouseControl.Init(programTitle)) {
		cout << "mouseControl.Init()" << endl;
		return 0;
	}

	while (true) {
		if (!PerceptualHash::getGameBoard(gameBoard)) {
			cout << "PerceptualHash::getGameBoard failed!" << endl;
			break;
		}

		//display
		for (int i = 0; i < Rows; i++)
		{
			for (int j = 0; j < Colunms; j++)
			{
				_tprintf(_T("%02d,"), gameBoard[i][j]);
			}
			_tprintf(_T("\n"));
		}

		CalPopGame calPopGame(gameBoard, Rows, Colunms);
		std::vector<std::pair<jpp::Point, jpp::Point>> ret;
		if (!calPopGame.cal(ret)) {
			cout << "calPopGame.cal failed, wait a while..." << endl;
			this_thread::sleep_for(chrono::microseconds(1500));

			continue;
		}

		for (auto pos = ret.begin(); pos != end(ret); ++pos) {

			cout << "( " << pos->first.x_ << ", " << pos->first.y_ << " ) <-> ( " << pos->second.x_ << ", " << pos->second.y_ << " )" << endl;

			mouseControl.Clicked(pos->first.x_, pos->first.y_);

			mouseControl.Clicked(pos->second.x_, pos->second.y_);

			// 点击之后会有连锁反应,需要等待一会儿,再去截取画面和点击
			//this_thread::sleep_for(chrono::milliseconds(100));
		}
		this_thread::sleep_for(chrono::milliseconds(200));
	}

	mouseControl.StopAndWait();
}

