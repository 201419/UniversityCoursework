/*************************************************
Author: 杨 树 ( yang shu )
Date: 2018-12-20
Description: 矩阵乘法加速函数 -- 利用缓存
**************************************************/

#pragma once

// 老师给的矩阵乘法优化版本
void matmat(int n, float **mat_a, float **mat_b, float **mat_c);

// AVX256 -- 加速版矩阵乘法
void matIntrin(int n, float **mat_a, float **mat_b, float **mat_c);

// My Optimization Version ：（AVX256 + 循环展开 + 矩阵分块 + Cache + 多线程）
void cacheIntrin(int n, int j_start, int j_end, float **mat_a, float **mat_b, float **mat_c, int BlockSize);