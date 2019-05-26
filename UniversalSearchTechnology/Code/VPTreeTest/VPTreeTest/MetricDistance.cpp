#include"MetricDistance.h"


int minimum(int a, int b, int c) {
	int min = (a <= b) ? a : b;
	return (min <= c) ? min : c;
}

double editDistance(MyString myStr1, MyString myStr2) {
	int i, j;

	int insertCost = 1;  // 插入的代价
	int deleteCost = 1;  // 删除的代价
	int replaceCost = 2;  // 替换的代价

	std::string str1 = myStr1.getData();
	std::string str2 = myStr2.getData();

	const int sizeofstr1 = str1.length();  // length of first
	const int sizeofstr2 = str2.length();  // length of second

	// 特殊情况的处理
	if (sizeofstr1 == 0) return sizeofstr2*insertCost;
	if (sizeofstr2 == 0) return sizeofstr1*deleteCost;

	// ----- 创建动态二维数组并初始化 -----
	int **matrix = new int*[sizeofstr1 + 1];  // the dynamic programming matrix
	for (i = 0; i <= sizeofstr1; i++) matrix[i] = new int[sizeofstr2 + 1];

	for (i = 0; i <= sizeofstr1; i++) matrix[i][0] = i*deleteCost;
	for (j = 0; j <= sizeofstr2; j++) matrix[0][j] = j*insertCost;

	// ----- 填充二维数组 -----
	char firstChar;
	int cost;  // 相等则代价为0；不相等代价为replaceCost

	for (i = 1; i <= sizeofstr1; i++) {
		firstChar = str1.at(i - 1);
		for (j = 1; j <= sizeofstr2; j++) {
			cost = (firstChar == str2.at(j - 1)) ? 0 : replaceCost;
			matrix[i][j] = minimum(matrix[i - 1][j] + deleteCost, matrix[i][j - 1] + insertCost, matrix[i - 1][j - 1] + cost);
		}
	}

	// ----- 保存编辑距离并释放数组内存 -----
	double matrixNum = matrix[sizeofstr1][sizeofstr2];

	for (int i = 0; i <= sizeofstr1; i++) { delete[] matrix[i]; }
	delete[]matrix;

	return matrixNum;

}


double euclideanDistance(DoubleVector vec1, DoubleVector vec2) {

	double distance = 0;

	// ---- 特殊情况的处理 -----
	if (vec1.getDim() == 0 || vec2.getDim() == 0) {
		std::cout << "vec1 or vec2 is null ..." << std::endl;
		return -1;
	}
	if (vec1.getDim() != vec2.getDim()) {
		std::cout << "vec1 and vec2 is not equal ..." << std::endl;
		return -1;
	}

	// ---- 计算 Euclidean 距离 -----
	for (int i = 0; i<vec1.getDim(); i++) distance += pow((vec1.getData(i) - vec2.getData(i)), 2);

	return sqrt(distance);

}