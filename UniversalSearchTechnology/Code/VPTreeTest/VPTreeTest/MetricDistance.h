/**
* @file��MetricDistance.h
* @description�����ڶ�������֮��ľ���
* @author������
* @data��2019-01-09
*/

#pragma once

#include<string>
#include"MyString.h"
#include"DoubleVector.h"

/** �����ַ���֮��� Levenshtein ���루���༭���룩
* @param myStr1�� ��һ���ַ���
* @param myStr2�� �ڶ����ַ���
* @return�� ���شӵ�һ���ַ������ڶ����ַ����� Levenshtein ����
*/
double editDistance(MyString myStr1, MyString myStr2);


/** ��������֮��� Euclidean ����
* @param vec1����һ������
* @param vec2���ڶ�������
* @return��������������֮��� Euclidean ����
*/
double euclideanDistance(DoubleVector vec1, DoubleVector vec2);