/**
* @file：MetricDistance.h
* @description：用于度量数据之间的距离
* @author：杨树
* @data：2019-01-09
*/

#pragma once

#include<string>
#include"MyString.h"
#include"DoubleVector.h"

/** 计算字符串之间的 Levenshtein 距离（即编辑距离）
* @param myStr1： 第一个字符串
* @param myStr2： 第二个字符串
* @return： 返回从第一个字符串到第二个字符串的 Levenshtein 距离
*/
double editDistance(MyString myStr1, MyString myStr2);


/** 计算向量之间的 Euclidean 距离
* @param vec1：第一个向量
* @param vec2：第二个向量
* @return：返回两个向量之间的 Euclidean 距离
*/
double euclideanDistance(DoubleVector vec1, DoubleVector vec2);