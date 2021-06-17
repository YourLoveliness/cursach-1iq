#include <GL/glut.h>
#include <math.h>
#include <glaux.h>

#pragma comment(lib,"GLAUX.LIB")
#pragma comment (lib, "legacy_stdio_definitions.lib")

#define PI 3.1415927

//угол поворота камеры
float angle = 0.0;
//координаты вектора направления движения камеры
float lx = 0.0f, lz = -1.0f;
//позиция камеры по XZ
float x = 0.0f, z = 5.0f;
//ключи статуса камеры. Переменные инициализируются нулевыми значениями, когда клавиши не нажаты
float deltaAngle = 0.0f;
float deltaMove = 0;
//тектурирование
unsigned int textures[2];
void TextureLoad()
{
	AUX_RGBImageRec* texture1 = auxDIBImageLoadA("b1.bmp"), * texture2 = auxDIBImageLoadA("cone.bmp");
	glGenTextures(2, &textures[0]);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture1->sizeX, texture1->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, texture1->data);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture2->sizeX, texture2->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, texture2->data);
}
//текущее состояние зрения
void changeSize(int w, int h) {
	//предотвращение деления на ноль
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	//используем матрицу проекции
	glMatrixMode(GL_PROJECTION);
	//обнуляем матрицу
	glLoadIdentity();
	//установить параметры вьюпорта
	glViewport(0, 0, w, h);
	//установить корректную перспективу
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	//вернуться к матрице проекции
	glMatrixMode(GL_MODELVIEW);
}
//позиция камеры при перемещении
void computePos(float deltaMove)
{
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}
//направление\угол камеры при перемещении
void computeDir(float deltaAngle)
{
	angle += deltaAngle;
	lx = sin(angle);
	lz = -cos(angle);
}
//шляпа карандаша
void draw_top(GLfloat radius, GLfloat height)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	GLfloat x = 0.0;
	GLfloat y = 0.0;
	GLfloat angle = 0.0;
	GLfloat angle_stepsize = 0.1;
	glBegin(GL_POLYGON);
	angle = 0.0;
	while (angle < 2 * PI) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glTexCoord3f(sin(angle) * radius, cos(angle) * radius, 0);
		glVertex3f(x, y, height);
		angle = angle + angle_stepsize;
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//тело карандаша
void draw_body(GLfloat radius, GLfloat height)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	GLdouble x = 0.0;
	GLdouble y = 0.0;
	GLdouble angle = 0.0;
	GLdouble angle_stepsize = 0.1;
	glBegin(GL_QUAD_STRIP);

	while (angle < 2 * PI)
	{
		x = radius * cos(angle);
		y = radius * sin(angle);
		glTexCoord3d(sin(angle) * radius, cos(angle) * radius, 0);
		glVertex3f(x, y, height);
		glVertex3f(x, y, 0.0);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glVertex3f(radius, 0.0, 0.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//конец карандаша
void draw_end(GLfloat radius, GLfloat height)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_TRIANGLE_FAN);
	//конус
	glVertex3f(0, 0, height);
	for (int angle = 0; angle < 360; angle++)
	{
		glTexCoord3f(sin(angle) * radius, cos(angle) * radius, 0);
		glVertex3f(sin(angle) * radius, cos(angle) * radius, 0);
	}
	//основание
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (int angle = 0; angle < 360; angle++)
	{
		glNormal3f(0, -1, 0);
		glTexCoord3f(sin(angle) * radius, cos(angle) * radius, 0);
		glVertex3f(sin(angle) * radius, cos(angle) * radius, 0);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//карандаш
void Pencil()
{
	glTranslated(0, 0, 0);
	glRotated(270, 1, 0, 0);
	draw_end(0.05, -0.35);
	draw_body(0.05, 1.0);
	draw_top(0.05, 1.0);
}

void renderScene(void)
{
	
	if (deltaMove)
		computePos(deltaMove);
	if (deltaAngle)
		computeDir(deltaAngle);
	//очистить буфер цвета и глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// обнулить трансформацию
	glLoadIdentity();
	// установить камеру
	gluLookAt(x, 1.5f, z,
		x + lx, 1.0f, z + lz,
		0.0f, 1.0f, 0.0f);
	//нарисуем "землю"
	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);// полигон с коондинатами
	glVertex3f(-100.0f, -0.5f, -100.0f);
	glVertex3f(-100.0f, -0.5f, 100.0f);
	glVertex3f(100.0f, -0.5f, 100.0f);
	glVertex3f(100.0f, -0.5f, -100.0f);
	glEnd();
	Pencil();
	
	glutSwapBuffers();
}
//движение по нажатию клавиш
void pressKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		deltaAngle = -0.0015f;
		break;
	case GLUT_KEY_RIGHT:
		deltaAngle = 0.0015f;
		break;
	case GLUT_KEY_UP:
		deltaMove = 0.03f;
		break;
	case GLUT_KEY_DOWN:
		deltaMove = -0.03f;
		break;
	}
}
//обнуление скорости камеры при отпускании кнопок
void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
		deltaAngle = 0.0f;
		break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		deltaMove = 0;
		break;
	}
}
//массивы для света

float ambient[] = { 1.0, 1.0, 1.0, 1.0 };
float color0[] = { 1.0, 0.5, 0.5, 1.0 };
float color1[] = { 1.0, 1.0, 1.0, 1.0 };
float color2[] = { 1.0, 0.3, 0.3, 1.0 };

//#1
GLfloat light0_diffuse[] = { 0.4, 0.7, 0.2 };
GLfloat light0_position[] = { 0.0, -0.7, 1.0, 1.0 };
//#2
GLfloat light1_diffuse[] = { 0.0, 0.0, 1.0 };
GLfloat light1_position[] = { 0.5 * cos(4 * PI / 3), 0.5 * sin(4 * PI / 3), 1.0, 1.0 };
//#3
GLfloat light2_diffuse[] = { 0.4, 0.7, 0.2 };
GLfloat light2_position[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat light2_spot_direction[] = { 0.0, 0.0, -1.0 };

void lighningControl(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 0x31:
		//обычный направленный на модельку свет
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);


		glEnable(GL_LIGHT0);

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

		break;
	case 0x32:
		//"мягкий" свет
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color1);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT2);

		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.4);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.8);

		break;
	case 0x33:
		//направленный снизу вверх у основания свет
		//меняет цвет фона
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color2);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);

		glEnable(GL_LIGHT2);

		glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
		glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 45);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_spot_direction);

		break;

	case 0x35:
		//отключаю
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT);
		break;
	}
}
int main(int argc, char** argv) {

	// инициализация GLUT и создание окна

	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("курс");

	// регистрация вызовов
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutSpecialFunc(pressKey);
	glutKeyboardFunc(lighningControl);

	// Новые функции для регистрации
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);

	//загружаю текстуры
	TextureLoad();
	//инициализация функции света
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_DEPTH_TEST);
	// главный цикл
	glutMainLoop();

	return 1;
}