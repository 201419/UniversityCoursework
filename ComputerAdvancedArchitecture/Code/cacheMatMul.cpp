/*************************************************
Author: 杨 树 ( yang shu )
Date: 2018-12-20
Description: 矩阵乘法加速函数 -- 利用缓存
**************************************************/

#include <intrin.h>


// 老师给的矩阵乘法优化版本
void matmat(int n, float **mat_a, float **mat_b, float **mat_c) {
	int i, j, k;
	for (i = 0; i < n; i++) {
		for (k = 0; k < n; k++) {
			for (j = 0; j < n; j++) {
				mat_c[i][j] += mat_a[i][k] * mat_b[k][j];
			}
		}
	}
}


// AVX256 -- 加速版矩阵乘法
void matIntrin(int n, float **mat_a, float **mat_b, float **mat_c) {
	int i, j, k;
	float temp[8], tempResult;
	__m256 a256, b256, c256;

	float **mat_tempB;  // temp Matrix for B.T
	mat_tempB = new float*[n];
	for (i = 0; i < n; i++) { mat_tempB[i] = new float[n]; }

	for (i = 0; i < n; i++) {  // matrix transpose
		for (j = 0; j < n; j++) {
			mat_tempB[j][i] = mat_b[i][j];
		}
	}


	int nSub8 = n - 8;

	for (i = 0; i < n; i++) {  // 第一层循环
		for (j = 0; j < n; j++) {  // 第二层循环
			c256 = _mm256_setzero_ps();
			for (k = 0; k <= nSub8; k += 8) {  // 第三层循环 => multiply and add operation
				a256 = _mm256_loadu_ps(&mat_a[i][k]);
				b256 = _mm256_loadu_ps(&mat_tempB[j][k]);
				c256 = _mm256_add_ps(c256, _mm256_mul_ps(a256, b256));
			}
			tempResult = 0;
			for (; k < n; k++) { tempResult += mat_a[i][k] * mat_tempB[j][k]; }
			_mm256_storeu_ps(temp, c256);
			for (k = 0; k < 8; k++) { tempResult += temp[k]; }
			mat_c[i][j] = tempResult;  // write to memory
		}
	}

	// delete 2-dim array
	for (int i = 0; i < n; ++i) { delete[] mat_tempB[i]; }
	delete[] mat_tempB;

}


void cacheIntrin(int n, int j_start, int j_end, float **mat_a, float **mat_b, float **mat_c, int BlockSize) {
	// n ：matrix size （已经预处理为八的整数倍）
	// j_start and j_end ：对矩阵 B 的列进行分割，再交给不同的线程去跑
	// mat_c ：mat_a * mat_b
	// BlockSize ：scale of matrix block（依据 BlockSize 对矩阵分块）

	int loopUnrolling8 = 8;  // 循环展开
	int i, j, k;
	int _row, _col;  // 用于Packing矩阵的时候进行赋值
	float temp_1[8], temp_2[8], temp_3[8], temp_4[8], temp_5[8], temp_6[8], temp_7[8], temp_8[8];

	register int i_micro, j_micro, k_micro, tempIndex;  // 矩阵 A 块大小为 iMicroSize * kMicroSize，矩阵 B 块大小为 kMicroSize * jMicroSize
	int iMicroSize, kMicroSize, jMicroSize;

	__m256 a256_1, a256_2, a256_3, a256_4, a256_5, a256_6, a256_7, a256_8;
	__m256 b256_1;  // 一次取八个A和一个B，减少B的访存次数
	__m256 c256_1, c256_2, c256_3, c256_4, c256_5, c256_6, c256_7, c256_8;

	for (j = j_start; j < j_end; j += BlockSize) {  // 三个循环用于分块
		if (n - j < BlockSize) { jMicroSize = n - j; } else { jMicroSize = BlockSize; }

		for (k = 0; k < n; k += BlockSize) {
			if (n - k < BlockSize) { kMicroSize = n - k; } else { kMicroSize = BlockSize; }

			float *packB = new float[kMicroSize*jMicroSize];  // 矩阵 B 块大小为 kMicroSize * jMicroSize
			for (_row = 0; _row < kMicroSize; _row++) {  // Pack matrix_B : Transpose and Vectorization
				float *mat_b_row = mat_b[k + _row];
				for (_col = 0; _col < jMicroSize; _col++) {  // 矩阵 B 的块的左上角坐标为 (k, j)
					packB[_col * kMicroSize + _row] = mat_b_row[j + _col];
				}
			}
			for (i = 0; i < n; i += BlockSize) {
				if (n - i < BlockSize) { iMicroSize = n - i; } else { iMicroSize = BlockSize; }

				float *packA = new float[iMicroSize*kMicroSize];  // 矩阵 A 块大小为 iMicroSize * kMicroSize
				for (_row = 0; _row < iMicroSize; _row++) {  // Pack matrix_A : Vectorization
					float *mat_a_row = mat_a[i + _row];
					for (_col = 0; _col < kMicroSize; _col++) {  // 矩阵 A 的块的左上角坐标为 (i, k)
						packA[_row * kMicroSize + _col] = mat_a_row[k + _col];
					}
				}

				// --- --- ---

				for (j_micro = 0; j_micro < jMicroSize; j_micro++) {  // 对块进行处理
					for (i_micro = 0; i_micro < iMicroSize; i_micro += loopUnrolling8) {  // 矩阵A循环展开，八行一起算
						c256_1 = _mm256_setzero_ps(); c256_2 = _mm256_setzero_ps();
						c256_3 = _mm256_setzero_ps(); c256_4 = _mm256_setzero_ps();
						c256_5 = _mm256_setzero_ps(); c256_6 = _mm256_setzero_ps();
						c256_7 = _mm256_setzero_ps(); c256_8 = _mm256_setzero_ps();

						// 最内层循环
						for (k_micro = 0; k_micro < kMicroSize; k_micro += loopUnrolling8) {  // 矩阵A循环展开，八行一起算
							a256_1 = _mm256_loadu_ps(&packA[i_micro * kMicroSize + k_micro]);
							a256_2 = _mm256_loadu_ps(&packA[(i_micro + 1) * kMicroSize + k_micro]);
							a256_3 = _mm256_loadu_ps(&packA[(i_micro + 2) * kMicroSize + k_micro]);
							a256_4 = _mm256_loadu_ps(&packA[(i_micro + 3) * kMicroSize + k_micro]);
							a256_5 = _mm256_loadu_ps(&packA[(i_micro + 4) * kMicroSize + k_micro]);
							a256_6 = _mm256_loadu_ps(&packA[(i_micro + 5) * kMicroSize + k_micro]);
							a256_7 = _mm256_loadu_ps(&packA[(i_micro + 6) * kMicroSize + k_micro]);
							a256_8 = _mm256_loadu_ps(&packA[(i_micro + 7) * kMicroSize + k_micro]);

							b256_1 = _mm256_loadu_ps(&packB[j_micro * kMicroSize + k_micro]);
							c256_1 = _mm256_fmadd_ps(a256_1, b256_1, c256_1);
							c256_2 = _mm256_fmadd_ps(a256_2, b256_1, c256_2);
							c256_3 = _mm256_fmadd_ps(a256_3, b256_1, c256_3);
							c256_4 = _mm256_fmadd_ps(a256_4, b256_1, c256_4);
							c256_5 = _mm256_fmadd_ps(a256_5, b256_1, c256_5);
							c256_6 = _mm256_fmadd_ps(a256_6, b256_1, c256_6);
							c256_7 = _mm256_fmadd_ps(a256_7, b256_1, c256_7);
							c256_8 = _mm256_fmadd_ps(a256_8, b256_1, c256_8);
						}
						_mm256_storeu_ps(temp_1, c256_1); _mm256_storeu_ps(temp_2, c256_2);
						_mm256_storeu_ps(temp_3, c256_3); _mm256_storeu_ps(temp_4, c256_4);
						_mm256_storeu_ps(temp_5, c256_5); _mm256_storeu_ps(temp_6, c256_6);
						_mm256_storeu_ps(temp_7, c256_7); _mm256_storeu_ps(temp_8, c256_8);

						for (tempIndex = 0; tempIndex < 8; tempIndex++) {
							mat_c[i + i_micro][j + j_micro] += temp_1[tempIndex];
							mat_c[i + i_micro + 1][j + j_micro] += temp_2[tempIndex];
							mat_c[i + i_micro + 2][j + j_micro] += temp_3[tempIndex];
							mat_c[i + i_micro + 3][j + j_micro] += temp_4[tempIndex];
							mat_c[i + i_micro + 4][j + j_micro] += temp_5[tempIndex];
							mat_c[i + i_micro + 5][j + j_micro] += temp_6[tempIndex];
							mat_c[i + i_micro + 6][j + j_micro] += temp_7[tempIndex];
							mat_c[i + i_micro + 7][j + j_micro] += temp_8[tempIndex];
						}

					}
				}

				// --- --- ---

				delete[] packA;
			}

			delete[] packB;
		}
	}

}
