#ifndef __MYGLWIDGET_H__
#define __MYGLWIDGET_H__

#ifdef MAC_OS
#include <QtOpenGL/QtOpenGL>
#else
#include <GL/glew.h>
#endif
#include <QtGui>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QGLWidget>

#include <QtCore/qdir.h>
#include <qopenglextrafunctions.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <time.h>
#include <conio.h>

#include <unordered_map>
#include "shape_coordinate.h"
#include "helper.h"

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    MyGLWidget(QWidget* parent = nullptr);
    ~MyGLWidget();

protected:
    // QOpenGLWidget提供的三个虚函数
    // initializeGL：在创建OpenGL环境时被调用，用于进行OpenGL的初始化操作
    // paintGL：在每次窗口需要重新绘制时被调用，用于绘制OpenGL场景
    // resizeGL：在窗口尺寸发生变化时被调用，用于处理OpenGL视口的调整
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void keyPressEvent(QKeyEvent* event);   // 键盘事件

    // 根据指定的文件中的内容初始化顶点着色器和片元着色器
    void initShader(std::string vertexPath, std::string fragmentPath);
    // 更新着色器的信息 每次绘制前调用
    void updateShader();

    /* ******************** 状态信息设置函数 ******************** */

    // 设置漫反射纹理贴图
    void setDiffuseMap(const char* imagePath);
    // 设置物体颜色 绘制前调用
    void setObjectColor(float r, float g, float b);
    // 设置观察视角 修改摄像机位置时调用
    void setViewPos(float x, float y, float z);
    // 设置镜头看向的位置
    void setViewDesPos(float x, float y, float z);
    // 旋转镜头 参数为true向右旋转 参数为false向左旋转
    void rotateViewDes(bool turnRight);
    // 检测用户是否按下键盘 并对摄像机进行设置
    void setCamera(int key);

    /* ******************** 各部件绘制函数 ******************** */

    void drawCashier();
    void drawRestaurant();
    void drawSkybox();
    void drawDesk();
    void drawChair();

    /* ******************** 基本形状绘制函数 ******************** */

private:
    // 私有函数 由其他draw函数调用 实现统一的图形绘制功能
    void _drawShape(GLuint givenVAO, GLenum mod, GLsizei count,
        float tx, float ty, float tz, float sx, float sy, float sz,
        float angle, float rx, float ry, float rz);

protected:
    // 根据指定的位移和缩放绘制长方体
    void drawCuboid(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
    // 根据指定的位移和缩放绘制半圆柱体
    void drawSemicylinder(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
    // 根据指定的位移和缩放绘制圆柱体
    void drawCylinder(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
    // 根据指定的位移绘制圆台体(较缓)
    void drawFrustum1(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
    // 根据指定的位移绘制圆台体(较陡)
    void drawFrustum2(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
    // 绘制圆锥
    void drawCone(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
    // 绘制半圆环
    void drawTorus(float innerRadius, float OutRadius,
        float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
        float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);

    /* ******************** 初始化相关函数 ******************** */

    // 初始化cubeVBO和cubeVAO 保存了绘制正方体的信息
    void initCube();
    // 初始化semicylinderVBO和semicylinderVAO 保存了绘制半圆柱体的信息
    void initSemicylinder();
    // 初始化cylinderVBO和cylinderVAO 保存了绘制圆柱体的信息
    void initCylinder();
    // 初始化coneVBO和coneVAO 保存了绘制圆锥体的信息
    void initCone();
    // 初始化givenVAO和givenVBO 保存了绘制上圆半径upperRadius下圆半径lowerRadius的圆台的信息
    void initFrustum(GLuint& givenVAO, GLuint& givenVBO, int& vertexNum,
        float upperRadius = 1.0f, float lowerRadius = 1.0f, float height = 1.0f);
    // 初始化torusVBO和torusVAO 并将半径和VAO的映射关系保存到radius2torusVAO中
    void initTorus(float innerRadius, float outRadius, const int SIDES = 100, const int RINGS = 100);
    // 初始化绘制餐厅所需信息
    void initRestaurant();
    // 初始化绘制天空盒所需信息
    void initSkybox();

private:
    QTimer* timer;

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

    /* ******************** 纹理贴图相关 ******************** */

    // 控制是否使用漫反射纹理贴图 1为使用 0为不使用
    GLint useTexture = 0;
    // 记录所有图像路径和纹理贴图的映射关系
    std::unordered_map<const char*, GLuint> imgpath2texture;
    // 记录所有图像路径和编号(即第几个图像)的映射关系
    std::unordered_map<const char*, int> imgpath2index;

    /* ******************** 形状顶点信息 ******************** */

    GLuint cubeVBO;         // 保存了绘制正方体的信息 包括顶点位置和法向量
    GLuint cubeVAO;         // 保存了绘制正方体的信息 用于管理顶点属性的状态
    GLuint semicylinderVBO; // 保存了绘制半圆柱体的信息 包括顶点位置和法向量
    GLuint semicylinderVAO; // 保存了绘制半圆柱体的信息 用于管理顶点属性的状态

    GLuint quadsVBO;    // 保存了绘制餐厅的信息 包括顶点位置和法向量
    GLuint quadsVAO;    // 保存了绘制餐厅的信息 用于管理顶点属性的状态
    GLuint skyboxVAO;   // 保存了绘制天空盒的信息 包括顶点位置和法向量
    GLuint skyboxVBO;   // 保存了绘制天空盒的信息 用于管理顶点属性的状态

    GLuint cylinderVBO; // 保存了绘制圆柱体的信息 包括顶点位置和法向量
    GLuint cylinderVAO; // 保存了绘制圆柱体的信息 用于管理顶点属性的状态
    GLuint coneVBO;     // 保存了绘制圆锥的信息 包括顶点位置和法向量
    GLuint coneVAO;     // 保存了绘制圆锥的信息 用于管理顶点属性的状态
    GLuint frustumVBO1; // 保存了绘制圆台(较缓)的信息 包括顶点位置和法向量
    GLuint frustumVAO1; // 保存了绘制圆台(较缓)的信息 用于管理顶点属性的状态
    GLuint frustumVBO2; // 保存了绘制圆台(较陡)的信息 包括顶点位置和法向量
    GLuint frustumVAO2; // 保存了绘制圆台(较陡)的信息 用于管理顶点属性的状态

    int vertexNumOfSemicylinder;    // 绘制半圆柱体所需的顶点数量
    int vertexNumOfCylinder;        // 绘制圆柱体所需的顶点数量
    int vertexNumOfCone;            // 绘制圆锥所需的顶点数量
    int vertexNumOfFrustum1;        // 绘制圆台(较缓)所需的顶点数量
    int vertexNumOfFrustum2;        // 绘制圆台(较陡)所需的顶点数量

    // 记录环半径（由innerRadius * OutEadius得到key）到半圆环的VAO的映射关系
    std::unordered_map<float, GLuint> radius2torusVAO;

    /* ******************** 状态信息 ******************** */

    float lightPos[3] = { 50.0f, 50.0f, 50.0f };      // 光源位置
    float viewPos[3] = { 0.0f, 100.0f, 700.0f };     // 观察位置
    float objectColor[3] = { 1.0f, 0.5f, 0.31f };   // 物体颜色
    float lightColor[3] = { 1.0f, 1.0f, 1.0f };     // 光源颜色
    float viewDesPos[3] = { 0.0f, 100.0f, 700.0f - 1.0f };  // 摄像机看向的位置 初始看向z轴负方向
    // 标识看向的方向 有 0 - 7 共八种情况
    // 0表示看向z轴负方向 接下来每增加1向右旋转45度
    int viewFlag = 0;
    // xRate 和 zRate 用来判断当前向右移动时坐标变换的方向
    // 如 xRate 为 1.0 且 zRate 为 0.0 时 向右移动即向x轴正方向移动
    // 当 xRate 为 0.7 且 zRate 为 0.7 时 向右移动即向x轴和z轴正方向的夹角方向移动
    float xRate = 1.0f;
    float zRate = 0.0f;

    /* ******************** 其他参数 ******************** */

    const float ITERATIONS = 10;    // 细分三角形迭代次数
    const float PI = 3.14;

    // initShader函数中使用到的变量 和读取文件相关
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    std::stringstream vertexShaderStream;
    std::stringstream fragmentShaderStream;
};
#endif // __MYGLWIDGET_H__