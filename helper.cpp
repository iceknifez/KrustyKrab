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