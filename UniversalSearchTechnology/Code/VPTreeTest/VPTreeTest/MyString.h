/**
* @file��MyString.h
* @description��MyString�࣬�ַ�����Ķ���
* @author������
* @data��2019-01-09
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

	int dataId;  // ������ԭʼ�ļ��еı��

	std::string str;  // ����ַ���

};