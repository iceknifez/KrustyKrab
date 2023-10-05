#ifndef __HELPER_H__
#define __HELPER_H__

// 该头文件主要包含了与 细分三角形 相关的方法

#include <GL/glew.h>
#include <vector>

// 细分三角形得到圆或半圆 结果保存在数组v中
void subdivideTriangle(int depth, std::vector<GLfloat>& v, float length = 1.0f);
// 通过两个原有顶点位置计算得到新的中点位置
void getHalf(GLfloat v1[], GLfloat v2[], GLfloat v12[], GLfloat scale = 1.0f);
// 将三角形v1v2v3的顶点信息添加到vertices数组中
void addTriangle(GLfloat v1[], GLfloat v2[], GLfloat v3[], std::vector<GLfloat>& v);
// 将 x, y, z 加入数组v中
void addVertex(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& v);
// 设置圆柱/半圆柱侧面中曲面相关的顶点信息
// 上下两个细分三角形的原点对边 构成一个侧面细分长方形
void generateSideVertices(std::vector<GLfloat>& circleVertices, std::vector<GLfloat>& upperVertices,
	std::vector<GLfloat>& lowerVertices, std::vector<GLfloat>& sideVertices, float textureHeight = 1.0f);

#endif // __HELPER_H__