/**
 * @file mylib.h
 * @author TITH (1023515576@qq.com)
 * @brief 自己写的基础库，用于替换标准库中不好用的函数或者添加功能
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

/**
 * @brief 字符串转浮点数（标准库中的atof在 freertos 中调用会死机，原因不明）
 * 
 * @param s 
 * @return double 
 */
double myatof(const char * s);
