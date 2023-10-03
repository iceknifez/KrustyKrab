#include "myglwidget.h"
#include <QtCore/qdir.h>
#include <qopenglextrafunctions.h>
using namespace std;

/*###################################################
##  函数: MyGLWidget
##  函数描述： MyGLWidget类的构造函数，实例化定时器timer
##  参数描述：
##  parent: MyGLWidget的父对象
#####################################################*/

MyGLWidget::MyGLWidget(QWidget* parent)
	:QOpenGLWidget(parent)
{
	resize(1280, 800);
	timer = new QTimer(this); // 实例化一个定时器
	timer->start(16); // 时间间隔设置为16ms，可以根据需要调整
	connect(timer, SIGNAL(timeout()), this, SLOT(update())); // 连接update()函数，每16ms触发一次update()函数进行重新绘图
}


/*###################################################
##  函数: ~MyGLWidget
##  函数描述： ~MyGLWidget类的析构函数，删除timer
##  参数描述： 无
#####################################################*/
MyGLWidget::~MyGLWidget()
{
	delete this->timer;
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
	setCamera(event->key());
}


/*###################################################
##  函数: initializeGL
##  函数描述： 初始化绘图参数，如视窗大小、背景色等
##  参数描述： 无
#####################################################*/
void MyGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	// vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Your Implementation

	glClearDepth(1.0f); //设置深度缓存

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// 设置正交投影
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float range = 45.0f;
	gluPerspective(range, 1.0 * width() / height(), 0.1f, 2000.0f);

	// 初始化着色器和VBO
	initShader("vertexShader.glsl", "fragmentShader.glsl");
	initCubeVBO();
	initSemicylinderVBO();

	initCylinderVBO();
	initTaperVBO();
	initOvalVBO();
	initConeVBO();

	initRestaurant();
	initSkybox();

	// 创建漫反射纹理贴图并进行绑定
	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, width(), height());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	angleSky = 0;
}

int angle = 0;	// 临时变量 用来呈现旋转效果
/*###################################################
##  函数: paintGL
##  函数描述： 绘图函数，实现图形绘制，会被update()函数调用
##  参数描述： 无
#####################################################*/
void MyGLWidget::paintGL()
{
	// Your Implementation
	// 检测用户是否移动摄像机 设置摄像机位置
	setCamera(0);
	updateShader();

	//glUseProgram(0);
	// 绘制船体和收银台
	glPushMatrix();
	//glTranslatef(0.0f, 7.5f, -33.0f);
	//glScalef(1.5f, 1.5f, 1.5f);
	glTranslatef(0.0f, 10.0f, -14.0f);
	glScalef(2.0f, 2.0f, 2.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	drawCashier();
	glPopMatrix();

	// 绘制地板
	//setObjectColor(135 / 255.0, 176 / 255.0, 111 / 255.0);
	useTexture = 1;
	//setDiffuseMap("img/floor.png");
	drawCuboid(0.0f, -0.5f, 0.0f, 480.0f, 1.0f, 500.0f);
	// 绘制餐厅内墙面	
	//setDiffuseMap("img/wallpaper1.png");
	drawCuboid(0.0f, 75.0f, -90.5f, 360.0f, 150.0f, 1.0f);
	//setDiffuseMap("img/wallpaper2.png");
	drawCuboid(0.0f, 150.5f, -140.0f, 360.0f, 1.0f, 100.0f);

	// 绘制杂物箱
	//setDiffuseMap("img/box.png");
	drawCuboid(-80.0f, 161.0f, -140.0f, 30.0f, 20.0f, 30.0f);
	drawCuboid(100.0f, 161.0f, -140.0f, 35.0f, 20.0f, 35.0f, 45.0f, 0.0f, 1.0f, 0.0f);
	drawCuboid(105.0f, 181.0f, -140.0f, 30.0f, 20.0f, 30.0f, 30.0f, 0.0f, 1.0f, 0.0f);
	useTexture = 0;

	// 绘制餐厅
	glPushMatrix();
	glScalef(6.0f, 3.0f, 10.0f);
	glTranslatef(0.0f, 0.0f, 5.0f);
	drawRestaurant();
	glPopMatrix();

	// 绘制桌椅
	// 左后
	glPushMatrix();
	glTranslatef(-100.0f, 1.0f, -10.0f);
	drawDesk();
	glTranslatef(-20.0f, 0.0f, 0.0f);
	drawChair();
	glTranslatef(35.0f, 0.0f, -15.0f);
	drawChair();
	glPopMatrix();
	// 左前
	glPushMatrix();
	glTranslatef(-60.0f, 1.0f, 70.0f);
	drawDesk();
	glTranslatef(0.0f, 0.0f, 20.0f);
	drawChair();
	glTranslatef(-15.0f, 0.0f, -35.0f);
	drawChair();
	glTranslatef(30.0f, 0.0f, 0.0f);
	drawChair();
	glPopMatrix();
	// 右后
	glPushMatrix();
	glTranslatef(100.0f, 1.0f, -10.0f);
	drawDesk();
	glTranslatef(0.0f, 0.0f, 20.0f);
	drawChair();
	glTranslatef(-15.0f, 0.0f, -35.0f);
	drawChair();
	glPopMatrix();
	// 右前
	glPushMatrix();
	glTranslatef(60.0f, 1.0f, 70.0f);
	drawDesk();
	glTranslatef(20.0f, 0.0f, 0.0f);
	drawChair();
	glTranslatef(-35.0f, 0.0f, -15.0f);
	drawChair();
	glPopMatrix();

	// 绘制天空盒
	useTexture = 1;	// 使用漫反射纹理贴图
	//setDiffuseMap("img/skybox1.png");
	glPushMatrix();
	//glTranslatef(0.0f, 349.0f, 0.0f);
	glTranslatef(viewPos[0], viewPos[1], viewPos[2]);
	glScalef(1410.0f, 700.0f, 1610.0f);
	glRotatef(angleSky * 43, 0.0f, 1.0f, 0.0f);
	DrawSkybox();
	glPopMatrix();
	useTexture = 0;

	// 绘制玻璃
	useTexture = 1;
	setDiffuseMap("img/glass.png");
	drawCuboid(6 * 22.0f, 90.0f, 187.5f, 6 * 16.0f, 120.0f, 1.0f);	// 右前
	drawCuboid(-6 * 22.0f, 90.0f, 187.5f, 6 * 16.0f, 120.0f, 1.0f);	// 左前
	drawCuboid(0.0f, 90.0f, -187.5f, 360.0f, 120.0f, 1.0f);			// 后方
	drawCuboid(200.0f, 90.0f, 0.0f, 1.0f, 120.0f, 300.0f);			// 右边
	drawCuboid(-200.0f, 90.0f, 0.0f, 1.0f, 120.0f, 300.0f);			// 左边
	useTexture = 0;
}


/*###################################################
##  函数: resizeGL
##  函数描述： 当窗口大小改变时调整视窗尺寸
##  参数描述： 无
#####################################################*/
void MyGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);

	// 调整窗口时保持画面比例不变
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float range = 45.0f;
	gluPerspective(range, 1.0 * width / height, 0.1f, 2000.0f);

	update();
}

void MyGLWidget::initShader(std::string vertexPath, std::string fragmentPath)
{
	// 打开指定的文件
	vertexShaderFile.open(vertexPath);
	fragmentShaderFile.open(fragmentPath);

	// 读取文件的缓冲内容到流中
	vertexShaderStream << vertexShaderFile.rdbuf();
	fragmentShaderStream << fragmentShaderFile.rdbuf();

	// 关闭文件
	vertexShaderFile.close();
	fragmentShaderFile.close();

	// 转换成将string
	std::string vertexCode = vertexShaderStream.str();
	std::string fragmentCode = fragmentShaderStream.str();

	// 将string转换成char*
	const char* vertexShaderData = vertexCode.c_str();
	const char* fragmentShaderData = fragmentCode.c_str();

	// 创建顶点着色器和片元着色器
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderData, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderData, NULL);
	glCompileShader(fragmentShader);

	// 创建一个着色程序 并把顶点着色器和片段着色器附加到该程序上
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// 绑定完成后即可删除着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(program);
}

void MyGLWidget::updateShader()
{
	// 启用着色程序
	//glUseProgram(program);

	// 获取当前的 modelview 和 projection 矩阵
	GLfloat modelview[16];
	GLfloat projection[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	glGetFloatv(GL_PROJECTION_MATRIX, projection);

	// 向顶点着色器传递当前的 modelview 和 projection 矩阵
	GLint modelviewLoc = glGetUniformLocation(program, "modelview");
	glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview);

	GLint projectionLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection);

	// 向片元着色器传递光源位置
	//GLint lightPosLoc = glGetUniformLocation(program, "lightPos");
	GLint lightPosLoc = glGetUniformLocation(program, "light.position");
	glUniform3f(lightPosLoc, lightPos[0], lightPos[1], lightPos[2]);

	// 向片元着色器传递观察位置
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	glUniform3f(viewPosLoc, viewPos[0], viewPos[1], viewPos[2]);

	// 向片元着色器传递物体颜色
	GLint objectColorLoc = glGetUniformLocation(program, "objectColor");
	glUniform3f(objectColorLoc, objectColor[0], objectColor[1], objectColor[2]);

	// 向片元着色器传递光源颜色
	GLint lightColorLoc = glGetUniformLocation(program, "lightColor");
	glUniform3f(lightColorLoc, lightColor[0], lightColor[1], lightColor[2]);

	GLint matAmbientLoc = glGetUniformLocation(program, "material.ambientColor");
	GLint matDiffuseLoc = glGetUniformLocation(program, "material.diffuseColor");
	GLint matSpecularLoc = glGetUniformLocation(program, "material.specular");
	GLint matShineLoc = glGetUniformLocation(program, "material.shininess");

	glUniform3f(matAmbientLoc, objectColor[0], objectColor[1], objectColor[2]);
	glUniform3f(matDiffuseLoc, objectColor[0], objectColor[1], objectColor[2]);
	glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
	glUniform1f(matShineLoc, 32.0f);

	GLint lightAmbientLoc = glGetUniformLocation(program, "light.ambient");
	GLint lightDiffuseLoc = glGetUniformLocation(program, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(program, "light.specular");

	//glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
	//glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	//glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightAmbientLoc, 0.4f, 0.4f, 0.4f);
	glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(lightSpecularLoc, 0.5f, 0.5f, 0.5f);

	glUniform1i(glGetUniformLocation(program, "material.diffuse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	glUniform1i(glGetUniformLocation(program, "useTexture"), useTexture);
}

void MyGLWidget::setDiffuseMap(const char* imagePath)
{
	QImage image = QImage(imagePath);
	// 将QT读取的图像转化为OpenGL纹理需要的格式
	// 比如把bgr换成rgb的形式 以及对图像进行一次翻转等等
	image = QGLWidget::convertToGLFormat(image);

	// 获取图片的高度和宽度
	int width, height;
	width = image.width();
	height = image.height();

	// 为当前绑定的纹理对象 即diffuseMap 附加上纹理图像
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
	glGenerateMipmap(GL_TEXTURE_2D);

	// 设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 设置多级渐远纹理过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyGLWidget::drawCashier()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 船体
	useTexture = 1;	// 使用漫反射纹理贴图
	//setDiffuseMap("img/boat1.png");
	drawCuboid(8 + 30 - 0.5f, 0.0f, 0.0f, 1.0f, 10.0f, 16.0f);	// 后钢板
	drawCuboid(8 + 0.5f, 0.0f, 0.0f, 1.0f, 10.0f, 16.0f);		// 前钢板
	drawCuboid(8 + 15.0f, 0.0f, 7.5f, 28.0f, 10.0f, 1.0f);		// 左钢板
	drawCuboid(8 + 15.0f, 0.0f, -7.5f, 28.0f, 10.0f, 1.0f);		// 右钢板
	drawCuboid(8 + 15.0f, -4.5f, 0.0f, 28.0f, 1.0f, 14.0f);		// 底板
	//setDiffuseMap("img/boat1_rotate.png");
	drawSemicylinder(8.0f, 0.0f, 0.0f, 8.0f, 5.0f, 8.0f);	// 船头
	//setDiffuseMap("img/boat2.png");
	drawCuboid(0.0f, 1.0f, 0.0f, 2.0f, 12.0f, 2.0f);		// 船头柱子

	setObjectColor(0.76f, 0.82f, 0.94f);	// 将颜色设置为浅蓝色 (193, 210, 240)
	drawCuboid(8 + 30 - 1.5f, 3.5f, 0.0f, 1.0f, 1.0f, 14.0f);		// 后突起
	drawCuboid(8 + 1.5f, 3.5f, 0.0f, 1.0f, 1.0f, 14.0f);			// 前突起
	drawCuboid(8 + 15.0f, 3.5f, 7.5f - 1.0f, 26.0f, 1.0f, 1.0f);	// 左突起
	drawCuboid(8 + 15.0f, 3.5f, -7.5f + 1.0f, 26.0f, 1.0f, 1.0f);	// 右突起

	drawCuboid(8 + 30 - 4.5f, 1.0f, 0.0f, 8.0f, 1.0f, 14.0f);	// 尾部架子

	// 收银台
	// setObjectColor(0.1f, 0.2f, 0.2f);	// 将收银台颜色设置为偏蓝绿的深灰色
	//setDiffuseMap("img/cashier.png");
	drawCuboid(8.0f - 2.0f, 6.0f, 0.0f, 4.0f, 2.0f, 8.0f);	// 平放结构
	drawCuboid(8.0f - 5.0f, 7.0f, 0.0f, 2.0f, 4.0f, 8.0f);	// 直立结构
	drawCuboid(8.0f - 5.0f, 9.5f, 0.0f, 1.0f, 1.0f, 1.0f);	// 小直立结构
	drawCuboid(8.0f - 5.0f, 10.5f, 0.0f, 1.0f, 1.0f, 3.0f);	// 小平放结构
	useTexture = 0; // 不使用漫反射纹理贴图
}

void MyGLWidget::drawCuboid(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glScalef(sx, sy, sz);
	glRotatef(angle, rx, ry, rz);
	updateShader();

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	// 指定顶点属性数组的数据格式和位置
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	glPopMatrix();
}

void MyGLWidget::drawSemicylinder(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glScalef(sx, sy, sz);
	glRotatef(angle, rx, ry, rz);
	updateShader();

	glBindBuffer(GL_ARRAY_BUFFER, semicylinderVBO);

	// 指定顶点属性数组的数据格式和位置
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, vertexNumOfSemicylinder);
	glPopMatrix();
}

void MyGLWidget::setObjectColor(float r, float g, float b)
{
	objectColor[0] = r;
	objectColor[1] = g;
	objectColor[2] = b;
}

void MyGLWidget::setViewPos(float x, float y, float z)
{
	viewPos[0] = x;
	viewPos[1] = y;
	viewPos[2] = z;
}

void MyGLWidget::setViewDesPos(float x, float y, float z)
{
	viewDesPos[0] = x;
	viewDesPos[1] = y;
	viewDesPos[2] = z;
}

void MyGLWidget::rotateViewDes(bool turnRight)
{
	if (turnRight) viewFlag = (viewFlag + 1) % 8;	// 向右转动镜头
	else viewFlag = (viewFlag + 7) % 8;				// 向左转动镜头

	switch (viewFlag)
	{
		// 每个case先设置相机新朝向 再设置当前向"右"移动时的具体方向
	case 0: setViewDesPos(viewPos[0], viewPos[1], viewPos[2] - 1.0f); 			// z轴负方向
		xRate = 1.0f; zRate = 0.0f; break;
	case 1: setViewDesPos(viewPos[0] + 1.0f, viewPos[1], viewPos[2] - 1.0f);	// z轴负方向x轴正方向夹角
		xRate = 0.7f; zRate = 0.7f; break;
	case 2: setViewDesPos(viewPos[0] + 1.0f, viewPos[1], viewPos[2]);			// x轴正方向
		xRate = 0.0f; zRate = 1.0f; break;
	case 3: setViewDesPos(viewPos[0] + 1.0f, viewPos[1], viewPos[2] + 1.0f);	// z轴正方向x轴正方向夹角
		xRate = -0.7f; zRate = 0.7f; break;
	case 4: setViewDesPos(viewPos[0], viewPos[1], viewPos[2] + 1.0f);			// z轴正方向
		xRate = -1.0f; zRate = 0.0f; break;
	case 5: setViewDesPos(viewPos[0] - 1.0f, viewPos[1], viewPos[2] + 1.0f);	// z轴正方向x轴负方向夹角
		xRate = -0.7f; zRate = -0.7f; break;
	case 6: setViewDesPos(viewPos[0] - 1.0f, viewPos[1], viewPos[2]);			// x轴负方向
		xRate = 0.0f; zRate = -1.0f; break;
	case 7: setViewDesPos(viewPos[0] - 1.0f, viewPos[1], viewPos[2] - 1.0f);	// z轴负方向x轴负方向夹角
		xRate = 0.7f; zRate = -0.7f; break;
	default: break;
	}
}

void MyGLWidget::setCamera(int key)
{
	// 上下左右移动相机
	float speed = 20.0f;
	//printf("%d\n", ch);
	float xRate2 = -zRate, zRate2 = xRate; // 向后的方向 即向右的方向顺时针旋转90度
	switch (key)
	{
	// 上下左右方向键的功能
	// 镜头向上移动
	case Qt::Key_Up: setViewPos(viewPos[0], viewPos[1] + speed, viewPos[2]);
		setViewDesPos(viewDesPos[0], viewDesPos[1] + speed, viewDesPos[2]); break;
	// 镜头向下移动
	case Qt::Key_Down: setViewPos(viewPos[0], viewPos[1] - speed, viewPos[2]);
		setViewDesPos(viewDesPos[0], viewDesPos[1] - speed, viewDesPos[2]); break;
	// 镜头向左移动
	case Qt::Key_Left: setViewPos(viewPos[0] - speed * xRate, viewPos[1], viewPos[2] - speed * zRate);
		setViewDesPos(viewDesPos[0] - speed * xRate, viewDesPos[1], viewDesPos[2] - speed * zRate); break;
	// 镜头向右移动
	case Qt::Key_Right: setViewPos(viewPos[0] + speed * xRate, viewPos[1], viewPos[2] + speed * zRate);
		setViewDesPos(viewDesPos[0] + speed * xRate, viewDesPos[1], viewDesPos[2] + speed * zRate); break;
	// wsad键的功能
	// 镜头向前移动
	case Qt::Key_W: setViewPos(viewPos[0] - speed * xRate2, viewPos[1], viewPos[2] - speed * zRate2);
		setViewDesPos(viewDesPos[0] - speed * xRate2, viewDesPos[1], viewDesPos[2] - speed * zRate2); break;
	// 镜头向后移动
	case Qt::Key_S: setViewPos(viewPos[0] + speed * xRate2, viewPos[1], viewPos[2] + speed * zRate2);
		setViewDesPos(viewDesPos[0] + speed * xRate2, viewDesPos[1], viewDesPos[2] + speed * zRate2); break;
	case Qt::Key_A: rotateViewDes(false); angleSky++; break;	// 镜头向左旋转45度
	case Qt::Key_D: rotateViewDes(true); angleSky--; break;	// 镜头向右旋转45度
	case Qt::Key_Q: close();
	default: break;
	}

	// 设置摄像机位置
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(viewPos[0], viewPos[1], viewPos[2], viewDesPos[0], viewDesPos[1], viewDesPos[2], 0, 1, 0);
}

void MyGLWidget::initCubeVBO()
{
	// 为VBO创建一个缓冲区对象
	glGenBuffers(1, &cubeVBO);
	// 利用数组中的内容更新VBO的信息
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	// 指定顶点属性数组的数据格式和位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 解绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyGLWidget::initSemicylinderVBO()
{
	// 两个三角形 作为半圆迭代的开始
	GLfloat temp[2 * 3 * 3] =
	{
		 0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,

		 0.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,
	};

	// 将temp中的顶点信息保存到semisircleVertices中
	semicircleVertices.resize(2 * 3 * 3);
	for (int i = 0; i < 2 * 3 * 3; i++)
	{
		semicircleVertices[i] = temp[i];
	}

	// 迭代10次 函数调用完成后semicircleVertices中保存了绘制半圆的顶点信息
	// 半圆由许多细分三角形组成 每个三角形以原点为起点 按顺时针方向保存顶点信息
	subdivideSemicircle(ITERATIONS, semicircleVertices);

	// 括号中的1 1 2分别为上半圆 下半圆 侧面曲面的顶点数量 6为侧面矩形顶点数量
	vertexNumOfSemicylinder = semicircleVertices.size() / 3 * (1 + 1 + 2) + 6;

	std::vector<GLfloat> upperVertices;	// 上半圆
	std::vector<GLfloat> lowerVertices;	// 下半圆
	std::vector<GLfloat> sideVertices;	// 侧面

	// 每个细分三角形靠近原点那个角的角度
	float deltaAngle = 180.0 / (semicircleVertices.size() / 9);

	for (int i = 0; i < semicircleVertices.size() / 3; i++)
	{
		// 顶点位置
		upperVertices.push_back(semicircleVertices[i * 3]);
		upperVertices.push_back(semicircleVertices[i * 3 + 1] + 1.0f);
		upperVertices.push_back(semicircleVertices[i * 3 + 2]);
		// 法向量 竖直向上
		upperVertices.push_back(0.0f);
		upperVertices.push_back(1.0f);
		upperVertices.push_back(0.0f);
		// 纹理坐标
		if (i % 3 == 0)
		{
			upperVertices.push_back(0.5f);
			upperVertices.push_back(0.0f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 180.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
			upperVertices.push_back(sin(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 180.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
			upperVertices.push_back(sin(theta * PI / 180));
		}

		// 顶点位置
		lowerVertices.push_back(semicircleVertices[i * 3]);
		lowerVertices.push_back(semicircleVertices[i * 3 + 1] - 1.0f);
		lowerVertices.push_back(semicircleVertices[i * 3 + 2]);
		// 法向量 竖直向下
		lowerVertices.push_back(0.0f);
		lowerVertices.push_back(-1.0f);
		lowerVertices.push_back(0.0f);
		// 纹理坐标
		if (i % 3 == 0)
		{
			lowerVertices.push_back(0.5f);
			lowerVertices.push_back(0.0f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 180.0 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
			lowerVertices.push_back(sin(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 180.0 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
			lowerVertices.push_back(sin(theta * PI / 180));
		}
	}

	// 设置侧面中曲面相关的顶点信息
	// 上下两个细分三角形的原点对边 构成一个侧面细分长方形
	int numOfTriangles = semicircleVertices.size() / 3 / 3;
	for (int i = 0; i < numOfTriangles; i++)
	{
		// 记上面的细分三角形为a 下面的细分三角形为b
		// v1 v2 和 v3 分别为原细分三角形的三个顶点
		GLfloat v1[3], v2[3], v3[3];
		GLfloat va1[3], va2[3], va3[3], vb1[3], vb2[3], vb3[3];
		for (int j = 0; j < 3; j++)
		{
			v1[j] = semicircleVertices[i * 9 + j];
			v2[j] = semicircleVertices[i * 9 + j + 3];
			v3[j] = semicircleVertices[i * 9 + j + 6];

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
		sideVertices.push_back(1.0f);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);	// 位置
		addVertex(v2[0], v2[1], v2[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(0.0f);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		addVertex(va3[0], va3[1], va3[2], sideVertices);	// 位置
		addVertex(v3[0], v3[1], v3[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(1.0f);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));

		addVertex(va3[0], va3[1], va3[2], sideVertices);	// 位置
		addVertex(v3[0], v3[1], v3[2], sideVertices);		// 法向量
		// 纹理坐标
		sideVertices.push_back(1.0f);
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

	// 设置侧面中平面相关的顶点信息
	addVertex(0.0f, 1.0f, -1.0f, sideVertices);		// 位置
	addVertex(1.0f, 0.0f, 0.0f, sideVertices);		// 法向量
	sideVertices.push_back(1.0f); sideVertices.push_back(1.0f);	// 纹理坐标
	addVertex(0.0f, 1.0f, 1.0f, sideVertices);		// 位置
	addVertex(1.0f, 0.0f, 0.0f, sideVertices);		// 法向量
	sideVertices.push_back(1.0f); sideVertices.push_back(0.0f);	// 纹理坐标
	addVertex(0.0f, -1.0f, -1.0f, sideVertices);	// 位置
	addVertex(1.0f, 0.0f, 0.0f, sideVertices);		// 法向量
	sideVertices.push_back(0.0f); sideVertices.push_back(1.0f);	// 纹理坐标

	addVertex(0.0f, -1.0f, -1.0f, sideVertices);	// 位置
	addVertex(1.0f, 0.0f, 0.0f, sideVertices);		// 法向量
	sideVertices.push_back(0.0f); sideVertices.push_back(1.0f);	// 纹理坐标
	addVertex(0.0f, 1.0f, 1.0f, sideVertices);		// 位置
	addVertex(1.0f, 0.0f, 0.0f, sideVertices);		// 法向量
	sideVertices.push_back(1.0f); sideVertices.push_back(0.0f);	// 纹理坐标
	addVertex(0.0f, -1.0f, 1.0f, sideVertices);		// 位置
	addVertex(1.0f, 0.0f, 0.0f, sideVertices);		// 法向量
	sideVertices.push_back(0.0f); sideVertices.push_back(0.0f);	// 纹理坐标

	semicylinderVertices.insert(semicylinderVertices.end(), upperVertices.begin(), upperVertices.end());
	semicylinderVertices.insert(semicylinderVertices.end(), lowerVertices.begin(), lowerVertices.end());
	semicylinderVertices.insert(semicylinderVertices.end(), sideVertices.begin(), sideVertices.end());

	// 为VBO创建一个缓冲区对象
	glGenBuffers(1, &semicylinderVBO);
	// 利用vertices数组中的内容更新VBO的信息
	glBindBuffer(GL_ARRAY_BUFFER, semicylinderVBO);
	glBufferData(GL_ARRAY_BUFFER, semicylinderVertices.size() * sizeof(GLfloat), &semicylinderVertices[0], GL_STATIC_DRAW);
	// 指定顶点属性数组的数据格式和位置
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 解绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyGLWidget::subdivideSemicircle(int depth, std::vector<GLfloat>& vertices)
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
		getHalf(v2, v3, v23);

		// 将顶点信息添加到vertices数组中
		addTriangle(v1, v2, v23, vertices);
		addTriangle(v1, v23, v3, vertices);
	}
	// 深度减1 进行下一轮迭代
	subdivideSemicircle(depth - 1, vertices);
}

void MyGLWidget::getHalf(GLfloat v1[], GLfloat v2[], GLfloat v12[], GLfloat scale)
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

void MyGLWidget::addTriangle(GLfloat v1[], GLfloat v2[], GLfloat v3[], std::vector<GLfloat>& v)
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

void MyGLWidget::addVertex(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat>& v)
{
	v.push_back(x);
	v.push_back(y);
	v.push_back(z);
}

/*###################################################
##  函数: initRestaurant
##  函数描述： 餐厅部分VBO初始化
##  参数描述： 无
#####################################################*/
void MyGLWidget::initRestaurant() {

	GLfloat vertices_quads[(22 + 18 + 24) * 4 * 8];
	// 绘制底部
	GLfloat vertices_bottom[] = {
		// 位置              // 法向量
		/* 侧面 */
		 10.0f, 0.0f, 10.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 10.0f, 0.0f, 20.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 10.0f, 10.0f, 20.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 10.0f, 10.0f, 10.0f, -1.0f, 0.0f, 0.0f, 1.0, 1.0f,

		 // 原法向量反了
		 10.0f, 10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		 30.0f, 10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		 30.0f, 0, 10.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		 10.0f, 0, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		 30.0f, 10.0f, 10.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 30.0f, 10.0f, -20.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 30.0f, 0, -20.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 30.0f, 0, 10.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		 30.0f, 10.0f, -20.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 -30.0f, 10.0f, -20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 -30.0f, 0, -20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 30.0f, 0, -20.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		 // 原法向量反了
		 -30.0f, 10.0f, -20.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 -30.0f, 10.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 -30.0f, 0, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 -30.0f, 0, -20.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		 // 原法向量反了
		 -30.0f, 10.0f, 10.0f, 0.0f, 0.0f, -1.0f,0.0f, 1.0f,
		 -10.0f, 10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		 -10.0f, 0, 10.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		 -30.0, 0, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		 // 原法向量反了
		 -10.0f, 10.0f, 10.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 -10.0f, 10.0f, 20.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 -10.0f, 0, 20.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 -10.0f, 0, 10.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,


		  -10.0f, 10.0f, 20.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		  -40.0f, 10.0f, 20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		  -40.0f, 0, 20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		  -10.0f, 0, 20.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		 -40.0f, 10.0f, 20.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 -40.0f, 10.0f, -30.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 -40.0f, 0, -30.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 -40.0f, 0, 20.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		 // 原法向量反了
		 -40.0f, 10.0f, -30.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		 40.0f, 10.0f, -30.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		 40.0f, 0, -30.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		 -40.0f, 0, -30.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		 // 原法向量反了
		 40.0f, 10.0f, -30.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 40.0f, 10.0f, 20.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 40.0f, 0, 20.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 40.0f, 0, -30.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		 40.0f, 10.0f, 20.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 10.0f, 10.0f, 20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 10.0f, 0, 20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 40.0f, 0, 20.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,


		 // 顶面
		 10.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 30.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 40.0f, 10.0f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 10.0f, 10.0f, 20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		 30.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		 30.0f, 10.0f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 40.0f, 10.0f, -30.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 40.0f, 10.0f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		 // 原法向量反了
		 30.0f, 10.0f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 40.0f, 10.0f, -30.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 -40.0f, 10.0f, -30.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 -30.0f, 10.0f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		 // 原法向量反了
		 -30.0f, 10.0f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 -40.0f, 10.0f, -30.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 -40.0f, 10.0f, 20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		 -30.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		 -30.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 -10.0f, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 -10.0f, 10.0f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 -40.0f, 10.0f, 20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		 /* 底面 */

		 10.0f, 0, 10.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 30.0f, 0, 10.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		 40.0f, 0, 20.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 10.0f, 0, 20.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

		 30.0f, 0, 10.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 30.0f, 0, -20.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 40.0f, 0, -30.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		 40.0f, 0, 20.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

		 // 原法向量反了
		 30.0f, 0, -20.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 40.0f, 0, -30.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		 -40.0f, 0, -30.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 -30.0f, 0, -20.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 // 原法向量反了
		 -30.0f, 0, -20.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		 -40.0f, 0, -30.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 -40.0f, 0, 20.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 -30.0f, 0, 10.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

		 -30.0f, 0, 10.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 -10.0f, 0, 10.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		 -10.0f, 0, 20.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 -40.0f, 0, 20.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	};
	memcpy(vertices_quads, vertices_bottom, 22 * 4 * 8 * sizeof(float));
	// 绘制屋顶下部分和门板
	float verties[] = {
		// 门框， 第一个长方体（左）
		-14.0f, 10.0f, 10.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		-10.0f, 10.0f, 10.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		-10.0f, 50.0f, 10.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-14.0f, 50.0f, 10.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

		-14.0f, 10.0f, 14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		-10.0f, 10.0f, 14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		-10.0f, 50.0f, 14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-14.0f, 50.0f, 14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

		-14.0f, 50.0f, 14.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-14.0f, 50.0f, 10.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-14.0f, 10.0f, 10.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-14.0f, 10.0f, 14.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-10.0f, 50.0f, 14.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, 50.0f, 10.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-10.0f, 10.0f, 10.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-10.0f, 10.0f, 14.0f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-14.0f, 10.0f, 10.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		-10.0f, 10.0f, 10.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, 10.0f, 14.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		-14.0f, 10.0f, 14.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

		-14.0f, 50.0f, 10.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		-10.0f, 50.0f, 10.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, 50.0f, 14.0f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		-14.0f, 50.0f, 14.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,

		// 门框， 第二个长方体（上）
		-10.0f, 46.0f, 10.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 10.0f, 46.0f, 10.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 10.0f, 50.0f, 10.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-10.0f, 50.0f, 10.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

		-10.0f, 46.0f, 14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		 10.0f, 46.0f, 14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 10.0f, 50.0f, 14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-10.0f, 50.0f, 14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

		-10.0f, 50.0f, 14.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-10.0f, 50.0f, 10.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-10.0f, 46.0f, 10.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-10.0f, 46.0f, 14.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		 10.0f, 50.0f, 14.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 10.0f, 50.0f, 10.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 10.0f, 46.0f, 10.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 10.0f, 46.0f, 14.0f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-10.0f, 46.0f, 10.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		 10.0f, 46.0f, 10.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 10.0f, 46.0f, 14.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		-10.0f, 46.0f, 14.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

		-10.0f, 50.0f, 10.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		 10.0f, 50.0f, 10.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 10.0f, 50.0f, 14.0f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		-10.0f, 50.0f, 14.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,

		// 门框， 第三个长方体（右）
		 10.0f, 10.0f, 10.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 14.0f, 10.0f, 10.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 14.0f, 50.0f, 10.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		 10.0f, 50.0f, 10.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

		 10.0f, 10.0f, 14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		 14.0f, 10.0f, 14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 14.0f, 50.0f, 14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		 10.0f, 50.0f, 14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

		 10.0f, 50.0f, 14.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 10.0f, 50.0f, 10.0f,  -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 10.0f, 10.0f, 10.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 10.0f, 10.0f, 14.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		 14.0f, 50.0f, 14.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 14.0f, 50.0f, 10.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 14.0f, 10.0f, 10.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 14.0f, 10.0f, 14.0f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		 10.0f, 10.0f, 10.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		 14.0f, 10.0f, 10.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 14.0f, 10.0f, 14.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 10.0f, 10.0f, 14.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

		 10.0f, 50.0f, 10.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		 14.0f, 50.0f, 10.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 14.0f, 50.0f, 14.0f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		 10.0f, 50.0f, 14.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	};
	memcpy(vertices_quads + 22 * 4 * 8, verties, 18 * 4 * 8 * sizeof(float));

	// 屋顶下部
	float verties2[] = {
		// 屋顶下部,后方
				-30.0f, 50.0f, -24.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
				 30.0f, 50.0f, -24.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
				 30.0f, 70.0f, -24.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
				-30.0f, 70.0f, -24.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

				-30.0f, 50.0f, -23.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
				 30.0f, 50.0f, -23.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
				 30.0f, 70.0f, -23.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
				-30.0f, 70.0f, -23.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

				-30.0f, 70.0f, -23.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				-30.0f, 70.0f, -24.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
				-30.0f, 50.0f, -24.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				-30.0f, 50.0f, -23.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				 30.0f, 70.0f, -23.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				 30.0f, 70.0f, -24.0f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
				 30.0f, 50.0f, -24.0f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				 30.0f, 50.0f, -23.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				-30.0f, 50.0f, -24.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
				 30.0f, 50.0f, -24.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
				 30.0f, 50.0f, -23.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
				-30.0f, 50.0f, -23.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

				-30.0f, 70.0f, -24.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
				 30.0f, 70.0f, -24.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
				 30.0f, 70.0f, -23.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
				-30.0f, 70.0f, -23.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,

				// 屋顶下部,前方
				-30.0f, 50.0f,  13.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
				 30.0f, 50.0f,  13.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
				 30.0f, 70.0f,  13.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
				-30.0f, 70.0f,  13.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

				-30.0f, 50.0f,  14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
				 30.0f, 50.0f,  14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
				 30.0f, 70.0f,  14.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
				-30.0f, 70.0f,  14.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

				-30.0f, 70.0f,  14.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				-30.0f, 70.0f,  13.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
				-30.0f, 50.0f,  13.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				-30.0f, 50.0f,  14.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				 30.0f, 70.0f,  14.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				 30.0f, 70.0f,  13.5f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
				 30.0f, 50.0f,  13.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				 30.0f, 50.0f,  14.0f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				-30.0f, 50.0f,  13.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
				 30.0f, 50.0f,  13.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
				 30.0f, 50.0f,  14.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
				-30.0f, 50.0f,  14.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

				-30.0f, 70.0f,  13.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
				 30.0f, 70.0f,  13.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
				 30.0f, 70.0f,  14.0f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
				-30.0f, 70.0f,  14.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,

				// 屋顶下部,右边
				33.5f, 50.0f, -20.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
				34.0f, 50.0f, -20.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
				34.0f, 70.0f, -20.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
				33.5f, 70.0f, -20.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

				33.5f, 50.0f,  10.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
				34.0f, 50.0f,  10.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
				34.0f, 70.0f,  10.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
				33.5f, 70.0f,  10.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

				33.5f, 70.0f,  10.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				33.5f, 70.0f, -20.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
				33.5f, 50.0f, -20.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				33.5f, 50.0f,  10.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				34.0f, 70.0f,  10.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				34.0f, 70.0f, -20.0f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
				34.0f, 50.0f, -20.0f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				34.0f, 50.0f,  10.0f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				33.5f, 50.0f, -20.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
				34.0f, 50.0f, -20.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
				34.0f, 50.0f,  10.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
				33.5f, 50.0f,  10.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

				33.5f, 70.0f, -20.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
				34.0f, 70.0f, -20.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
				34.0f, 70.0f,  10.0f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
				33.5f, 70.0f,  10.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,

				// 屋顶下部,左边
				-34.0f, 50.0f, -20.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
				-33.5f, 50.0f, -20.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
				-33.5f, 70.0f, -20.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
				-34.0f, 70.0f, -20.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

				-34.0f, 50.0f,  10.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
				-33.5f, 50.0f,  10.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
				-33.5f, 70.0f,  10.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
				-34.0f, 70.0f,  10.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

				-34.0f, 70.0f,  10.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				-34.0f, 70.0f, -20.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 1.0f,
				-34.0f, 50.0f, -20.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				-34.0f, 50.0f,  10.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				-33.5f, 70.0f,  10.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
				-33.5f, 70.0f, -20.0f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
				-33.5f, 50.0f, -20.0f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
				-33.5f, 50.0f,  10.0f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

				-34.0f, 50.0f, -20.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
				-33.5f, 50.0f, -20.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
				-33.5f, 50.0f,  10.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
				-34.0f, 50.0f,  10.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

				-34.0f, 70.0f, -20.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
				-33.5f, 70.0f, -20.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
				-33.5f, 70.0f,  10.0f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
				-34.0f, 70.0f,  10.0f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	};
	memcpy(vertices_quads + (22 + 18) * 4 * 8, verties2, 24 * 4 * 8 * sizeof(float));

	// 外部底层VBO初始化
	// 生成VBO对象绑定
	glGenBuffers(1, &quadsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadsVBO);
	// 生成VAO对象绑定
	glGenVertexArrays(1, &quadsVAO);
	glBindVertexArray(quadsVAO);
	// 把之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_quads), &vertices_quads, GL_STATIC_DRAW);
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 解绑VAO
	glBindVertexArray(0);
}


/*###################################################
##  函数: DrawRestaurant
##  函数描述： 绘图餐厅外部
##  参数描述： 无
#####################################################*/
void MyGLWidget::drawRestaurant() {

	// 绘制底部、门板和屋顶下部
	useTexture = 1;	// 使用漫反射纹理贴图
	//setDiffuseMap("img/wood7.jpg");	// 指定图片
	glPushMatrix();
	glBindVertexArray(quadsVAO);
	updateShader();	// 每次进行绘制之前要调用该方法
	glDrawArrays(GL_QUADS, 0, 4 * (22 + 18 + 24));
	glBindVertexArray(0);
	glPopMatrix();

	// 绘制屋顶
	drawSemicylinder(0.0f, 70.0f, -5.0f, 34.0f, 20.0f, 19.0f, -90.0f, 0.0f, 0.0f, 1.0f);

	// 柱子2，注意后面的柱子先绘制
	glPushMatrix();
	glTranslatef(34.0f, 10.0f, -24.0f);
	glRotatef(-90, 1, 0, 0);
	// 半径为4，高为60的圆柱体
	drawCylinder(4.0f, 360.0f, 60.0f);
	glPopMatrix();
	// 柱子3
	glPushMatrix();
	glTranslatef(-34.0f, 10.0f, -24.0f);
	glRotatef(-90, 1, 0, 0);
	// 半径为4，高为60的圆柱体
	drawCylinder(4.0f, 360.0f, 60.0f);
	glPopMatrix();


	// 绘制两旁的柱子
	// 柱子1
	glPushMatrix();
	glTranslatef(34.0f, 10.0f, 14.0f);
	// 柱子沿x轴旋转90度
	glRotatef(-90, 1, 0, 0);
	// 半径为4，高为60的圆柱体
	drawCylinder(4.0f, 360.0f, 60.0f);
	glPopMatrix();
	// 柱子4
	glPushMatrix();
	glTranslatef(-34.0f, 10.0f, 14.0f);
	glRotatef(-90, 1, 0, 0);
	// 半径为4，高为60的圆柱体
	drawCylinder(4.0f, 360.0f, 60.0f);
	glPopMatrix();

	// 绘制圆环
	glPushMatrix();
	glTranslatef(34.0, 70.0f, -5.0f);
	glRotatef(90, 0, 1.0, 0.0);
	drawTorus(15, 23);
	glPopMatrix();

	// 绘制圆环
	glPushMatrix();
	glTranslatef(-34.0, 70.0f, -5.0f);
	glRotatef(90, 0, 1.0, 0.0);
	drawTorus(15, 23);
	glPopMatrix();

	useTexture = 0;

}


/*###################################################
##  函数: DrawTorus
##  函数描述： innerRadius:内环半径，OutEadius：外环半径
##  参数描述： 绘制三维立体半圆环
#####################################################*/
void MyGLWidget::drawTorus(float innerRadius, float OutRadius) {
	const int Sides = 100;
	const int Rings = 100;
	float verties[8 * (Sides + 1) * (Rings + 1) * 2];
	float alphaStep = (float)(PI / Sides); // 只画半圆环,不用乘2
	float BetaStep = (float)(2 * PI / Sides);
	float beta = 0;
	float alpha = 0;
	float x0, x1, y0, y1, z0, z1, x2, y2, z2;
	float nor[3];
	float r = (OutRadius - innerRadius) / 2; // 圆环的半径
	int index = 0;
	for (int i = 0; i <= Sides; i++) {
		alpha = i * alphaStep;
		for (int j = 0; j <= Rings; j++) {
			beta = j * BetaStep;
			x0 = (float)(cos(alpha) * (r + innerRadius + r * cos(beta)));
			y0 = (float)(sin(alpha) * (r + innerRadius + r * cos(beta)));
			z0 = (float)(r * sin(beta));
			x1 = (float)(cos(alpha + alphaStep) * (r + innerRadius + r * cos(beta)));
			y1 = (float)(sin(alpha + alphaStep) * (r + innerRadius + r * cos(beta)));
			z1 = (float)(r * sin(beta));
			nor[0] = (float)(cos(alpha) * cos(beta));
			nor[1] = (float)(sin(alpha) * cos(beta));
			nor[2] = (float)(sin(beta));
			verties[index++] = x0;
			verties[index++] = y0;
			verties[index++] = z0;
			verties[index++] = nor[0];
			verties[index++] = nor[1];
			verties[index++] = nor[2];
			verties[index++] = ((float)2 * i / Rings);
			verties[index++] = (float)j / Rings;

			nor[0] = (float)(cos(alpha + alphaStep) * cos(beta));
			nor[1] = (float)(sin(alpha + alphaStep) * cos(beta));
			nor[2] = (float)(sin(beta));
			verties[index++] = x1;
			verties[index++] = y1;
			verties[index++] = z1;
			verties[index++] = nor[0];
			verties[index++] = nor[1];
			verties[index++] = nor[2];
			verties[index++] = ((float)2 * i / Rings);
			verties[index++] = (float)j / Rings;
		}
	}
	glGenBuffers(1, &VBOOutButtomId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOOutButtomId);
	// 生成VAO对象绑定
	glGenVertexArrays(1, &VAOOutButtomId);
	glBindVertexArray(VAOOutButtomId);
	// 把之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(verties), verties, GL_STATIC_DRAW);
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 解绑VAO
	glBindVertexArray(0);

	glBindVertexArray(VAOOutButtomId);
	updateShader();
	glDrawArrays(GL_QUAD_STRIP, 0, 2 * (Sides + 1) * (Rings + 1));
	glBindVertexArray(0);
}

/*###################################################
##  函数: DrawCylinder
##  函数描述： innerRadius:内环半径，OutEadius：外环半径，Sides和Rings都是切割数
##  参数描述： 绘制圆柱体
#####################################################*/
void MyGLWidget::drawCylinder(float Radius, float angle, float height) {
	const int n = 50;
	float VertiesCylinder[8 * (n + 1) * 2];
	float angDegSpan = angle / n;
	int index = 0;
	int texIndex = 0;
	float p1[3], p2[3], p3[3], p4[3];
	float nor[3];
	for (int u = 0; u <= n; u++) {
		float i = u * angDegSpan;
		p2[0] = (float)(Radius * sin(i * PI / 180.0f));
		p2[1] = (float)(Radius * cos(i * PI / 180.0f));
		p2[2] = height;
		p1[0] = (float)(Radius * sin(i * PI / 180.0f));
		p1[1] = (float)(Radius * cos(i * PI / 180.0f));
		p1[2] = 0.0;
		VertiesCylinder[index++] = p2[0];
		VertiesCylinder[index++] = p2[1];
		VertiesCylinder[index++] = p2[2];
		VertiesCylinder[index++] = p2[0];
		VertiesCylinder[index++] = p2[1];
		VertiesCylinder[index++] = 0.0f;
		VertiesCylinder[index++] = i / 360.0f;
		VertiesCylinder[index++] = 0.0f;

		VertiesCylinder[index++] = p1[0];
		VertiesCylinder[index++] = p1[1];
		VertiesCylinder[index++] = p1[2];
		VertiesCylinder[index++] = p1[0];
		VertiesCylinder[index++] = p1[1];
		VertiesCylinder[index++] = 0.0f;
		VertiesCylinder[index++] = i / 360.0f;
		VertiesCylinder[index++] = 1.0f;
	}

	glGenBuffers(1, &VBOOutButtomId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOOutButtomId);
	// 生成VAO对象绑定
	glGenVertexArrays(1, &VAOOutButtomId);
	glBindVertexArray(VAOOutButtomId);
	// 把之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertiesCylinder), VertiesCylinder, GL_STATIC_DRAW);
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 解绑VAO
	glBindVertexArray(0);

	glBindVertexArray(VAOOutButtomId);
	updateShader();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (n + 1));
	glBindVertexArray(0);
}


void MyGLWidget::drawDesk()
{
	// 桌面
	useTexture = 1;
	//setDiffuseMap("img/desk1.png");
	drawCylinder(0.0f, 21.0f, 0.0f, 16.0f, 3.0f, 16.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	// 支柱
	//setDiffuseMap("img/desk2.png");
	drawCylinder(0.0f, 11.0f, 0.0f, 2.0f, 20.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	// 地盘
	drawCylinder(0.0f, 0.0f, 0.0f, 5.0f, 2.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	// 六个装饰
	useTexture = 0;
	setObjectColor(192 / 255.0f, 0.0f, 0.0f);
	drawTaper(-19.0f, 21.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	drawTaper(-9.5f, 21.0f, 16.45f, 60.0f, 0.0f, 1.0f, 0.0f);
	drawTaper(9.5f, 21.0f, 16.45f, 120.0f, 0.0f, 1.0f, 0.0f);
	drawTaper(19.0f, 21.0f, 0.0f, 180.0f, 0.0f, 1.0f, 0.0f);
	//drawTaper(9.5f, 0.0f, -16.45f, 60.0f, 0.0f, -1.0f, 0.0f);
	//drawTaper(-9.5f, 0.0f, -16.45f, 120.0f, 0.0f, -1.0f, 0.0f);
	drawTaper(9.5f, 21.0f, -16.45f, 240.0f, 0.0f, 1.0f, 0.0f);
	drawTaper(-9.5f, 21.0f, -16.45f, 300.0f, 0.0f, 1.0f, 0.0f);
}

void MyGLWidget::drawChair()
{

	// 绘制桶上沿
	useTexture = 1;
	//setDiffuseMap("img/chair1_2.png");
	glPushMatrix();
	glTranslatef(0.0f, 12.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	updateShader();
	drawTorus(2.5, 3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 12.0f, 0.0f);
	glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
	updateShader();
	drawTorus(2.5, 3);
	glPopMatrix();

	//绘制水桶
	drawOval(0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	drawCone(0.0f, 7.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	drawCylinder(0.0f, 6.0f, 0.0f, 4.0f, 2.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(177.0f, 0.0f, 1.0f, 0.0f);
	drawCone(0.0f, 5.0f, 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	drawOval(0.0f, 2.0f, 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	glPopMatrix();


	// 绘制桶的铁环
	//setDiffuseMap("img/chair2.png");
	glPushMatrix();
	glTranslatef(0.0f, 10.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	updateShader();
	drawTorus(3.1, 3.7);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 10.0f, 0.0f);
	glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
	updateShader();
	drawTorus(3.1, 3.7);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 2.0f, 0.0f);
	glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
	updateShader();
	drawTorus(3.1, 3.7);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 2.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	updateShader();
	drawTorus(3.1, 3.7);
	glPopMatrix();

}

void MyGLWidget::drawOval(float tx, float ty, float tz, float angle, float rx, float ry, float rz)
{
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glRotatef(angle, rx, ry, rz);
	updateShader();

	glBindBuffer(GL_ARRAY_BUFFER, ovalVBO);

	// 指定顶点属性数组的数据格式和位置
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glDrawArrays(GL_TRIANGLES, 0, vertexNumOfOval);
	glPopMatrix();
}

void MyGLWidget::drawCone(float tx, float ty, float tz, float angle, float rx, float ry, float rz)
{
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glRotatef(angle, rx, ry, rz);
	updateShader();

	glBindBuffer(GL_ARRAY_BUFFER, coneVBO);

	// 指定顶点属性数组的数据格式和位置
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, vertexNumOfCone);
	glPopMatrix();
}

void MyGLWidget::drawCylinder(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glScalef(sx, sy, sz);
	glRotatef(angle, rx, ry, rz);
	updateShader();

	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);

	// 指定顶点属性数组的数据格式和位置
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, vertexNumOfCylinder);
	glPopMatrix();
}

void MyGLWidget::drawTaper(float tx, float ty, float tz, float angle, float rx, float ry, float rz)
{
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glRotatef(angle, rx, ry, rz);
	updateShader();

	glBindBuffer(GL_ARRAY_BUFFER, taperVBO);

	// 指定顶点属性数组的数据格式和位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//glDrawArrays(GL_TRIANGLES, 0, taperVertices.size() / 2);
	glDrawArrays(GL_TRIANGLES, 0, taperVertices.size() / 2 / 3);
	glPopMatrix();
}

void MyGLWidget::initCylinderVBO()
{
	// 四个三角形，作为圆迭代的开始
	GLfloat temp[4 * 3 * 3] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	// 将temp中的顶点信息保存到circleVertices中
	circleVertices.resize(4 * 3 * 3);
	for (int i = 0; i < 4 * 3 * 3; i++)
	{
		circleVertices[i] = temp[i];
	}

	// 迭代10次 函数调用完成后circleVertices中保存了绘制半圆的顶点信息
	// 圆由许多细分三角形组成 每个三角形以原点为起点 按顺时针方向保存顶点信息
	subdivideCircle(ITERATIONS, circleVertices, 1);

	vertexNumOfCylinder = circleVertices.size() / 3 * (1 + 1 + 2);

	vector<GLfloat> upperVertices; // 上圆
	vector<GLfloat> lowerVertices; // 下圆
	vector<GLfloat> sideVertices; // 侧面

	float deltaAngle = 360.0 / (circleVertices.size() / 9);

	for (int i = 0; i < circleVertices.size() / 3; i++)
	{
		// 顶点位置
		upperVertices.push_back(circleVertices[i * 3]);
		upperVertices.push_back(circleVertices[i * 3 + 1] + 0.5f);
		upperVertices.push_back(circleVertices[i * 3 + 2]);
		//法向量 竖直向上
		upperVertices.push_back(0.0f);
		upperVertices.push_back(1.0f);
		upperVertices.push_back(0.0f);
		// 纹理坐标
		if (i % 3 == 0)
		{
			upperVertices.push_back(0.5f);
			upperVertices.push_back(0.5f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 360.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 360.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}

		//顶点位置
		lowerVertices.push_back(circleVertices[i * 3]);
		lowerVertices.push_back(circleVertices[i * 3 + 1] - 0.5f);
		lowerVertices.push_back(circleVertices[i * 3 + 2]);
		//法向量 竖直向下
		lowerVertices.push_back(0.0f);
		lowerVertices.push_back(-1.0f);
		lowerVertices.push_back(0.0f);
		//纹理坐标
		if (i % 3 == 0)
		{
			lowerVertices.push_back(0.5f);
			lowerVertices.push_back(0.5f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 360.0 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 360 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
	}

	// 设置侧面中曲面相关的顶点信息
	// 上下两个细分三角形的原点对边 构成一个侧面细分长方形
	int numOfTriangles = circleVertices.size() / 3 / 3;
	for (int i = 0; i < circleVertices.size() / 3 / 3; i++)
	{
		// 记上面的细分三角形为a, 下面的细分三角形为b
		// v1 v2 和v3 分别为原细分三角形的三个顶点
		GLfloat v1[3], v2[3], v3[3];
		GLfloat va1[3], va2[3], va3[3], vb1[3], vb2[3], vb3[3];
		for (int j = 0; j < 3; j++)
		{
			v1[j] = circleVertices[i * 9 + j];
			v2[j] = circleVertices[i * 9 + j + 3];
			v3[j] = circleVertices[i * 9 + j + 6];

			va1[j] = upperVertices[i * 24 + j];
			va2[j] = upperVertices[i * 24 + j + 8];
			va3[j] = upperVertices[i * 24 + j + 16];
			vb1[j] = lowerVertices[i * 24 + j];
			vb2[j] = lowerVertices[i * 24 + j + 8];
			vb3[j] = lowerVertices[i * 24 + j + 16];
		}

		addVertex(va2[0], va2[1], va2[2], sideVertices);
		addVertex(v2[0], v2[1], v2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.16f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(v2[0], v2[1], v2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(v3[0], v3[1], v3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(0.16f);

		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(v3[0], v3[1], v3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(0.16f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(v2[0], v2[1], v2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(vb3[0], vb3[1], vb3[2], sideVertices);
		addVertex(v3[0], v3[1], v3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(0.0f);
	}
	cylinderVertices.insert(cylinderVertices.end(), upperVertices.begin(), upperVertices.end());
	cylinderVertices.insert(cylinderVertices.end(), lowerVertices.begin(), lowerVertices.end());
	cylinderVertices.insert(cylinderVertices.end(), sideVertices.begin(), sideVertices.end());

	glGenBuffers(1, &cylinderVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
	glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(GLfloat), &cylinderVertices[0], GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyGLWidget::subdivideCircle(int depth, vector<GLfloat>& vertices, float length)
{
	// 当深度小于等于0时 停止迭代
	if (depth <= 0)
	{
		return;
	}

	// 创建一个临时数组temp 保存原来的顶点信息
	vector<GLfloat> temp(vertices);
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
	subdivideCircle(depth - 1, vertices, length);
}

void MyGLWidget::initTaperVBO()
{
	// 四个三角形 作为圆迭代的开始
	GLfloat temp[4 * 3 * 3] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	// 将temp中的顶点信息保存到taperdownVertices中
	taperdownVertices.resize(4 * 3 * 3);
	for (int i = 0; i < 4 * 3 * 3; i++)
	{
		taperdownVertices[i] = temp[i];
	}
	subdivideCircle(ITERATIONS, taperdownVertices, 1);

	vector<GLfloat> sideVertices; // 侧面
	vector<GLfloat> lowerVertices; // 下半圆

	for (int i = 0; i < taperdownVertices.size() / 3; ++i)
	{
		// 顶点位置
		lowerVertices.push_back(taperdownVertices[i * 3]);
		lowerVertices.push_back(taperdownVertices[i * 3 + 1]);
		lowerVertices.push_back(taperdownVertices[i * 3 + 2]);
		// 法向量 朝x轴负方向
		lowerVertices.push_back(-1.0f);
		lowerVertices.push_back(0.0f);
		lowerVertices.push_back(0.0f);
	}

	// 设置侧面中曲面相关的顶点信息
	// 圆锥底部细分三角形的原点对边，加上位于x轴上的点(4.0f, 0.0f, 0.0f)构成一个侧面细分三角形
	for (int i = 0; i < taperdownVertices.size() / 3 / 3; i++)
	{
		//记底部的细分三角形为a
		//v1 v2 和 v3 分别为原细分三角形的三个顶点
		GLfloat v1[3], v2[3], v3[3];
		GLfloat va1[3], va2[3], va3[3];
		for (int j = 0; j < 3; j++)
		{
			v1[j] = taperdownVertices[i * 9 + j];
			v2[j] = taperdownVertices[i * 9 + j + 3];
			v3[j] = taperdownVertices[i * 9 + j + 6];

			va1[j] = lowerVertices[i * 18 + j]; // 原点 无作用
			va2[j] = lowerVertices[i * 18 + j + 6];
			va3[j] = lowerVertices[i * 18 + j + 12];
		}

		addVertex(va2[0], va2[1], va2[2], sideVertices); // 位置
		addVertex(v2[0] + 0.25f, v2[1], v2[2], sideVertices); // 法向量
		addVertex(va3[0], va3[1], va3[2], sideVertices); // 位置
		addVertex(v3[0] + 0.25f, v3[1], v3[2], sideVertices); // 法向量
		addVertex(4.0f, 0.0f, 0.0f, sideVertices); // 位置
		addVertex((v2[0] + v3[0]) / 2 + 0.25f, (v2[1] + v3[1]) / 2, (v2[2] + v3[2]) / 2, sideVertices); //法向量
	}

	taperVertices.insert(taperVertices.end(), lowerVertices.begin(), lowerVertices.end());
	taperVertices.insert(taperVertices.end(), sideVertices.begin(), sideVertices.end());

	// 为VBO创建一个缓冲区对象
	glGenBuffers(1, &taperVBO);
	// 利用vertices数组中的内容更新VBO的信息
	glBindBuffer(GL_ARRAY_BUFFER, taperVBO);
	glBufferData(GL_ARRAY_BUFFER, taperVertices.size() * sizeof(GLfloat), &taperVertices[0], GL_STATIC_DRAW);
	// 指定顶点属性数组的数据格式和位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 解绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyGLWidget::initOvalVBO()
{
	// 四个三角形，作为圆迭代的开始
	GLfloat tempup[4 * 3 * 3] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.0f,
		-3.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		-3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.0f,
		3.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.0f,
	};

	GLfloat tempdown[4 * 3 * 3] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.5f,
		-3.5f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		-3.5f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.5f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.5f,
		3.5f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		3.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.5f,
	};

	ovalupVertices.resize(4 * 3 * 3);
	ovaldownVertices.resize(4 * 3 * 3);
	for (int i = 0; i < 4 * 3 * 3; i++)
	{
		ovalupVertices[i] = tempup[i];
		ovaldownVertices[i] = tempdown[i];
	}

	subdivideCircle(ITERATIONS, ovalupVertices, 3);
	subdivideCircle(ITERATIONS, ovaldownVertices, 3.5);

	vertexNumOfOval = ovalupVertices.size() / 3 * (1 + 2) + ovaldownVertices.size() / 3;

	vector<GLfloat> upperVertices; // 上圆
	vector<GLfloat> lowerVertices; // 下圆
	vector<GLfloat> sideVertices; // 侧面

	float deltaAngle = 360.0 / (ovalupVertices.size() / 9);

	for (int i = 0; i < ovalupVertices.size() / 3; i++)
	{
		// 顶点位置
		upperVertices.push_back(ovalupVertices[i * 3]);
		upperVertices.push_back(ovalupVertices[i * 3 + 1] + 2.0f);
		upperVertices.push_back(ovalupVertices[i * 3 + 2]);
		//法向量 竖直向上
		upperVertices.push_back(0.0f);
		upperVertices.push_back(1.0f);
		upperVertices.push_back(0.0f);
		//纹理坐标
		if (i % 3 == 0)
		{
			upperVertices.push_back(0.5f);
			upperVertices.push_back(0.5f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 360.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 360.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
	}

	for (int i = 0; i < ovaldownVertices.size() / 3; i++)
	{
		//顶点位置
		lowerVertices.push_back(ovaldownVertices[i * 3]);
		lowerVertices.push_back(ovaldownVertices[i * 3 + 1] + 0.0f);
		lowerVertices.push_back(ovaldownVertices[i * 3 + 2]);
		//法向量 竖直向下
		lowerVertices.push_back(0.0f);
		lowerVertices.push_back(-1.0f);
		lowerVertices.push_back(0.0f);

		if (i % 3 == 0)
		{
			lowerVertices.push_back(0.5f);
			lowerVertices.push_back(0.5f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 360.0 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 360.0 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
	}

	// 设置侧面中曲面相关的顶点信息
	// 上下两个细分三角形的原点对边 构成一个侧面细分梯形
	int numOfTriangles = ovalupVertices.size() / 3 / 3;
	for (int i = 0; i < ovalupVertices.size() / 3 / 3; i++)
	{
		// 记上面的细分三角形为a, 下面的细分三角形为b
		// v1 v2 和v3 分别为原细分三角形的三个顶点
		GLfloat vu1[3], vu2[3], vu3[3], vd1[3], vd2[3], vd3[3];
		GLfloat va1[3], va2[3], va3[3], vb1[3], vb2[3], vb3[3];
		for (int j = 0; j < 3; j++)
		{
			vu1[j] = ovalupVertices[i * 9 + j];
			vu2[j] = ovalupVertices[i * 9 + j + 3];
			vu3[j] = ovalupVertices[i * 9 + j + 6];
			vd1[j] = ovaldownVertices[i * 9 + j];
			vd2[j] = ovaldownVertices[i * 9 + j + 3];
			vd3[j] = ovaldownVertices[i * 9 + j + 6];

			va1[j] = upperVertices[i * 24 + j];
			va2[j] = upperVertices[i * 24 + j + 8];
			va3[j] = upperVertices[i * 24 + j + 16];
			vb1[j] = lowerVertices[i * 24 + j];
			vb2[j] = lowerVertices[i * 24 + j + 8];
			vb3[j] = lowerVertices[i * 24 + j + 16];
		}

		addVertex(va2[0], va2[1], va2[2], sideVertices);
		addVertex(vu2[0], vu2[1] + 0.75f, vu2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(1.0f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(vd2[0], vd2[1] + 0.88f, vd2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(vu3[0], vu3[1] + 0.75f, vu3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(1.0f);

		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(vu3[0], vu3[1] + 0.75f, vu3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(1.0f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(vd2[0], vd2[1] + 0.88f, vd2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(vb3[0], vb3[1], vb3[2], sideVertices);
		addVertex(vd3[0], vd3[1] + 0.88f, vd3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(0.0f);
	}
	ovalVertices.insert(ovalVertices.end(), upperVertices.begin(), upperVertices.end());
	ovalVertices.insert(ovalVertices.end(), lowerVertices.begin(), lowerVertices.end());
	ovalVertices.insert(ovalVertices.end(), sideVertices.begin(), sideVertices.end());

	glGenBuffers(1, &ovalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ovalVBO);
	glBufferData(GL_ARRAY_BUFFER, ovalVertices.size() * sizeof(GLfloat), &ovalVertices[0], GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyGLWidget::initConeVBO()
{
	// 四个三角形，作为圆迭代的开始
	GLfloat tempup[4 * 3 * 3] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.5f,
		-3.5f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		-3.5f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.5f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.5f,
		3.5f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		3.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.5f,
	};

	GLfloat tempdown[4 * 3 * 3] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 4.0f,
		-4.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		-4.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -4.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -4.0f,
		4.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		4.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 4.0f,
	};

	coneupVertices.resize(4 * 3 * 3);
	conedownVertices.resize(4 * 3 * 3);
	for (int i = 0; i < 4 * 3 * 3; i++)
	{
		coneupVertices[i] = tempup[i];
		conedownVertices[i] = tempdown[i];
	}

	subdivideCircle(ITERATIONS, coneupVertices, 3.5);
	subdivideCircle(ITERATIONS, conedownVertices, 4);

	vertexNumOfCone = coneupVertices.size() / 3 * (1 + 2) + conedownVertices.size() / 3;
	vector<GLfloat> upperVertices; // 上圆
	vector<GLfloat> lowerVertices; // 下圆
	vector<GLfloat> sideVertices; // 侧面

	float deltaAngle = 360.0 / (coneupVertices.size() / 9);

	for (int i = 0; i < coneupVertices.size() / 3; i++)
	{
		// 顶点位置
		upperVertices.push_back(coneupVertices[i * 3]);
		upperVertices.push_back(coneupVertices[i * 3 + 1] + 3.0f);
		upperVertices.push_back(coneupVertices[i * 3 + 2]);
		//法向量 竖直向上
		upperVertices.push_back(0.0f);
		upperVertices.push_back(1.0f);
		upperVertices.push_back(0.0f);
		//纹理坐标
		if (i % 3 == 0)
		{
			upperVertices.push_back(0.5f);
			upperVertices.push_back(0.5f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 360.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 360.0 - deltaAngle * triangleIndex;
			upperVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			upperVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
	}

	for (int i = 0; i < conedownVertices.size() / 3; i++)
	{
		//顶点位置
		lowerVertices.push_back(conedownVertices[i * 3]);
		lowerVertices.push_back(conedownVertices[i * 3 + 1] + 0.0f);
		lowerVertices.push_back(conedownVertices[i * 3 + 2]);
		//法向量 竖直向下
		lowerVertices.push_back(0.0f);
		lowerVertices.push_back(-1.0f);
		lowerVertices.push_back(0.0f);
		if (i % 3 == 0)
		{
			lowerVertices.push_back(0.5f);
			lowerVertices.push_back(0.5f);
		}
		else if (i % 3 == 1)
		{
			int triangleIndex = i / 3;
			float theta = 360.0 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
		else if (i % 3 == 2)
		{
			int triangleIndex = i / 3 + 1;
			float theta = 360.0 - deltaAngle * triangleIndex;
			lowerVertices.push_back(0.5f + 0.5 * sin(theta * PI / 180));
			lowerVertices.push_back(0.5f + 0.5 * cos(theta * PI / 180));
		}
	}

	// 设置侧面中曲面相关的顶点信息
	// 上下两个细分三角形的原点对边 构成一个侧面细分梯形
	int numOfTriangles = coneupVertices.size() / 3 / 3;
	for (int i = 0; i < coneupVertices.size() / 3 / 3; i++)
	{
		// 记上面的细分三角形为a, 下面的细分三角形为b
		// v1 v2 和v3 分别为原细分三角形的三个顶点
		GLfloat vu1[3], vu2[3], vu3[3], vd1[3], vd2[3], vd3[3];
		GLfloat va1[3], va2[3], va3[3], vb1[3], vb2[3], vb3[3];
		for (int j = 0; j < 3; j++)
		{
			vu1[j] = coneupVertices[i * 9 + j];
			vu2[j] = coneupVertices[i * 9 + j + 3];
			vu3[j] = coneupVertices[i * 9 + j + 6];
			vd1[j] = conedownVertices[i * 9 + j];
			vd2[j] = conedownVertices[i * 9 + j + 3];
			vd3[j] = conedownVertices[i * 9 + j + 6];

			va1[j] = upperVertices[i * 24 + j];
			va2[j] = upperVertices[i * 24 + j + 8];
			va3[j] = upperVertices[i * 24 + j + 16];
			vb1[j] = lowerVertices[i * 24 + j];
			vb2[j] = lowerVertices[i * 24 + j + 8];
			vb3[j] = lowerVertices[i * 24 + j + 16];
		}

		addVertex(va2[0], va2[1], va2[2], sideVertices);
		addVertex(vu2[0], vu2[1] + 0.58f, vu2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(1.0f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(vd2[0], vd2[1] + 0.67f, vd2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(vu3[0], vu3[1] + 0.58f, vu3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(1.0f);

		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(vu3[0], vu3[1] + 0.58f, vu3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(1.0f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(vd2[0], vd2[1] + 0.67f, vd2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(vb3[0], vb3[1], vb3[2], sideVertices);
		addVertex(vd3[0], vd3[1] + 0.67f, vd3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(0.0f);
	}
	coneVertices.insert(coneVertices.end(), upperVertices.begin(), upperVertices.end());
	coneVertices.insert(coneVertices.end(), lowerVertices.begin(), lowerVertices.end());
	coneVertices.insert(coneVertices.end(), sideVertices.begin(), sideVertices.end());

	glGenBuffers(1, &coneVBO);
	glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
	glBufferData(GL_ARRAY_BUFFER, coneVertices.size() * sizeof(GLfloat), &coneVertices[0], GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*###################################################
##  函数: DrawSkybox
##  函数描述： 天空盒绘制
##  参数描述： 无
#####################################################*/
void MyGLWidget::DrawSkybox()
{
	glBindVertexArray(skyboxVAO);
	updateShader();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

/*###################################################
##  函数: initSkybox
##  函数描述： 天空盒部分VBO初始化
##  参数描述： 无
#####################################################*/
void MyGLWidget::initSkybox()
{
	// 正方体
	GLfloat skyboxVertices[12 * 3 * 8] = {
		/* 位置 */			  /* 法向量 */		   /* 纹理坐标 */
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  0.34f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  0.75f,  0.34f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  0.75f,  0.65f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  0.75f,  0.65f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  0.65f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  0.34f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.25f,  0.34f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.34f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.65f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.5f,  0.65f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.25f, 0.65f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.25f,  0.34f,

		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f,  0.65f,
		-0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.65f,
		-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.34f,
		-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.34f,
		-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f,  0.34f,
		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.25f,  0.65f,

		 0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.5f,  0.65f,
		 0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f,  0.65f,
		 0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f,  0.34f,
		 0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.75f,  0.34f,
		 0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.5f,  0.34f,
		 0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.5f,  0.65f,

		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.25f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.34f,
		 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.34f,
		-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.25f,  0.34f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.25f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.25f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.65f,
		 0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.5f,  0.65f,
		-0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.25f,  0.65f,
		-0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.25f,  1.0f
	};
	// skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}
