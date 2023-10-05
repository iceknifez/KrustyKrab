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

#endif // __SHAPE_COORDINATE_H__