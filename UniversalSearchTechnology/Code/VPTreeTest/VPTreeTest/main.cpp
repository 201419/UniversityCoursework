/**
* @file��main.cpp
* @description��������
* @author������
* @data��2019-01-09
*/

#include<iostream>
#include<fstream>
#include"MyString.h"
#include"DoubleVector.h"
#include"MetricDistance.h"
#include"VPTree.h"


// ���� >>
std::istream& operator >> (std::istream& is, std::vector<MyString> &strItems) {
	size_t num = 0;
	is >> num;
	for (size_t i = 0; i < num; ++i) {
		std::string str;
		is >> str;
		MyString item(str, i);
		strItems.push_back(item);
	}
	return is;
}
std::istream& operator >> (std::istream& is, std::vector<DoubleVector> &douItems) {
	size_t num = 0;
	is >> num;
	size_t dim = 0;
	is >> dim;
	for (size_t i = 0; i < num; ++i) {
		std::vector<double> data;
		for (size_t j = 0; j < dim; ++j) {
			double k = 0;
			is >> k;
			data.push_back(k);
		}
		DoubleVector item(data, i);
		douItems.push_back(item);
	}
	return is;
}


int main() {
	
	std::vector<MyString> strItems;
	VPTree<MyString, editDistance> vpt_string;
	// ----- read file -----
	std::ifstream ifs;
	ifs.open("stringData.txt");
	if (!ifs) { std::cout << "[stringData.txt] is empty ..." << std::endl; return 0; }
	ifs >> strItems;
	ifs.close();  // close
	std::cout << "Reading [stringData.txt] is completed ..." << std::endl;
	// ----- ���� VP Tree -----
	vpt_string.create(strItems);
	std::cout << "[MyString VPTree] is created ..." << std::endl;

	std::vector<DoubleVector> douItems;
	VPTree<DoubleVector, euclideanDistance> vpt_double;
	// ----- read file -----
	//std::ifstream ifs;
	ifs.open("doubleData.txt");
	if (!ifs) { std::cout << " [doubleData.txt] is empty ..." << std::endl; return 0; }
	ifs >> douItems;
	ifs.close();  // close
	std::cout << "Reading [doubleData.txt] is completed ..." << std::endl;
	// ----- ���� VP Tree -----
	vpt_double.create(douItems);
	std::cout << "[Vector VPTree] is created ..." << std::endl;

	std::cout << std::endl;
	std::cout << "************************************************************************" << std::endl;
	std::cout << "Input �� [method(1��k-���ڣ�2��r-��Χ)] [string] [k value or r value]" << std::endl;
	std::cout << "-------------------------------" << std::endl;
	std::cout << "Input Example �� 1 sjkdj 5" << std::endl;
	std::cout << "It means ��ʹ��k���ڲ��Ҿ����ַ�����sjkdj�������5������" << std::endl;
	std::cout << "************************************************************************" << std::endl;
	std::cout << "Input �� [method(1��k-���ڣ�2��r-��Χ)] [vector] [k value or r value]" << std::endl;
	std::cout << "-------------------------------" << std::endl;
	std::cout << "Input Example �� 1 3.5 3.5 5" << std::endl;
	std::cout << "It means ��ʹ��k���ڲ��Ҿ������� [3.5, 3.5] �����5������" << std::endl;
	std::cout << "************************************************************************" << std::endl;
	std::cout << std::endl;


	int exit;
	while (true) {
		std::cout << "Please input a number(0��Exit   1��Query String   2��Query Vector)�� ";
		std::cin >> exit;
		if (exit == 0) {
			break;
		} else if (exit == 1) {
			std::cout << "Query String�� ";
			// ----- ��ѯ�ַ��� -----
			size_t method, k;
			double r;
			std::string str;
			std::vector<MyString> results;
			std::vector<double> distances;

			std::cin >> method >> str;
			MyString target(str);
			if (method == 1) {  // k���ڲ�ѯ
				std::cin >> k;
				vpt_string.knnQuery(target, results, distances, k);
			} else if (method == 2) {  // r��Χ��ѯ
				std::cin >> r;
				vpt_string.rangeQuery(target, r, results, distances);
			}
			//vpt_string.query(target, results, distances);  // ��ѯ����ĵ�
			std::cout << "[String Query] is completed ..." << std::endl;

			// ----- ��ѯ������ -----
			std::cout << std::endl;
			std::cout << " Id\t�ַ�������\t����" << std::endl;
			std::cout << "----------------------------------" << std::endl;
			for (size_t i = 0; i < results.size(); i++) {
				std::cout << " " << results[i].getID() << "\t[" << results[i].getData() << "]   \t" << distances[i] << std::endl;
			}
			std::cout << std::endl;

		} else if (exit == 2) {
			std::cout << "Query Vector�� ";
			// ----- ��ѯ���� -----
			size_t method, k;
			double r;
			std::vector<double> data;
			std::vector<DoubleVector> results;
			std::vector<double> distances;

			std::cin >> method;
			for (size_t t = 0; t < 2; t++) {
				double value;
				std::cin >> value;
				data.push_back(value);
			}
			DoubleVector target(data);
			if (method == 1) {  // k���ڲ�ѯ
				std::cin >> k;
				vpt_double.knnQuery(target, results, distances, k);
			} else if (method == 2) {  // r��Χ��ѯ
				std::cin >> r;
				vpt_double.rangeQuery(target, r, results, distances);
			}
			//vpt_double.query(target, results, distances);  // ��ѯ����ĵ�
			std::cout << "[Vector Query] is completed ..." << std::endl;

			// ----- ��ѯ������ -----
			std::cout << std::endl;
			std::cout << " Id\t��������\t����" << std::endl;
			std::cout << "----------------------------------" << std::endl;
			for (size_t i = 0; i < results.size(); i++) {
				std::cout << " " << results[i].getID() << "\t[" << results[i].getData(0);
				for (int j = 1; j < results[i].getDim(); j++) {
					std::cout << ", " << results[i].getData(j);
				}
				std::cout << "]   \t" << distances[i] << std::endl;
			}
			std::cout << std::endl;

		}
	}
	
	std::cout << std::endl;

	system("pause");
	return 0;

}