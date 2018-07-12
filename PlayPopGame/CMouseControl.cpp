#include "stdafx.h"
#include "config.h"
#include "CMouseControl.h"

#include <future>
#include <cassert>

using namespace std;

MouseControl::~MouseControl() {
	// ������� �ֶ�����stop
	assert(true == exit_);
}

bool MouseControl::Init(LPCTSTR programTitle)
{
	HWND hwnd = FindWindow(NULL, programTitle);
	if (NULL == hwnd) {
		return false;
	}
	hwnd_ = hwnd;

	// �����첽����
	ret_ =  std::async(std::launch::async, [this]() {
		// ��Ҫ�ȴ��������е�����ֵȫ��������
		while (!(exit_ && safeQueue_.size() <= 0)) {
			pair<int, int> pos;
			if (safeQueue_.pop_wait(&pos, 100)) {
				Clicked(pos.first, pos.second);
				if (timeoutMills_ > 0) {
					// �ȴ�һ���
					this_thread::sleep_for(chrono::milliseconds(timeoutMills_));
				}
			}
			else {
				this_thread::sleep_for(chrono::microseconds(100));
			}
		}
	});
	return true;
}

void MouseControl::ClickedAsync(int xpos, int ypos)
{
	safeQueue_.push(make_pair(xpos, ypos));
}

void MouseControl::Clicked(int x, int y)
{
	if (hwnd_ != nullptr) {

		auto pos = calPos(x, y);
		//PostMessage(hwnd_, WM_LBUTTONDOWN, 0, MAKELPARAM(pos.first, pos.second));
		//PostMessage(hwnd_, WM_LBUTTONUP, 0, MAKELPARAM(pos.first, pos.second));

		SendMessage(hwnd_, WM_LBUTTONDOWN, 0, MAKELPARAM(pos.first, pos.second));
		SendMessage(hwnd_, WM_LBUTTONUP, 0, MAKELPARAM(pos.first, pos.second));

		
	}
}

pair<int, int> MouseControl::calPos(int i, int j) {
	return make_pair(StartPosX + ItemWidth * j + ItemWidth / 2, StartPosY + ItemHeight * i + ItemHeight / 2);
}

void MouseControl::StopAndWait() {
	if (!exit_) {
		exit_ = true;
		return ret_.get(); // �ȴ��첽�������
	}
}
