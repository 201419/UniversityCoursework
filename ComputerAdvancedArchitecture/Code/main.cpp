/*************************************************
Author: �� �� ( yang shu )
Date: 2018-12-13
Description: ���Ծ���˷�����Ч��
**************************************************/

#include <iostream>
#include <intrin.h>
#include <time.h> 
#include <thread>
#include <vector>
#include <fstream>
#include "cacheMatMul.h"
using namespace std;

// �������
void display(int n, float **mat) {
	cout << "************************************" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) { cout << mat[i][j] << " "; }
		cout << endl;
	}
	cout << "************************************" << endl;
}

// �����������Ƿ���ȷ�����Ż�ǰ�����Ƿ�һ�£�
float SumofSquaredError(int n, float **mat_right, float **mat_test) {
	int i, j;
	float SumofSquaredError = 0;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			SumofSquaredError += (mat_right[i][j] - mat_test[i][j])*(mat_right[i][j] - mat_test[i][j]);
		}
	}
	return SumofSquaredError;
}

// My Optimization Version ����AVX256 + ѭ��չ�� + ����ֿ� + Cache + ���̣߳�
void OptimVersion(int nReal, int nCalcu, float **mat_a, float **mat_b, float **mat_c) {

	// nReal��  ����ʵ�ʴ�С
	// nCalcu�� �Ծ����Ե���֮��Ĵ�С����ʵ�ʼ���ʱ�Ĵ�С
	//          nCalcu % 8 == 0 and 0 <= nCalcu - nReal <= 7
	// mat_c��  mat_a * mat_b


	int BlockSize;  // ����ֿ��С��0�����ֿ�
	// �����ԣ�512*512�ķֿ��ٶȽϿ죬�����ģ 512*512*4 = 1 MB
	// �ҵĵ����� "Intel(R) Core(TM) i5-4590 CPU @ 3.30GHz"
	// ���� L1����Ϊ256KB��L2����Ϊ1MB��L1����Ϊ6MB
	if (nCalcu >= 2048) { BlockSize = 512; }  // ����2048�Ĺ�ģʱ�����С����Ϊ512
	else if (nCalcu >= 768) { BlockSize = 256; }  // ��ģ��768��2048ʱ�����С����Ϊ256
	else if (nCalcu >= 256) { BlockSize = 64; }  // ��ģ��256��768ʱ�����С����Ϊ64
	else { BlockSize = 0; }  // ��ģС��256ʱ�����ֿ飬ֱ����AVX��

	if (BlockSize == 0) {
		matIntrin(nCalcu, mat_a, mat_b, mat_c);
		return;
	}

	int threadNum = 4;  // �߳���
	int start_1, start_2, start_3;
	vector<thread> pool;

	// ���ĸ��߳̾��⸺�أ�����ʹ���ĸ��̷ֵ߳��ľ��� B �������ȽϾ��⣩������Ϊ�����߳�����������
	if ((nCalcu / BlockSize) % threadNum == 0) {
		start_1 = ((nCalcu / BlockSize) / threadNum)*BlockSize;
		start_2 = (2 * (nCalcu / BlockSize) / threadNum)*BlockSize;
		start_3 = (3 * (nCalcu / BlockSize) / threadNum)*BlockSize;
	} else {
		start_1 = (((nCalcu / BlockSize) / threadNum) + 1)*BlockSize;
		start_2 = ((2 * (nCalcu / BlockSize) / threadNum) + 1)*BlockSize;
		start_3 = ((3 * (nCalcu / BlockSize) / threadNum) + 1)*BlockSize;
	}

	pool.push_back(thread(cacheIntrin, nCalcu, 0, start_1, mat_a, mat_b, mat_c, BlockSize));
	pool.push_back(thread(cacheIntrin, nCalcu, start_1, start_2, mat_a, mat_b, mat_c, BlockSize));
	pool.push_back(thread(cacheIntrin, nCalcu, start_2, start_3, mat_a, mat_b, mat_c, BlockSize));
	pool.push_back(thread(cacheIntrin, nCalcu, start_3, nCalcu, mat_a, mat_b, mat_c, BlockSize));
	pool[0].join();
	pool[1].join();
	pool[2].join();
	pool[3].join();

}

int main() {
	ofstream outfile;
	int n;
	while (true) {

		cout << "please input a number ( Exit if less than 1 ) : ";
		cin >> n;
		if (n < 1) break;
		cout << endl;

		// LOG Record ... 
		outfile.open("runLog.txt", ios::app);
		if (outfile.is_open()) {
			outfile << "please input a number ( Exit if less than 1 ) : " << n << endl << endl;
			outfile.close();
		}


		// ----- ������λ���鲢�Ծ����Ե������� -----

		int nCalcu = n + (8 - (n % 8)) % 8;  // �Ծ����Ե���ʹ�þ����ģΪ�˵�������
		//cout << "nCalcu value is " << nCalcu << endl << endl;

		// create dynamic 2-dim array
		int i, j;

		// new 2-dim array
		float **mat_a, **mat_b, **mat_c_mul, **mat_c_intrin;
		mat_a = new float*[nCalcu]; for (i = 0; i < nCalcu; i++) { mat_a[i] = new float[nCalcu]; }
		mat_b = new float*[nCalcu]; for (i = 0; i < nCalcu; i++) { mat_b[i] = new float[nCalcu]; }
		mat_c_mul = new float*[nCalcu]; for (i = 0; i < nCalcu; i++) { mat_c_mul[i] = new float[nCalcu]; }
		mat_c_intrin = new float*[nCalcu]; for (i = 0; i < nCalcu; i++) { mat_c_intrin[i] = new float[nCalcu]; }

		srand((unsigned)time(nullptr));
		for (i = 0; i < n; i++) {  // assign value
			for (j = 0; j < n; j++) {
				mat_a[i][j] = float(rand() / double(RAND_MAX));
				mat_b[i][j] = float(rand() / double(RAND_MAX));
				mat_c_mul[i][j] = 0;
				mat_c_intrin[i][j] = 0;
			}
			for (; j < nCalcu; j++) {
				mat_a[i][j] = 0; mat_b[i][j] = 0;
				mat_c_mul[i][j] = 0; mat_c_intrin[i][j] = 0;
			}
		}
		for (; i < nCalcu; i++) {
			for (j = 0; j < nCalcu; j++) {
				mat_a[i][j] = 0; mat_b[i][j] = 0;
				mat_c_mul[i][j] = 0; mat_c_intrin[i][j] = 0;
			}
		}


		// ----- Calculate Time -----
		clock_t t_start, t_end;
		double t_matmat, t_matIntrin;

		// ----- ����˷�����ʦ���ĵ�������ѭ���汾�� -----
		t_start = clock();
		matmat(n, mat_a, mat_b, mat_c_mul);  // mat multiply mat
		t_end = clock();
		t_matmat = (double(t_end - t_start)) / CLOCKS_PER_SEC;

		cout << "T1����ʦ���ĵ�������ѭ���汾������" << t_matmat << "�� s ..." << endl << endl;
		outfile.open("runLog.txt", ios::app);  // LOG Record ...
		if (outfile.is_open()) {
			outfile << "T1����ʦ���ĵ�������ѭ���汾������" << t_matmat << "�� s ..." << endl << endl;
			outfile.close();
		}

		// ----- AVX256 + ѭ��չ�� + ����ֿ� + Cache + ���̣߳��Լ�д�����Ű汾�� -----
		t_start = clock();
		OptimVersion(n, nCalcu, mat_a, mat_b, mat_c_intrin);
		t_end = clock();
		t_matIntrin = (double(t_end - t_start)) / CLOCKS_PER_SEC;

		cout << "T2��AVX256+ѭ��չ��+����ֿ�+Cache+���̣߳�����" << t_matIntrin << "�� s ..." << endl << endl;
		outfile.open("runLog.txt", ios::app);  // LOG Record ...
		if (outfile.is_open()) {
			outfile << "T2��AVX256+ѭ��չ��+����ֿ�+Cache+���̣߳�����" << t_matIntrin << "�� s ..." << endl << endl;
			outfile.close();
		}
		//cout << endl << mat_a[0][n - 1] << " " << mat_a[0][n] << endl;
		//cout << mat_c_intrin[n - 1][n - 1] << " " << mat_c_intrin[n - 1][n] << endl << endl;

		// ----- -----

		cout << "���ٱȣ�Speedup ratio���� T1/T2 = " << t_matmat / t_matIntrin << endl << endl << endl;
		outfile.open("runLog.txt", ios::app);
		if (outfile.is_open()) {
			outfile << "���ٱȣ�Speedup ratio���� T1/T2 = " << t_matmat / t_matIntrin << endl << endl << endl;
			outfile.close();
		}


		// delete 2-dim array
		for (int i = 0; i < n; ++i) {
			delete[] mat_a[i];
			delete[] mat_b[i];
			delete[] mat_c_mul[i];
			delete[] mat_c_intrin[i];
		}
		delete[] mat_a;
		delete[] mat_b;
		delete[] mat_c_mul;
		delete[] mat_c_intrin;
	}

	system("pause");  // exit
	return 0;
}
