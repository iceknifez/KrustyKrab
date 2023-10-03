#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#ifdef MAC_OS
#include <QtOpenGL/QtOpenGL>
#else
#include <GL/glew.h>
#endif
#include <QtGui>
#include <QOpenGLWidget>
//#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QGLWidget>


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <time.h>
#include <conio.h>

#include "shape_coordinate.h"

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    MyGLWidget(QWidget* parent = nullptr);
    ~MyGLWidget();
    void keyPressEvent(QKeyEvent* event);   // 键盘事件

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    // 根据指定的文件中的内容初始化顶点着色器和片元着色器
    void initShader(std::string vertexPath, std::string fragmentPath);
    // 更新着色器的信息 每次绘制前调用
    void updateShader();
    // 设置漫反射纹理贴图
    void setDiffuseMap(const char* imagePath);

    /* *********************** */

    // 绘制船体和收银台
    void drawCashier();
    // 根据指定的位移和缩放绘制长方体
    void drawCuboid(float tx, float ty, float tz, float sx, float sy, float sz,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
    // 根据指定的位移和缩放绘制半圆柱体
    void drawSemicylinder(float tx, float ty, float tz, float sx, float sy, float sz,
        float angle = 0.0f, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
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
    // 初始化cubeVBO和cubeVAO 保存了绘制正方体的信息
    void initCube();
    // 初始化semicylinderVBO和semicylinderVAO 保存了绘制半圆柱体的信息
    void initSemicylinder();
    // 细分得到半圆 结果保存在semicircleVertices数组中
    void subdivideSemicircle(int depth, std::vector<GLfloat>& v);
    // 通过两个原有顶点位置计算得到新的中点位置
    void getHalf(GLfloat v1[], GLfloat v2[], GLfloat v12[], GLfloat scale = 1.0f);
    // 将三角形v1v2v3的顶点信息添加到vertices数组中
    void addTriangle(GLfloat v1[], GLfloat v2[], GLfloat v3[], std::vector<GLfloat>& v);
    void addVertex(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& v);

    /* *********************** */

    /* *********************** */

    void drawRestaurant();
    void initRestaurant();
    // 绘制半圆环
    void drawTorus(float innerRadius, float OutRadius);
    // 绘制天空盒
    void DrawSkybox();
    // 初始化绘制天空盒所需信息
    void initSkybox();

    /* *********************** */

    /* *********************** */

    // 绘制桌子
    void drawDesk();
    // 绘制椅子
    void drawChair();
    // 根据指定的位移绘制椭圆柱体
    void drawOval(float tx, float ty, float tz, float angle, float rx, float ry, float rz);
    // 根据指定的位移绘制圆台体
    void drawCone(float tx, float ty, float tz, float angle, float rx, float ry, float rz);
    // 根据指定的位移和缩放绘制圆柱体
    void drawCylinder(float tx, float ty, float tz, float sx, float sy, float sz,
        float angle, float rx, float ry, float rz);
    // 绘制圆锥
    void drawTaper(float tx, float ty, float tz, float angle, float rx, float ry, float rz);

    // 初始化cylinderVBO 该vbo保存了绘制圆柱体的信息
    void initCylinderVBO();
    // 细分得到圆 结果保存在circleVertices数组中
    void subdivideCircle(int depth, std::vector<GLfloat>& v, float length);

    // 初始化taperVBO 该vbo保存了绘制圆锥体的信息
    void initTaperVBO();
    // 初始化ovalVBO 该vbo保存了绘制椭圆体的信息
    void initOvalVBO();
    // 初始化coneVBO 该vbo保存了绘制圆台的信息
    void initConeVBO();

    /* *********************** */

private:
    QTimer* timer;
    // 你可以将你的shader或者vbo声明为私有变量
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
    // 漫反射纹理贴图
    GLuint diffuseMap;
    // 控制是否使用漫反射纹理贴图 1为使用 0为不使用
    GLint useTexture = 0;

    /* *********************** */

    GLuint cubeVBO; // 保存了绘制正方体的信息 包括顶点位置和法向量
    GLuint cubeVAO; // 保存了绘制正方体的信息 用于管理顶点属性的状态
    GLuint semicylinderVBO; // 保存了绘制半圆柱体的信息 包括顶点位置和法向量
    GLuint semicylinderVAO; // 保存了绘制半圆柱体的信息 用于管理顶点属性的状态
    std::vector<GLfloat> semicircleVertices;    // 保存了半圆顶点位置
    std::vector<GLfloat> semicylinderVertices;  // 保存了半圆柱体顶点信息 包括位置和法向量
    int vertexNumOfSemicylinder;    // 绘制半圆柱体所需的顶点数量

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
    // 细分三角形迭代次数
    const float ITERATIONS = 10;

    /* *********************** */

    /* *********************** */

    const float PI = 3.14;
    GLuint VBOOutButtomId;
    GLuint VAOOutButtomId;
    GLuint quadsVBO;
    GLuint quadsVAO;

    GLuint skyboxVAO;
    GLuint skyboxVBO;

    /* *********************** */
    int angleSky;
    /* *********************** */

    GLuint cylinderVBO; // 保存了绘制圆柱体的信息 包括顶点位置和法向量
    GLuint taperVBO; // 保存了绘制圆锥的信息，包括顶点位置和法向量
    GLuint ovalVBO; // 保存了绘制椭圆柱的信息，包括顶点位置和法向量
    GLuint coneVBO; // 保存了绘制圆台的信息，包括顶点位置和法向量
    std::vector<GLfloat> circleVertices;    // 保存了圆顶点位置
    std::vector<GLfloat> cylinderVertices;  // 保存了圆柱体顶点信息 包括位置和法向量
    std::vector<GLfloat> taperVertices;     // 保存了圆锥顶点信息 包括位置和法向量
    std::vector<GLfloat> taperdownVertices; // 保存了圆锥底部圆顶点位置
    std::vector<GLfloat> ovalupVertices;    // 保存了椭圆顶部圆顶点位置信息
    std::vector<GLfloat> ovaldownVertices;  // 保存了椭圆底部圆顶点位置信息
    std::vector<GLfloat> ovalVertices;      // 保存了椭圆体定点信息 包括位置和法向量
    std::vector<GLfloat> coneupVertices;    // 保存了圆台顶部圆顶点位置信息
    std::vector<GLfloat> conedownVertices;  // 保存了圆台底部圆顶点位置信息
    std::vector<GLfloat> coneVertices;      // 保存了圆台体定点信息 包括位置和法向量
    int vertexNumOfCylinder;    // 绘制圆柱体所需的顶点数量
    int vertexNumOfOval;    // 绘制椭圆柱体所需的顶点数量
    int vertexNumOfCone; // 绘制圆台所需的顶点数量

    /* *********************** */

    // initShader函数中使用到的变量 和读取文件相关
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;
    std::stringstream vertexShaderStream;
    std::stringstream fragmentShaderStream;
};
#endif // MYGLWIDGET_H
