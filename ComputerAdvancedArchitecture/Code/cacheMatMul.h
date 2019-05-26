/*************************************************
Author: �� �� ( yang shu )
Date: 2018-12-20
Description: ����˷����ٺ��� -- ���û���
**************************************************/

#pragma once

// ��ʦ���ľ���˷��Ż��汾
void matmat(int n, float **mat_a, float **mat_b, float **mat_c);

// AVX256 -- ���ٰ����˷�
void matIntrin(int n, float **mat_a, float **mat_b, float **mat_c);

// My Optimization Version ����AVX256 + ѭ��չ�� + ����ֿ� + Cache + ���̣߳�
void cacheIntrin(int n, int j_start, int j_end, float **mat_a, float **mat_b, float **mat_c, int BlockSize);