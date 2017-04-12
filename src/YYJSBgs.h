/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "bgslibrary.h"
#include "BgsNameEnum.h"

/**
前景背景分离
*/
class YYJSBgs
{

	IBGS *bgs;

public:

	YYJSBgs(void);
	~YYJSBgs(void);

	/**
	根据不同的名称构造不同的背景分离器
	@param	bgsName	背景分离算法名
	*/
	void createBgs(BgsNameEnum bgsName);

	/**
	调用算法执行具体的前背景分离操作
	@param	input		输入图像
	@param	foreground	前景图像，二值图像，与input有同样的尺寸
	@param	background	背景图像，二值图像，与input有同样的尺寸
	*/
	void process(Mat input,Mat& foreground,Mat& background);


};
