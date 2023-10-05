#include "helper.h"


void subdivideTriangle(int depth, std::vector<GLfloat>& vertices, float length)
{
	// 当深度小于等于0时 停止迭代
	if (depth <= 0)
	{
		return;
	}

	// 创建一个临时数组temp 保存原来的顶点信息
	std::vector<GLfloat> temp(vertices);
	vertices.clear();

	// 每个三角形有三个顶点 每个顶点有xyz三个分量
	int numOfTriangles = temp.size() / 3 / 3;

	// 对每个三角形 利用除原点外的两个顶点计算新的中点
	// 并利用该中点将原三角形分成两个新三角形
	for (int i = 0; i < numOfTriangles; i++)
	{
		// v1 v2 和 v3 分别为三角形的三个顶点 其中v1为原点
		GLfloat v1[3], v2[3], v3[3];
		for (int j = 0; j < 3; j++)
		{
			v1[j] = temp[i * 9 + j];
			v2[j] = temp[i * 9 + j + 3];
			v3[j] = temp[i * 9 + j + 6];
		}

		// 得到原点对边中点
		GLfloat v23[3];
		getHalf(v2, v3, v23, length);

		// 将顶点信息添加到vertices数组中
		addTriangle(v1, v2, v23, vertices);
		addTriangle(v1, v23, v3, vertices);
	}
	// 深度减1 进行下一轮迭代
	subdivideTriangle(depth - 1, vertices, length);
}

void getHalf(GLfloat v1[], GLfloat v2[], GLfloat v12[], GLfloat scale)
{
	// 该函数通过v1和v2得到中点v12
	// v12到原点的距离为1
	for (int i = 0; i < 3; i++)
	{
		v12[i] = v1[i] + v2[i];
	}
	double length = sqrt(v12[0] * v12[0] + v12[1] * v12[1] + v12[2] * v12[2]);
	for (int i = 0; i < 3; i++)
	{
		v12[i] = v12[i] / length * scale;
	}
}

void addTriangle(GLfloat v1[], GLfloat v2[], GLfloat v3[], std::vector<GLfloat>& v)
{
	// v1 v2 v3分别代表三角形的三个顶点
	// 每个顶点有xyz三个分量 将这些信息添加到vertices数组中
	v.push_back(v1[0]);
	v.push_back(v1[1]);
	v.push_back(v1[2]);

	v.push_back(v2[0]);
	v.push_back(v2[1]);
	v.push_back(v2[2]);

	v.push_back(v3[0]);
	v.push_back(v3[1]);
	v.push_back(v3[2]);
}

void addVertex(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& v)
{
	v.push_back(x);
	v.push_back(y);
	v.push_back(z);
}

void generateSideVertices(std::vector<GLfloat>& circleVertices, std::vector<GLfloat>& upperVertices,
	std::vector<GLfloat>& lowerVertices, std::vector<GLfloat>& sideVertices, float textureHeight) {
	// 设置圆柱/半圆柱侧面中曲面相关的顶点信息
	// 上下两个细分三角形的原点对边 构成一个侧面细分长方形
	int numOfTriangles = circleVertices.size() / 3 / 3;
	for (int i = 0; i < numOfTriangles; i++)
	{
		// 记上面的细分三角形为a 下面的细分三角形为b
		// v1 v2 和 v3 分别为原细分三角形的三个顶点
		GLfloat v1[3], v2[3], v3[3];
		GLfloat va1[3], va2[3], va3[3], vb1[3], vb2[3], vb3[3];
		for (int j = 0; j < 3; j++)
		{
			v1[j] = circleVertices[i * 9 + j];
			v2[j] = circleVertices[i * 9 + j + 3];
			v3[j] = circleVertices[i * 9 + j + 6];

			va1[j] = upperVertices[i * 24 + j];		// 原点 无作用
			va2[j] = upperVertices[i * 24 + j + 8];
			va3[j] = upperVertices[i * 24 + j + 16];
			vb1[j] = lowerVertices[i * 24 + j];		// 原点 无作用
			vb2[j] = lowerVertices[i * 24 + j + 8];
			vb3[j] = lowerVertices[i * 24 + j + 16];
		}
		/*
		侧视图如下
			   va3  va2
				|\^^|
				| \ |
				|__\|
			   vb3 vb2
		*/

		// va2顶点对应的法向量相当于 semicircleVertices中v2的位置坐标 下同
		addVertex(va2[0], va2[1], va2[2], sideVertices);	// 位置
		addVertex(v2[0], v2[1], v2[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(textureHeight);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);	// 位置
		addVertex(v2[0], v2[1], v2[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(0.0f);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		addVertex(va3[0], va3[1], va3[2], sideVertices);	// 位置
		addVertex(v3[0], v3[1], v3[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(textureHeight);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));

		addVertex(va3[0], va3[1], va3[2], sideVertices);	// 位置
		addVertex(v3[0], v3[1], v3[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(textureHeight);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);	// 位置
		addVertex(v2[0], v2[1], v2[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(0.0f);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		addVertex(vb3[0], vb3[1], vb3[2], sideVertices);	// 位置
		addVertex(v3[0], v3[1], v3[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(0.0f);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
	}
}