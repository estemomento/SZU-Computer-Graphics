/*
 *        Computer Graphics Course - Shenzhen University
 *                 Mid-term Assignment - Tetris
 *             Programmed by ALEKO LAU - 2016.10.24
 * ============================================================
 *
 * - 已实现功能如下：
 * -  1) 绘制棋盘格
 * -  2) 键盘左/右/下键控制方块的移动，上键旋转方块
 * -  3) 随机生成方块并赋上不同的颜色
 * -  4) 方块之间的碰撞检测
 * -  5) 方块的自动向下移动
 * -  6) 棋盘格中每一行填充满之后自动消除 (clearrow)
 * -  7) 积分系统，下落速度随积分增加而加快。
 * -  8) 读取外部文件记录生涯最高分
 * -  9) 键盘快捷键控制重启游戏
 * - 10) 开场、结束、消除音效
 *
 */

#include "include/Angel.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <mmsystem.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

int starttime ;			// 控制方块向下移动时间
int rotation = 0;		// 控制当前窗口中的方块旋转
int random;             // 随机生成方块变量
int score;				// 当前游戏积分
int bestscore;			// 游戏最高分
vec2 tile[4];			// 表示当前窗口中的方块
int xsize = 400;		// 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;
int FALL_TIME = 1000;
bool halt = false;		// 游戏停止变量
// 二维数组表示所有可能出现的方块和方向。
vec2 allRotationsOshape[1][4] =
							  {{vec2(0, 0), vec2(-1,0), vec2(0,-1), vec2(-1,-1)}};  //   "O"
vec2 allRotationsIshape[2][4] =
							  {{vec2(0, 0), vec2(1, 0), vec2(-2, 0),vec2(-1, 0)},	//   "I"
							   {vec2(0, 1), vec2(0, 0), vec2(0,-1), vec2(0, -2)}};  
vec2 allRotationsSshape[2][4] =
							  {{vec2(0, 0), vec2(1, 0), vec2(0,-1), vec2(-1,-1)},	//   "S"
							   {vec2(0, 1), vec2(0, 0), vec2(1,-1), vec2(1,  0)}};  
vec2 allRotationsZshape[2][4] =
							  {{vec2(0, 0), vec2(-1,0), vec2(0,-1), vec2(1, -1)},	//   "Z"
							   {vec2(1, 1), vec2(0, 0), vec2(0,-1), vec2(1,  0)}};  
vec2 allRotationsLshape[4][4] =
							  {{vec2(0, 0), vec2(-1,0), vec2(1, 0), vec2(-1,-1)},	//   "L"
							   {vec2(0, 1), vec2(0, 0), vec2(0,-1), vec2(1, -1)},   
							   {vec2(1, 1), vec2(-1,0), vec2(0, 0), vec2(1,  0)},   
							   {vec2(-1,1), vec2(0, 1), vec2(0, 0), vec2(0, -1)}};
vec2 allRotationsJshape[4][4] = 
							  {{vec2(0, 0), vec2(-1,0), vec2(1, 0), vec2(1, -1)},	//   "J"
							   {vec2(0, 1), vec2(1, 1), vec2(0,-1), vec2(0,  0)},   
							   {vec2(-1,1), vec2(-1,0), vec2(0, 0), vec2(1,  0)},   
							   {vec2(-1,-1),vec2(0, 1), vec2(0, 0), vec2(0, -1)}};
vec2 allRotationsTshape[4][4] = 
							  {{vec2(0, 0), vec2(-1,0), vec2(1, 0), vec2(0, -1)},	//   "T"
							   {vec2(0, 1), vec2(0, 0), vec2(0,-1), vec2(1,  0)},   
							   {vec2(0, 1), vec2(-1,0), vec2(0, 0), vec2(1,  0)},   
							   {vec2(-1,0), vec2(0, 1), vec2(0, 0), vec2(0, -1)}};


// 绘制窗口的颜色变量
vec4 orange  = vec4(1.0, 0.5, 0.0, 1.0);
vec4 cyan    = vec4(0.0, 1.0, 1.0, 1.0);
vec4 magenta = vec4(1.0, 0.0, 1.0, 1.0);
vec4 yellow  = vec4(1.0, 1.0, 0.0, 1.0);
vec4 red     = vec4(1.0, 0.0, 0.0, 1.0);
vec4 green   = vec4(0.0, 1.0, 0.0, 1.0);
vec4 blue    = vec4(0.0, 0.0, 1.0, 1.0);
vec4 white   = vec4(1.0, 1.0, 1.0, 1.0);
vec4 gray    = vec4(0.4, 0.4, 0.4, 1.0);
vec4 black   = vec4(0.0, 0.0, 0.0, 1.0);

// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
vec2 tilepos = vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。
// （以棋盘格的左下角为原点的坐标系）
bool board[10][20];


// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
vec4 boardcolours[1200];

GLuint locxsize;
GLuint locysize;

GLuint vaoIDs[3];
GLuint vboIDs[6];

//////////////////////////////////////////////////////////////////////////
// 读取最高分数据
void read_profile(const string filename)
{
	if (filename.empty())
		return;
	ifstream fin;
	fin.open(filename); // 打开文件
		fin >> bestscore; // 读取最高分
	fin.close();
}

//////////////////////////////////////////////////////////////////////////
// 写入最高分数据
void write_profile(const string filename)
{
	if (filename.empty())
		return;
	ofstream fout (filename); // 读取文件
	if (fout.is_open())
	{
		fout << bestscore; // 保存最高分
		fout.close();
	}
}

//////////////////////////////////////////////////////////////////////////
// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(vec2 pos, vec4 colour)
{
	// 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
	for (int i = 0; i < 6; i++)
		boardcolours[(int)(6 * (10 * pos.y + pos.x) + i)] = colour;

	vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);

	// 计算偏移量，在适当的位置赋上颜色
	int offset = 6 * sizeof(vec4) * (int)(10 * pos.y + pos.x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////
// 当前方块移动或者旋转时，更新VBO
void updatetile()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);

	// 每个方块包含四个格子
	for (int i = 0; i < 4; i++)
	{
		// 计算格子的坐标值
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;

		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// 每个格子包含两个三角形，所以有6个顶点坐标
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
		glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(vec4), 6 * sizeof(vec4), newpoints);
	}
	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////
// 设置当前方块为下一个即将出现的方块。在游戏开始的时候调用来创建一个初始的方块。
void newtile()
{
	if (!halt) // 游戏不结束时
	{
		// 将新方块放于棋盘格的最上行中间位置并设置默认的旋转方向
		tilepos = vec2(5, 19);
		rotation = 0;
		random = rand() % 7; // 生成随机数控制方块下落
		for (int i = 0; i < 4; i++)
		{
			switch (random)
			{
			case 0:
				tile[i] = allRotationsOshape[0][i];
				break;
			case 1:
				tile[i] = allRotationsIshape[0][i];
				break;
			case 2:
				tile[i] = allRotationsSshape[0][i];
				break;
			case 3:
				tile[i] = allRotationsZshape[0][i];
				break;
			case 4:
				tile[i] = allRotationsLshape[0][i];
				break;
			case 5:
				tile[i] = allRotationsJshape[0][i];
				break;
			case 6:
				tile[i] = allRotationsTshape[0][i];
				break;
			default:
				break;
			}
		}

		updatetile();

		// 给新方块赋上颜色
		vec4 newcolours[24];
		for (int i = 0; i < 24; i++)
			switch (random)
			{
			case 0:
				newcolours[i] = orange;
				break;
			case 1:
				newcolours[i] = red;
				break;
			case 2:
				newcolours[i] = yellow;
				break;
			case 3:
				newcolours[i] = cyan;
				break;
			case 4:
				newcolours[i] = blue;
				break;
			case 5:
				newcolours[i] = green;
				break;
			case 6:
				newcolours[i] = magenta;
				break;
			default:
				break;
			}

		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

//////////////////////////////////////////////////////////////////////////
// 游戏和OpenGL初始化
void init()
{
	// 初始化棋盘格，包含64个顶点坐标（总共32条线），并且每个顶点一个颜色值
	vec4 gridpoints[64];
	vec4 gridcolours[64];

	// 初始化积分
	score = 0;

	// 纵向线
	for (int i = 0; i < 11; i++)
	{
		gridpoints[2*i] = vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);

	}

	// 水平线
	for (int i = 0; i < 21; i++)
	{
		gridpoints[22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}

	// 将所有线赋成灰色
	for (int i = 0; i < 64; i++)
		gridcolours[i] = gray;

	// 初始化棋盘格，并将没有被填充的格子设置成黑色
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black;

	// 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 10; j++)
		{
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);

			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}

	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false;

	// 载入着色器
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	locxsize = glGetUniformLocation(program, "xsize");
	locysize = glGetUniformLocation(program, "ysize");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	glGenVertexArrays(3, &vaoIDs[0]);

	// 棋盘格顶点
	glBindVertexArray(vaoIDs[0]);
	glGenBuffers(2, vboIDs);

	// 棋盘格顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridcolours, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	// 棋盘格每个格子
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// 棋盘格每个格子顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格每个格子顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	// 当前方块
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// 当前方块顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 当前方块顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);

	// 游戏初始化
	PlaySound("./Audio/opening.wav", NULL, SND_SYNC | SND_FILENAME); // 开场音效
	newtile(); // 第一块方块
	printf("SCORE: %d", score); // 打印初始分数
	starttime = glutGet(GLUT_ELAPSED_TIME);
}

//////////////////////////////////////////////////////////////////////////
// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内。(碰撞)
bool checkvalid(vec2 cellpos)
{
	int cx = cellpos.x, cy = cellpos.y;
	// 检查边界范围
	if ((cellpos.x >= 0) && (cellpos.x < 10) && (cellpos.y >= 0) && (cellpos.y < 20))
	{ // 检查是否被填充
		if(board[cx][cy]) return false;
		return true;
	}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
// 在棋盘上有足够空间的情况下旋转当前方块
void rotate()
{
	// 计算得到下一个旋转方向
	int nextrotation = (rotation + 1) % 4;

	// 检查当前旋转之后的位置的有效性
	if (checkvalid((allRotationsLshape[nextrotation][0]) + tilepos)
		&& checkvalid((allRotationsLshape[nextrotation][1]) + tilepos)
		&& checkvalid((allRotationsLshape[nextrotation][2]) + tilepos)
		&& checkvalid((allRotationsLshape[nextrotation][3]) + tilepos))
	{
		// 更新旋转，将当前方块设置为旋转之后的方块
		rotation = nextrotation;
		for (int i = 0; i < 4; i++)
			switch (random) // case 0 只有一种形状 case 1-3 有两种形状
			{
			case 0:
				tile[i] = allRotationsOshape[0][i];
				break;
			case 1:
				tile[i] = allRotationsIshape[rotation % 2][i];
				break;
			case 2:
				tile[i] = allRotationsSshape[rotation % 2][i];
				break;
			case 3:
				tile[i] = allRotationsZshape[rotation % 2][i];
				break;
			case 4:
				tile[i] = allRotationsLshape[rotation][i];
				break;
			case 5:
				tile[i] = allRotationsJshape[rotation][i];
				break;
			case 6:
				tile[i] = allRotationsTshape[rotation][i];
				break;
			default:
				break;
			}

		updatetile();
	}
}

void heart()
{
	halt = true;
	for(int i = 0; i < 10; ++i) for(int j = 0; j < 20; ++j) changecellcolour(vec2(i, j), black);
	// L
	changecellcolour(vec2(0, 19), green);
	changecellcolour(vec2(0, 18), green);
	changecellcolour(vec2(0, 17), green);
	changecellcolour(vec2(0, 16), green);
	changecellcolour(vec2(1, 16), green);
	changecellcolour(vec2(2, 16), green);
	// C
	changecellcolour(vec2(4, 19), green);
	changecellcolour(vec2(4, 18), green);
	changecellcolour(vec2(4, 17), green);
	changecellcolour(vec2(4, 16), green);
	changecellcolour(vec2(5, 16), green);
	changecellcolour(vec2(5, 19), green);
	// Y
	changecellcolour(vec2(9, 19), green);
	changecellcolour(vec2(9, 18), green);
	changecellcolour(vec2(9, 17), green);
	changecellcolour(vec2(9, 16), green);
	changecellcolour(vec2(8, 16), green);
	changecellcolour(vec2(8, 18), green);
	changecellcolour(vec2(7, 16), green);
	changecellcolour(vec2(7, 18), green);
	changecellcolour(vec2(7, 19), green);

	// Heart
	changecellcolour(vec2(2, 14), red);
	changecellcolour(vec2(3, 14), red);
	changecellcolour(vec2(6, 14), red);
	changecellcolour(vec2(7, 14), red);
	changecellcolour(vec2(1, 13), red);
	changecellcolour(vec2(2, 13), red);
	changecellcolour(vec2(3, 13), red);
	changecellcolour(vec2(4, 13), red);
	changecellcolour(vec2(5, 13), red);
	changecellcolour(vec2(6, 13), red);
	changecellcolour(vec2(7, 13), red);
	changecellcolour(vec2(8, 13), red);
	changecellcolour(vec2(2, 12), red);
	changecellcolour(vec2(3, 12), red);
	changecellcolour(vec2(4, 12), red);
	changecellcolour(vec2(5, 12), red);
	changecellcolour(vec2(6, 12), red);
	changecellcolour(vec2(7, 12), red);
	changecellcolour(vec2(3, 11), red);
	changecellcolour(vec2(4, 11), red);
	changecellcolour(vec2(5, 11), red);
	changecellcolour(vec2(6, 11), red);
	changecellcolour(vec2(4, 10), red);
	changecellcolour(vec2(5, 10), red);

	// P
	changecellcolour(vec2(0, 8), blue);
	changecellcolour(vec2(0, 7), blue);
	changecellcolour(vec2(0, 6), blue);
	changecellcolour(vec2(0, 5), blue);
	changecellcolour(vec2(1, 8), blue);
	changecellcolour(vec2(1, 6), blue);
	changecellcolour(vec2(2, 8), blue);
	changecellcolour(vec2(2, 7), blue);
	changecellcolour(vec2(2, 6), blue);
	// J
	changecellcolour(vec2(5, 8), blue);
	changecellcolour(vec2(5, 6), blue);
	changecellcolour(vec2(5, 5), blue);
	changecellcolour(vec2(4, 5), blue);
	// Q
	changecellcolour(vec2(9, 8), blue);
	changecellcolour(vec2(9, 7), blue);
	changecellcolour(vec2(9, 6), blue);
	changecellcolour(vec2(9, 5), blue);
	changecellcolour(vec2(8, 8), blue);
	changecellcolour(vec2(8, 6), blue);
	changecellcolour(vec2(7, 8), blue);
	changecellcolour(vec2(7, 7), blue);
	changecellcolour(vec2(7, 6), blue);
}

//////////////////////////////////////////////////////////////////////////
// 清除row行，积分设置，难度递增，音效
void clearrow(int row)
{
	for (int k = row; k < 19; k++)
		for (int i = 0; i < 10; i++)
		{
			// 方块下落
			board[i][k] = board[i][k + 1];
			// 改变颜色
			changecellcolour(vec2(i, k), boardcolours[(int)(6 * (10 * (k + 1) + i))]);
		}
	// 分数增加
	score++;
	// 显示分数
	printf("\r");
	printf("SCORE: %d", score);
	// 高分判断
	bestscore = (bestscore < score) ? score : bestscore;
	// 写入数据到 data.tetris
	write_profile("data.tetris");
	// I love you :)
	if (score == 15) heart();
	// 难度递增
	FALL_TIME = 1000 - sqrt(score) * 160; // FALL_TIME是一个幂函数
	// 播放音乐
	PlaySound("./Audio/clear.wav", NULL, SND_SYNC | SND_FILENAME);
}



//////////////////////////////////////////////////////////////////////////
// 检查棋盘格在row行有没有被填充满（并进行清除）
void checkfullrow(int row)
{
	int count = 0;
	for (int i = 0; i < 10; i++) // 检查
		if (!checkvalid(vec2(i, row)))
			count++;	
	if (count == 10) // 清除
	{
		clearrow(row);
		checkfullrow(row); // 重新检查当前行
	}
}

//////////////////////////////////////////////////////////////////////////
// 检查棋盘格在line列有没有被填充满（并进行游戏结束判定）
void checkfullline(int line)
{
	bool full = false;
		if (!checkvalid(vec2(line, 19)))
			if (!checkvalid(vec2(line, 18)))
				if (!checkvalid(vec2(line, 17)))
					full = true;
	if (full)
	{
		cout << endl << "GAME OVER! YOUR SCORE IS: " << score << ", BEST SCORE IS: " << bestscore << endl; 
		PlaySound("./Audio/gameover.wav", NULL, SND_SYNC | SND_FILENAME);
		halt = true;
	}
}

//////////////////////////////////////////////////////////////////////////
// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile()
{
	// 每个格子
	for (int i = 0; i < 4; i++)
	{
		// 获取格子在棋盘格上的坐标
		int x = (tile[i] + tilepos).x;
		int y = (tile[i] + tilepos).y;
		// 将格子对应在棋盘格上的位置设置为填充
		board[x][y] = true;
		// 并将相应位置的颜色修改
		switch (random)
		{
		case 0:
			changecellcolour(vec2(x, y), orange);
			break;
		case 1:
			changecellcolour(vec2(x, y), red);
			break;
		case 2:
			changecellcolour(vec2(x, y), yellow);
			break;
		case 3:
			changecellcolour(vec2(x, y), cyan);
			break;
		case 4:
			changecellcolour(vec2(x, y), blue);
			break;
		case 5:
			changecellcolour(vec2(x, y), green);
			break;
		case 6:
			changecellcolour(vec2(x, y), magenta);
			break;
		default:
			break;
		}
	}
	for (int i = 0; i < 20; i++)
		checkfullrow(i); // 放置后检查是否满行
	for (int i = 0; i < 10; i++)
		if(!halt) // 若游戏不结束
			checkfullline(i); // 检查游戏是否结束
}

//////////////////////////////////////////////////////////////////////////
// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false。、
bool movetile(vec2 direction)
{
	// 计算移动之后的方块的位置坐标
	vec2 newtilepos[4];
	for (int i = 0; i < 4; i++)
		newtilepos[i] = tile[i] + tilepos + direction;

	// 检查移动之后的有效性
	if (checkvalid(newtilepos[0])
		&& checkvalid(newtilepos[1])
		&& checkvalid(newtilepos[2])
		&& checkvalid(newtilepos[3]))
		{
			// 有效：移动该方块
			tilepos.x = tilepos.x + direction.x;
			tilepos.y = tilepos.y + direction.y;

			updatetile();

			return true;
		}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// 重新启动游戏
void restart()
{
	halt = false;
	init();
}

//////////////////////////////////////////////////////////////////////////
// 游戏渲染部分
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]);
	glDrawArrays(GL_TRIANGLES, 0, 1200); // 绘制棋盘格 (10*20*2 = 400 个三角形)

	glBindVertexArray(vaoIDs[2]);
	glDrawArrays(GL_TRIANGLES, 0, 24);	 // 绘制当前方块 (8 个三角形)

	glBindVertexArray(vaoIDs[0]);
	glDrawArrays(GL_LINES, 0, 64);		 // 绘制棋盘格的线

	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// 键盘响应事件中的特殊按键响应
void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:	// 向上按键旋转方块
			rotate();
			break;
		case GLUT_KEY_DOWN: // 向下按键移动方块
			if (!movetile(vec2(0, -1)))
			{
				settile();
				newtile();
			}
			break;
		case GLUT_KEY_LEFT:  // 向左按键移动方块
			movetile(vec2(-1, 0));
			break;
		case GLUT_KEY_RIGHT: // 向右按键移动方块
			movetile(vec2(1, 0));
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 键盘响应时间中的普通按键响应
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 033: // ESC键 和 'q' 键退出游戏
			exit(EXIT_SUCCESS);
			break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': // 'r' 键重启游戏
			restart();
			break;
	}
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////
// 自动下落
void autofall(int x)
{
	if (!movetile(vec2(0, -1)))
	{
		settile();
		newtile();
	}
	glutPostRedisplay();
	glutTimerFunc(FALL_TIME, autofall, 1); // 调用自身实现自动下落
}

//////////////////////////////////////////////////////////////////////////
void idle(void)
{
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	srand(time(NULL));
	read_profile("data.tetris");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(520, 92);
	glutCreateWindow("Tetris");
	glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(FALL_TIME, autofall, 1); // 启动自动下落
	glutIdleFunc(idle);
	
	glutMainLoop();
	return 0;
}
