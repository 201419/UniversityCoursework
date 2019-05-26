/**
* @file：MyString.h
* @description：MyString类，字符串类的定义
* @author：杨树
* @data：2019-01-09
*/

#pragma once

#include<iostream>


class MyString {
public:

	MyString() {}
	MyString(std::string s) {
		str = s;
		dataId = -1;
	}
	MyString(std::string s, int id) {
		str = s;
		dataId = id;
	}

	~MyString() {}

	void setID(int id) { dataId = id; }
	int getID() { return dataId; }

	std::string getData() { return str; }

private:

	int dataId;  // 数据在原始文件中的编号

	std::string str;  // 存放字符串

};