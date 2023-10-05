#ifndef __HELPER_H__
#define __HELPER_H__

#include <GL/glew.h>
#include <vector>

// 细分得到圆或半圆 结果保存在数组v中
void subdivideTriangle(int depth, std::vector<GLfloat>& v, float length = 1.0f);
// 通过两个原有顶点位置计算得到新的中点位置
void getHalf(GLfloat v1[], GLfloat v2[], GLfloat v12[], GLfloat scale = 1.0f);
// 将三角形v1v2v3的顶点信息添加到vertices数组中
void addTriangle(GLfloat v1[], GLfloat v2[], GLfloat v3[], std::vector<GLfloat>& v);
void addVertex(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& v);

#endif // __HELPER_H__