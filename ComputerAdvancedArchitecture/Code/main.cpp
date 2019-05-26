/*************************************************
Author: 杨 树 ( yang shu )
Date: 2018-12-13
Description: 测试矩阵乘法加速效果
**************************************************/

#include <iostream>
#include <intrin.h>
#include <time.h> 
#include <thread>
#include <vector>
#include <fstream>
#include "cacheMatMul.h"
using namespace std;

// 输出矩阵
void display(int n, float **mat) {
	cout << "************************************" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) { cout << mat[i][j] << " "; }
		cout << endl;
	}
	cout << "************************************" << endl;
}

// 检查矩阵运算是否正确（即优化前后结果是否一致）
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

// My Optimization Version ：（AVX256 + 循环展开 + 矩阵分块 + Cache + 多线程）
void OptimVersion(int nReal, int nCalcu, float **mat_a, float **mat_b, float **mat_c) {

	// nReal：  矩阵实际大小
	// nCalcu： 对矩阵边缘填充之后的大小，即实际计算时的大小
	//          nCalcu % 8 == 0 and 0 <= nCalcu - nReal <= 7
	// mat_c：  mat_a * mat_b


	int BlockSize;  // 矩阵分块大小，0代表不分块
	// 经测试，512*512的分块速度较快，这个规模 512*512*4 = 1 MB
	// 我的电脑是 "Intel(R) Core(TM) i5-4590 CPU @ 3.30GHz"
	// 其中 L1缓存为256KB，L2缓存为1MB，L1缓存为6MB
	if (nCalcu >= 2048) { BlockSize = 512; }  // 超过2048的规模时，块大小设置为512
	else if (nCalcu >= 768) { BlockSize = 256; }  // 规模在768到2048时，块大小设置为256
	else if (nCalcu >= 256) { BlockSize = 64; }  // 规模在256到768时，块大小设置为64
	else { BlockSize = 0; }  // 规模小于256时，不分块，直接用AVX算

	if (BlockSize == 0) {
		matIntrin(nCalcu, mat_a, mat_b, mat_c);
		return;
	}

	int threadNum = 4;  // 线程数
	int start_1, start_2, start_3;
	vector<thread> pool;

	// 对四个线程均衡负载（尽量使得四个线程分到的矩阵 B 的列数比较均衡），若改为其他线程数需从新设计
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


		// ----- 创建二位数组并对矩阵边缘进行填充 -----

		int nCalcu = n + (8 - (n % 8)) % 8;  // 对矩阵边缘填充使得矩阵规模为八的整数倍
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

		// ----- 矩阵乘法（老师给的调换内外循环版本） -----
		t_start = clock();
		matmat(n, mat_a, mat_b, mat_c_mul);  // mat multiply mat
		t_end = clock();
		t_matmat = (double(t_end - t_start)) / CLOCKS_PER_SEC;

		cout << "T1（老师给的调换内外循环版本）：【" << t_matmat << "】 s ..." << endl << endl;
		outfile.open("runLog.txt", ios::app);  // LOG Record ...
		if (outfile.is_open()) {
			outfile << "T1（老师给的调换内外循环版本）：【" << t_matmat << "】 s ..." << endl << endl;
			outfile.close();
		}

		// ----- AVX256 + 循环展开 + 矩阵分块 + Cache + 多线程（自己写的最优版本） -----
		t_start = clock();
		OptimVersion(n, nCalcu, mat_a, mat_b, mat_c_intrin);
		t_end = clock();
		t_matIntrin = (double(t_end - t_start)) / CLOCKS_PER_SEC;

		cout << "T2（AVX256+循环展开+矩阵分块+Cache+多线程）：【" << t_matIntrin << "】 s ..." << endl << endl;
		outfile.open("runLog.txt", ios::app);  // LOG Record ...
		if (outfile.is_open()) {
			outfile << "T2（AVX256+循环展开+矩阵分块+Cache+多线程）：【" << t_matIntrin << "】 s ..." << endl << endl;
			outfile.close();
		}
		//cout << endl << mat_a[0][n - 1] << " " << mat_a[0][n] << endl;
		//cout << mat_c_intrin[n - 1][n - 1] << " " << mat_c_intrin[n - 1][n] << endl << endl;

		// ----- -----

		cout << "加速比（Speedup ratio）： T1/T2 = " << t_matmat / t_matIntrin << endl << endl << endl;
		outfile.open("runLog.txt", ios::app);
		if (outfile.is_open()) {
			outfile << "加速比（Speedup ratio）： T1/T2 = " << t_matmat / t_matIntrin << endl << endl << endl;
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
