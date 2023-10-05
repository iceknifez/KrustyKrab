#ifndef __SHAPE_COORDINATE_H__
#define __SHAPE_COORDINATE_H__

#include <GL/glew.h>

// 正方体
extern GLfloat cube[12 * 3 * 8];

// 餐厅底部
extern GLfloat restaurant_bottom[22 * 4 * 8];
// 餐厅侧面和门板
extern GLfloat restaurant_side[18 * 4 * 8];
// 餐厅侧面屋顶下的上半部分
extern GLfloat restaurant_side2[24 * 4 * 8];

// 天空盒（正方体）
extern GLfloat skyboxVertices[12 * 3 * 8];

// 初始三角形（用于细分生成圆或半圆）
// 按顺时针（俯视）排列的四个等腰三角形
extern GLfloat initialTriangle[4 * 3 * 3];

// 初始三角形（用于细分生成圆或半圆） 竖直版本
// 按顺时针（从右向左看）排列的四个等腰三角形
extern GLfloat initialTriangleVertical[4 * 3 * 3];

#endif // __SHAPE_COORDINATE_H__