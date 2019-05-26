/**
* @file：DoubleVector.h
* @description：DoubleVector类，向量类的定义
* @author：杨树
* @data：2019-01-09
*/

#pragma once

#include<vector>

class DoubleVector {
public:
	
	DoubleVector() { dim = 0; }
	DoubleVector(std::vector<double> data) {
		dim = data.size();
		dataList = data;
		dataId = -1;
	}
	DoubleVector(std::vector<double> data, int id) {
		dim = data.size();
		dataList = data;
		dataId = id;
	}

	~DoubleVector() {}

	void setID(int id) { dataId = id; }
	int getID() { return dataId; }

	int getDim() { return dim; }
	double getData(int i) { return dataList[i]; }

	//bool operator==(DoubleVector douV) {
	//	bool equal = true;
	//	for (int i = 0; i < dim; i++) {
	//		if (dataList[i] != douV.getData(i)) { equal = false; break; }
	//	}
	//	return equal;
	//}

private:

	int dataId;  // 数据在原始文件中的编号

	int dim;  // 向量数据的维度

	std::vector<double> dataList;  // 向量数据

};