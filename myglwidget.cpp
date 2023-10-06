#include "myglwidget.h"
using namespace std;

MyGLWidget::MyGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
	timer = new QTimer(this);	// 实例化一个定时器
	timer->start(40);			// 时间间隔设置为40ms

	// 利用Qt的信号和槽机制 连接定时器和update()函数，每16ms触发一次update()函数进行重新绘图
	// 调用update()函数时 会请求重绘事件 并在下一个事件循环中调用paintGL()函数来重新绘制OpenGL场景
	connect(timer, SIGNAL(timeout()), this, SLOT(update())); 
}

MyGLWidget::~MyGLWidget()
{
	delete this->timer;
}

void MyGLWidget::initializeGL()
{
	resize(1280, 800);
	// 将当前OpenGL上下文中的函数指针与Qt提供的OpenGL函数类进行绑定
	initializeOpenGLFunctions();

	// 设置深度缓冲
	glClearDepth(1.0f);			// 设置深度缓冲区的清除值 1.0为最远的深度
	glDepthFunc(GL_LESS);		// 设置深度测试函数 GL_LESS指定只有当新像素的深度值
								// 小于当前像素的深度值时 新像素才会被绘制。
	glEnable(GL_DEPTH_TEST);	// 启用深度测试 OpenGL会自动根据深度值进行像素的遮挡和显示

	// 设置透视投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	// 将投影矩阵重置为单位矩阵 确保当前的设置不受到以前的影响
	float range = 45.0f;
	// 四个参数分别为 视角范围（以度为单位） 宽高比 近裁剪面和远裁剪面
	gluPerspective(range, 1.0 * width() / height(), 0.1f, 2000.0f);

	// 初始化着色器和VBO
	initShader("vertexShader.glsl", "fragmentShader.glsl");

	// 初始化各种形状和组件的VBO和VAO
	initCube();
	initSemicylinder();
	initCylinder();
	initCone();
	initFrustum(frustumVAO1, frustumVBO1, vertexNumOfFrustum1, 3.0f, 3.5f, 2.0f);
	initFrustum(frustumVAO2, frustumVBO2, vertexNumOfFrustum2, 3.5f, 4.0f, 3.0f);

	initRestaurant();
	initSkybox();

	// 启用混合功能
	glEnable(GL_BLEND);
	// 根据源颜色的alpha通道值来确定绘制物体时的不透明度
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 设置每一帧开始前用来清除颜色缓冲区的颜色为黑色
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void MyGLWidget::paintGL()
{
	setCamera(0);
	updateShader();

	// 绘制船体和收银台
	glPushMatrix();
	glTranslatef(0.0f, 10.0f, -14.0f);
	glScalef(2.0f, 2.0f, 2.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	drawCashier();
	glPopMatrix();

	// 绘制地板
	useTexture = 1;
	setDiffuseMap("img/floor.png");
	drawCuboid(0.0f, -0.5f, 0.0f, 480.0f, 1.0f, 500.0f);
	// 绘制餐厅内墙面	
	setDiffuseMap("img/wallpaper1.png");
	drawCuboid(0.0f, 75.0f, -90.5f, 360.0f, 150.0f, 1.0f);
	setDiffuseMap("img/wallpaper2.png");
	drawCuboid(0.0f, 150.5f, -140.0f, 360.0f, 1.0f, 100.0f);

	// 绘制杂物箱
	setDiffuseMap("img/box.png");
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
	setDiffuseMap("img/skybox1.png");
	glPushMatrix();
	//glTranslatef(0.0f, 349.0f, 0.0f);
	glTranslatef(viewPos[0], viewPos[1], viewPos[2]);
	//glScalef(1410.0f, 700.0f, 1610.0f);
	glScalef(2500.0f, 1500.0f, 2500.0f);
	drawSkybox();
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

void MyGLWidget::resizeGL(int width, int height)
{
	// 调整窗口时保持画面比例不变
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float range = 45.0f;
	gluPerspective(range, 1.0 * width / height, 0.1f, 2000.0f);
	// 请求重新绘制窗口内容
	update();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
	// 检测用户是否移动摄像机 设置摄像机位置
	setCamera(event->key());
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

	// 创建一个着色程序 并把顶点着色器和片元着色器附加到该程序上
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// 绑定完成后即可删除着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 启用着色程序
	glUseProgram(program);

	// 设置光源的环境光照、漫反射光照和镜面光照 并传递到片元着色器中
	GLint lightAmbientLoc = glGetUniformLocation(program, "light.ambient");
	GLint lightDiffuseLoc = glGetUniformLocation(program, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(program, "light.specular");

	//glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
	//glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	//glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightAmbientLoc, 0.4f, 0.4f, 0.4f);
	glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(lightSpecularLoc, 0.5f, 0.5f, 0.5f);
}

void MyGLWidget::updateShader()
{
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
}

/* ******************** 状态信息设置函数 ******************** */

void MyGLWidget::setDiffuseMap(const char* imagePath)
{
	GLuint textureID = 0;
	int index = 0;
	// 如果纹理贴图之前未加载过 则创建新的纹理对象 并将路径与纹理对象的映射关系保存到 imgpath2texture 中
	if (!imgpath2texture.count(imagePath)) {
		GLint maxTextureUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		if (imgpath2texture.size() >= maxTextureUnits) {	// 纹理贴图数量已达上限 添加失败
			std::cout << "The number of texture maps has reached the maximum limit, fail to a new texture map!" << std::endl;
			return;
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		index = imgpath2texture.size();
		imgpath2index[imagePath] = index;		// 记录图像对应的编号
		imgpath2texture[imagePath] = textureID;	// 记录图像对应的纹理贴图

		QImage image = QImage(imagePath);
		// 将QT读取的图像转化为OpenGL纹理需要的格式
		// 比如把bgr换成rgb的形式 以及对图像进行一次翻转等等
		image = QGLWidget::convertToGLFormat(image);

		// 获取图片的高度和宽度
		int width, height;
		width = image.width();
		height = image.height();

		// 为当前绑定的纹理对象附加上纹理图像
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
		glGenerateMipmap(GL_TEXTURE_2D);

		// 设置纹理环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// 设置多级渐远纹理过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else {
		textureID = imgpath2texture[imagePath];
		index = imgpath2index[imagePath];
	}
	glUniform1i(glGetUniformLocation(program, "material.diffuse"), index);
	// 绑定纹理对象到纹理单元
	glActiveTexture(GL_TEXTURE0 + index);  // 设置纹理单元为 GL_TEXTURE0 + index
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(program, "useTexture"), useTexture);
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
	float speed = 7.0f;
	float xRate2 = -zRate, zRate2 = xRate; // 向后的方向 即向右的方向顺时针旋转90度
	const float RANGE = 1000.0f;	// 限制移动范围
	switch (key)
	{
		// 上下左右方向键的功能
		// 镜头向上移动
	case Qt::Key_Up: if(abs(viewPos[1] + speed) > RANGE / 2) break;
		setViewPos(viewPos[0], viewPos[1] + speed, viewPos[2]);
		setViewDesPos(viewDesPos[0], viewDesPos[1] + speed, viewDesPos[2]); break;
		// 镜头向下移动
	case Qt::Key_Down: if(abs(viewPos[1] - speed) > RANGE / 2) break;
		setViewPos(viewPos[0], viewPos[1] - speed, viewPos[2]);
		setViewDesPos(viewDesPos[0], viewDesPos[1] - speed, viewDesPos[2]); break;
		// 镜头向左移动
	case Qt::Key_Left: if(abs(viewPos[0] - speed * xRate) > RANGE || abs(viewPos[2] - speed * zRate) > RANGE) break;
		setViewPos(viewPos[0] - speed * xRate, viewPos[1], viewPos[2] - speed * zRate);
		setViewDesPos(viewDesPos[0] - speed * xRate, viewDesPos[1], viewDesPos[2] - speed * zRate); break;
		// 镜头向右移动
	case Qt::Key_Right: if (abs(viewPos[0] + speed * xRate) > RANGE || abs(viewPos[2] + speed * zRate) > RANGE) break;
		setViewPos(viewPos[0] + speed * xRate, viewPos[1], viewPos[2] + speed * zRate);
		setViewDesPos(viewDesPos[0] + speed * xRate, viewDesPos[1], viewDesPos[2] + speed * zRate); break;
		// wsad键的功能
		// 镜头向前移动
	case Qt::Key_W: if (abs(viewPos[0] - speed * xRate2) > RANGE || abs(viewPos[2] - speed * zRate2) > RANGE) break;
		setViewPos(viewPos[0] - speed * xRate2, viewPos[1], viewPos[2] - speed * zRate2);
		setViewDesPos(viewDesPos[0] - speed * xRate2, viewDesPos[1], viewDesPos[2] - speed * zRate2); break;
		// 镜头向后移动
	case Qt::Key_S: if (abs(viewPos[0] + speed * xRate2) > RANGE || abs(viewPos[2] + speed * zRate2) > RANGE) break;
		setViewPos(viewPos[0] + speed * xRate2, viewPos[1], viewPos[2] + speed * zRate2);
		setViewDesPos(viewDesPos[0] + speed * xRate2, viewDesPos[1], viewDesPos[2] + speed * zRate2); break;
	case Qt::Key_A: rotateViewDes(false); break;	// 镜头向左旋转45度
	case Qt::Key_D: rotateViewDes(true); break;	// 镜头向右旋转45度
	case Qt::Key_Escape: close();
	default: break;
	}

	// 设置摄像机位置
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(viewPos[0], viewPos[1], viewPos[2], viewDesPos[0], viewDesPos[1], viewDesPos[2], 0, 1, 0);
}

/* ******************** 各部件绘制函数 ******************** */

void MyGLWidget::drawCashier()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 船体
	useTexture = 1;	// 使用漫反射纹理贴图
	setDiffuseMap("img/boat1_2.png");
	drawCuboid(8 + 30 - 0.5f, 0.0f, 0.0f, 1.0f, 10.0f, 16.0f);	// 后钢板
	drawCuboid(8 + 0.5f, 0.0f, 0.0f, 1.0f, 10.0f, 16.0f);		// 前钢板
	drawCuboid(8 + 15.0f, 0.0f, 7.5f, 28.0f, 10.0f, 1.0f);		// 左钢板
	drawCuboid(8 + 15.0f, 0.0f, -7.5f, 28.0f, 10.0f, 1.0f);		// 右钢板
	setDiffuseMap("img/boat1_2_rotate.png");
	drawSemicylinder(8.0f, 0.0f, 0.0f, 8.0f, 5.0f, 8.0f);		// 船头
	setDiffuseMap("img/boat2.png");
	drawCuboid(8 + 15.0f, -4.5f, 0.0f, 28.0f, 1.0f, 14.0f);		// 底部钢板
	drawCuboid(0.0f, 1.0f, 0.0f, 2.0f, 12.0f, 2.0f);			// 船头柱子

	//setObjectColor(0.76f, 0.82f, 0.94f);	// 将颜色设置为浅蓝色 (193, 210, 240)
	drawCuboid(8 + 30 - 1.5f, 3.5f, 0.0f, 1.0f, 1.0f, 14.0f);		// 后突起
	drawCuboid(8 + 1.5f, 3.5f, 0.0f, 1.0f, 1.0f, 14.0f);			// 前突起
	drawCuboid(8 + 15.0f, 3.5f, 7.5f - 1.0f, 26.0f, 1.0f, 1.0f);	// 左突起
	drawCuboid(8 + 15.0f, 3.5f, -7.5f + 1.0f, 26.0f, 1.0f, 1.0f);	// 右突起

	drawCuboid(8 + 30 - 4.5f, 1.0f, 0.0f, 8.0f, 1.0f, 14.0f);	// 尾部架子

	// 收银台
	// setObjectColor(0.1f, 0.2f, 0.2f);	// 将收银台颜色设置为偏蓝绿的深灰色
	setDiffuseMap("img/cashier.png");
	drawCuboid(8.0f - 2.0f, 6.0f, 0.0f, 4.0f, 2.0f, 8.0f);	// 平放结构
	drawCuboid(8.0f - 5.0f, 7.0f, 0.0f, 2.0f, 4.0f, 8.0f);	// 直立结构
	drawCuboid(8.0f - 5.0f, 9.5f, 0.0f, 1.0f, 1.0f, 1.0f);	// 小直立结构
	drawCuboid(8.0f - 5.0f, 10.5f, 0.0f, 1.0f, 1.0f, 3.0f);	// 小平放结构
	useTexture = 0; // 不使用漫反射纹理贴图
}

void MyGLWidget::drawRestaurant() {

	// 绘制底部、门板和屋顶下部
	useTexture = 1;	// 使用漫反射纹理贴图
	setDiffuseMap("img/wood3.png");	// 指定图片
	glPushMatrix();
	glBindVertexArray(quadsVAO);
	updateShader();	// 每次进行绘制之前要调用该方法
	glDrawArrays(GL_QUADS, 0, 4 * (22 + 18 + 24));
	glBindVertexArray(0);
	glPopMatrix();

	// 绘制屋顶
	drawSemicylinder(0.0f, 70.0f, -5.0f, 34.0f, 20.0f, 19.0f, -90.0f, 0.0f, 0.0f, 1.0f);

	// 绘制柱子（半径为4，高为60的圆柱体） 注意后面的柱子先绘制
	drawCylinder(34.0f, 40.0f, -24.0f, 4.0f, 60.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 柱子2
	drawCylinder(-34.0f, 40.0f, -24.0f, 4.0f, 60.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 柱子3

	// 绘制两旁的柱子
	drawCylinder(34.0f, 40.0f, 14.0f, 4.0f, 60.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 柱子1
	drawCylinder(-34.0f, 40.0f, 14.0f, 4.0f, 60.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);	// 柱子4

	// 绘制圆环
	drawTorus(15, 23, 34.0, 70.0f, -5.0f, 1.0f, 1.0f, 1.0f, 90.0f, 0.0f, 1.0f, 0.0f);
	drawTorus(15, 23, -34.0, 70.0f, -5.0f, 1.0f, 1.0f, 1.0f, 90.0f, 0.0f, 1.0f, 0.0f);

	useTexture = 0;
}

void MyGLWidget::drawDesk()
{
	// 桌面
	useTexture = 1;
	setDiffuseMap("img/desk1.png");
	drawCylinder(0.0f, 21.0f, 0.0f, 16.0f, 3.0f, 16.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	// 支柱
	setDiffuseMap("img/desk2.png");
	drawCylinder(0.0f, 11.0f, 0.0f, 2.0f, 20.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	// 地盘
	drawCylinder(0.0f, 0.0f, 0.0f, 5.0f, 2.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	// 六个装饰
	useTexture = 0;
	setObjectColor(192 / 255.0f, 0.0f, 0.0f);
	drawCone(-19.0f, 21.0f, 0.0f);
	drawCone(-9.5f, 21.0f, 16.45f, 1.0f, 1.0f, 1.0f, 60.0f, 0.0f, 1.0f, 0.0f);
	drawCone(9.5f, 21.0f, 16.45f, 1.0f, 1.0f, 1.0f, 120.0f, 0.0f, 1.0f, 0.0f);
	drawCone(19.0f, 21.0f, 0.0f, 1.0f, 1.0f, 1.0f, 180.0f, 0.0f, 1.0f, 0.0f);
	drawCone(9.5f, 21.0f, -16.45f, 1.0f, 1.0f, 1.0f, 240.0f, 0.0f, 1.0f, 0.0f);
	drawCone(-9.5f, 21.0f, -16.45f, 1.0f, 1.0f, 1.0f, 300.0f, 0.0f, 1.0f, 0.0f);
}

void MyGLWidget::drawChair()
{
	// 绘制桶上沿
	useTexture = 1;
	setDiffuseMap("img/chair1.png");
	drawTorus(2.5f, 3.0f, 0.0f, 12.0f, 0.0f, 1.0f, 1.0f, 1.0f, 90.0f, 1.0f, 0.0f, 0.0f);
	drawTorus(2.5f, 3.0f, 0.0f, 12.0f, 0.0f, 1.0f, 1.0f, 1.0f, 90.0f, -1.0f, 0.0f, 0.0f);

	//绘制水桶
	drawFrustum1(0.0f, 10.0f, 0.0f);
	drawFrustum2(0.0f, 7.0f, 0.0f);
	glPushMatrix();
	glRotatef(177.0f, 0.0f, 1.0f, 0.0f);
	drawFrustum2(0.0f, 5.0f, 0.0f, 1.0f, 1.0f, 1.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	drawFrustum1(0.0f, 2.0f, 0.0f, 1.0f, 1.0f, 1.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	glPopMatrix();
	setDiffuseMap("img/chair1_rotate.png");
	drawCylinder(0.0f, 6.0f, 0.0f, 4.0f, 2.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	// 绘制桶的铁环
	setDiffuseMap("img/chair2.png");
	drawTorus(3.1f, 3.7f, 0.0f, 10.0f, 0.0f, 1.0f, 1.0f, 1.0f, 90.0f, 1.0f, 0.0f, 0.0f);
	drawTorus(3.1f, 3.7f, 0.0f, 10.0f, 0.0f, 1.0f, 1.0f, 1.0f, 90.0f, -1.0f, 0.0f, 0.0f);

	drawTorus(3.1f, 3.7f, 0.0f, 2.0f, 0.0f, 1.0f, 1.0f, 1.0f, 90.0f, -1.0f, 0.0f, 0.0f);
	drawTorus(3.1f, 3.7f, 0.0f, 2.0f, 0.0f, 1.0f, 1.0f, 1.0f, 90.0f, 1.0f, 0.0f, 0.0f);
}

void MyGLWidget::drawSkybox()
{
	glBindVertexArray(skyboxVAO);
	updateShader();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

/* ******************** 基本形状绘制函数 ******************** */

// 由其他draw函数调用 实现统一的图形绘制功能
void MyGLWidget::_drawShape(GLuint givenVAO, GLenum mod, GLsizei count,
	float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glScalef(sx, sy, sz);
	glRotatef(angle, rx, ry, rz);

	updateShader();

	glBindVertexArray(givenVAO);
	glDrawArrays(mod, 0, count);
	glBindVertexArray(0);
	glPopMatrix();
}

void MyGLWidget::drawCuboid(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	_drawShape(cubeVAO, GL_TRIANGLES, 12 * 3,
		tx, ty, tz, sx, sy, sz, angle, rx, ry, rz);
}

void MyGLWidget::drawSemicylinder(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	_drawShape(semicylinderVAO, GL_TRIANGLES, vertexNumOfSemicylinder,
		tx, ty, tz, sx, sy, sz, angle, rx, ry, rz);
}

void MyGLWidget::drawCylinder(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	_drawShape(cylinderVAO, GL_TRIANGLES, vertexNumOfCylinder,
		tx, ty, tz, sx, sy, sz, angle, rx, ry, rz);
}

void MyGLWidget::drawFrustum1(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	_drawShape(frustumVAO1, GL_TRIANGLES, vertexNumOfFrustum1,
		tx, ty, tz, sx, sy, sz, angle, rx, ry, rz);
}

void MyGLWidget::drawFrustum2(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	_drawShape(frustumVAO2, GL_TRIANGLES, vertexNumOfFrustum2,
		tx, ty, tz, sx, sy, sz, angle, rx, ry, rz);
}

void MyGLWidget::drawCone(float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz)
{
	_drawShape(coneVAO, GL_TRIANGLES, vertexNumOfCone,
		tx, ty, tz, sx, sy, sz, angle, rx, ry, rz);
}

// 绘制三维立体半圆环 innerRadius:内环半径，OutEadius：外环半径，Sides和Rings都是切割数
void MyGLWidget::drawTorus(float innerRadius, float outRadius,
	float tx, float ty, float tz, float sx, float sy, float sz,
	float angle, float rx, float ry, float rz) {
	const int SIDES = 100;
	const int RINGS = 100;
	float key = innerRadius * outRadius;
	if (!radius2torusVAO.count(key))
		initTorus(innerRadius, outRadius);
	GLuint torusVAO = radius2torusVAO[key];

	_drawShape(torusVAO, GL_QUAD_STRIP, 2 * (SIDES + 1) * (RINGS + 1),
		tx, ty, tz, sx, sy, sz, angle, rx, ry, rz);
}

/* ******************** 初始化相关函数 ******************** */

void MyGLWidget::initCube()
{
	// 创建VAO并绑定
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
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
	glBindVertexArray(0);
}

void MyGLWidget::initSemicylinder()
{
	std::vector<GLfloat> semicircleVertices;    // 保存了半圆顶点位置
	std::vector<GLfloat> semicylinderVertices;  // 保存了半圆柱体顶点信息 包括位置和法向量

	// 将initialTriangle中前两个三角形的顶点信息保存到semisircleVertices中
	semicircleVertices.resize(2 * 3 * 3);
	for (int i = 0; i < 2 * 3 * 3; i++)
	{
		semicircleVertices[i] = initialTriangle[i];
	}

	// 迭代细分三角 函数调用完成后semicircleVertices中保存了绘制半圆的顶点信息
	// 半圆由许多细分三角形组成 每个三角形以原点为起点 按顺时针方向保存顶点信息
	subdivideTriangle(ITERATIONS, semicircleVertices);

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
	generateSideVertices(semicircleVertices, upperVertices, lowerVertices, sideVertices);

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

	// 创建VAO并绑定
	glGenVertexArrays(1, &semicylinderVAO);
	glBindVertexArray(semicylinderVAO);
	// 为VBO创建一个缓冲区对象
	glGenBuffers(1, &semicylinderVBO);
	// 利用vertices数组中的内容更新VBO的信息
	glBindBuffer(GL_ARRAY_BUFFER, semicylinderVBO);
	glBufferData(GL_ARRAY_BUFFER, semicylinderVertices.size() * sizeof(GLfloat), &semicylinderVertices[0], GL_STATIC_DRAW);
	// 指定顶点属性数组的数据格式和位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 解绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyGLWidget::initCylinder()
{
	std::vector<GLfloat> circleVertices;    // 保存了圆顶点位置
	std::vector<GLfloat> cylinderVertices;  // 保存了圆柱体顶点信息 包括位置和法向量

	// 将initialTriangle中的顶点信息保存到circleVertices中
	circleVertices.resize(4 * 3 * 3);
	for (int i = 0; i < 4 * 3 * 3; i++)
	{
		circleVertices[i] = initialTriangle[i];
	}

	// 迭代细分三角 函数调用完成后circleVertices中保存了绘制圆的顶点信息
	// 圆由许多细分三角形组成 每个三角形以原点为起点 按顺时针方向保存顶点信息
	subdivideTriangle(ITERATIONS, circleVertices, 1);

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

	generateSideVertices(circleVertices, upperVertices, lowerVertices, sideVertices, 0.16f);

	cylinderVertices.insert(cylinderVertices.end(), upperVertices.begin(), upperVertices.end());
	cylinderVertices.insert(cylinderVertices.end(), lowerVertices.begin(), lowerVertices.end());
	cylinderVertices.insert(cylinderVertices.end(), sideVertices.begin(), sideVertices.end());

	// 创建VAO并绑定
	glGenVertexArrays(1, &cylinderVAO);
	glBindVertexArray(cylinderVAO);

	glGenBuffers(1, &cylinderVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
	glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(GLfloat), &cylinderVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 解绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyGLWidget::initCone()
{
	std::vector<GLfloat> taperVertices;     // 保存了圆锥顶点信息 包括位置和法向量
	std::vector<GLfloat> taperdownVertices; // 保存了圆锥底部圆顶点位置

	// 将temp中的顶点信息保存到taperdownVertices中
	taperdownVertices.resize(4 * 3 * 3);
	for (int i = 0; i < 4 * 3 * 3; i++)
	{
		taperdownVertices[i] = initialTriangleVertical[i];
	}
	subdivideTriangle(ITERATIONS, taperdownVertices, 1);

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

		addVertex(va2[0], va2[1], va2[2], sideVertices);		// 位置
		addVertex(v2[0] + 0.25f, v2[1], v2[2], sideVertices);	// 法向量
		addVertex(va3[0], va3[1], va3[2], sideVertices);		// 位置
		addVertex(v3[0] + 0.25f, v3[1], v3[2], sideVertices);	// 法向量
		addVertex(4.0f, 0.0f, 0.0f, sideVertices);				// 位置
		addVertex((v2[0] + v3[0]) / 2 + 0.25f, (v2[1] + v3[1]) / 2, (v2[2] + v3[2]) / 2, sideVertices); //法向量
	}

	taperVertices.insert(taperVertices.end(), lowerVertices.begin(), lowerVertices.end());
	taperVertices.insert(taperVertices.end(), sideVertices.begin(), sideVertices.end());

	vertexNumOfCone = taperVertices.size() / 2 / 3;

	// 创建VAO并绑定
	glGenVertexArrays(1, &coneVAO);
	glBindVertexArray(coneVAO);
	// 为VBO创建一个缓冲区对象
	glGenBuffers(1, &coneVBO);
	// 利用vertices数组中的内容更新VBO的信息
	glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
	glBufferData(GL_ARRAY_BUFFER, taperVertices.size() * sizeof(GLfloat), &taperVertices[0], GL_STATIC_DRAW);
	// 指定顶点属性数组的数据格式和位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 解绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyGLWidget::initFrustum(GLuint& givenVAO, GLuint& givenVBO, int& vertexNum,
	float upperRadius, float lowerRadius, float height)
{
	std::vector<GLfloat> coneupVertices;    // 保存了圆台顶部圆顶点位置信息
	std::vector<GLfloat> conedownVertices;  // 保存了圆台底部圆顶点位置信息
	std::vector<GLfloat> coneVertices;      // 保存了圆台体顶点信息 包括位置和法向量

	coneupVertices.resize(4 * 3 * 3);
	conedownVertices.resize(4 * 3 * 3);
	for (int i = 0; i < 4 * 3 * 3; i++)
	{
		// 指定上下底面两个圆的半径
		coneupVertices[i] = initialTriangle[i] * upperRadius;
		conedownVertices[i] = initialTriangle[i] * lowerRadius;
	}

	subdivideTriangle(ITERATIONS, coneupVertices, upperRadius);
	subdivideTriangle(ITERATIONS, conedownVertices, lowerRadius);

	vertexNum = coneupVertices.size() / 3 * (1 + 2) + conedownVertices.size() / 3;
	vector<GLfloat> upperVertices; // 上圆
	vector<GLfloat> lowerVertices; // 下圆
	vector<GLfloat> sideVertices; // 侧面

	float deltaAngle = 360.0 / (coneupVertices.size() / 9);

	for (int i = 0; i < coneupVertices.size() / 3; i++)
	{
		// 顶点位置
		upperVertices.push_back(coneupVertices[i * 3]);
		upperVertices.push_back(coneupVertices[i * 3 + 1] + height);
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

		float k = (lowerRadius - upperRadius) / height;	// 侧面斜率的倒数

		addVertex(va2[0], va2[1], va2[2], sideVertices);
		addVertex(vu2[0], upperRadius * k, vu2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(1.0f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(vd2[0], lowerRadius * k, vd2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(vu3[0], upperRadius * k, vu3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(1.0f);

		addVertex(va3[0], va3[1], va3[2], sideVertices);
		addVertex(vu3[0], upperRadius * k, vu3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(1.0f);
		addVertex(vb2[0], vb2[1], vb2[2], sideVertices);
		addVertex(vd2[0], lowerRadius * k, vd2[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * i);
		sideVertices.push_back(0.0f);
		addVertex(vb3[0], vb3[1], vb3[2], sideVertices);
		addVertex(vd3[0], lowerRadius * k, vd3[2], sideVertices);
		sideVertices.push_back(1.0f / numOfTriangles * (i + 1));
		sideVertices.push_back(0.0f);
	}
	coneVertices.insert(coneVertices.end(), upperVertices.begin(), upperVertices.end());
	coneVertices.insert(coneVertices.end(), lowerVertices.begin(), lowerVertices.end());
	coneVertices.insert(coneVertices.end(), sideVertices.begin(), sideVertices.end());

	// 创建VAO并绑定
	glGenVertexArrays(1, &givenVAO);
	glBindVertexArray(givenVAO);
	glGenBuffers(1, &givenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, givenVBO);
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
	// 解绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyGLWidget::initTorus(float innerRadius, float OutRadius, const int SIDES, const int RINGS)
{
	GLuint torusVBO;
	GLuint torusVAO;
	// 之前没有生成过innerRadius和OutRadius对应的VAO 则生成新的VAO并记录半径与VAO的映射关系
	std::vector<GLfloat> torusVertices(8 * (SIDES + 1) * (RINGS + 1) * 2);
	float alphaStep = (float)(PI / SIDES); // 只画半圆环,不用乘2
	float BetaStep = (float)(2 * PI / SIDES);
	float beta = 0;
	float alpha = 0;
	float x0, x1, y0, y1, z0, z1, x2, y2, z2;
	float nor[3];
	float r = (OutRadius - innerRadius) / 2; // 圆环的半径
	int index = 0;
	for (int i = 0; i <= SIDES; i++) {
		alpha = i * alphaStep;
		for (int j = 0; j <= RINGS; j++) {
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
			torusVertices[index++] = x0;
			torusVertices[index++] = y0;
			torusVertices[index++] = z0;
			torusVertices[index++] = nor[0];
			torusVertices[index++] = nor[1];
			torusVertices[index++] = nor[2];
			torusVertices[index++] = ((float)2 * i / RINGS);
			torusVertices[index++] = (float)j / RINGS;

			nor[0] = (float)(cos(alpha + alphaStep) * cos(beta));
			nor[1] = (float)(sin(alpha + alphaStep) * cos(beta));
			nor[2] = (float)(sin(beta));
			torusVertices[index++] = x1;
			torusVertices[index++] = y1;
			torusVertices[index++] = z1;
			torusVertices[index++] = nor[0];
			torusVertices[index++] = nor[1];
			torusVertices[index++] = nor[2];
			torusVertices[index++] = ((float)2 * i / RINGS);
			torusVertices[index++] = (float)j / RINGS;
		}
	}

	// 生成VAO和VBO并绑定
	glGenVertexArrays(1, &torusVAO);
	glBindVertexArray(torusVAO);
	glGenBuffers(1, &torusVBO);
	glBindBuffer(GL_ARRAY_BUFFER, torusVBO);
	// 把之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, torusVertices.size() * sizeof(GLfloat), &torusVertices[0], GL_STATIC_DRAW);
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// 记录环半径与VAO的映射关系
	float key = innerRadius * OutRadius;
	radius2torusVAO[key] = torusVAO;
}

void MyGLWidget::initRestaurant() {

	GLfloat vertices_quads[(22 + 18 + 24) * 4 * 8];

	memcpy(vertices_quads, restaurant_bottom, 22 * 4 * 8 * sizeof(float));						// 餐厅底部
	memcpy(vertices_quads + 22 * 4 * 8, restaurant_side, 18 * 4 * 8 * sizeof(float));			// 餐厅侧面和门板
	memcpy(vertices_quads + (22 + 18) * 4 * 8, restaurant_side2, 24 * 4 * 8 * sizeof(float));	// 餐厅侧面屋顶下的上半部分

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

void MyGLWidget::initSkybox()
{
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