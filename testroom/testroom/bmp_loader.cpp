#define _CRT_SECURE_NO_WARNINGS
#include "Angel.h"
#include "bmp_loader.h"
using namespace std;

static unsigned int getint(FILE *fp){ // 读取4bit整数
  int c, c1, c2, c3;
  c = getc(fp);  
  c1 = getc(fp); 
  c2 = getc(fp);  
  c3 = getc(fp);
  return ((unsigned int) c) +  (((unsigned int) c1) << 8) + (((unsigned int) c2) << 16) + (((unsigned int) c3) << 24);
}

static unsigned int getshort(FILE *fp) // 读取2bit整数
{
  int c, c1;
  c = getc(fp);  
  c1 = getc(fp);
  return ((unsigned int) c) + (((unsigned int) c1) << 8);
} 

// 加载bmp图片
int ImageLoad(char *filename, Image *image) {
  FILE *file;
  int size; // 图片大小
  unsigned int planes;
  unsigned int bpp; // 每像素24bits
  char temp;
  
  // 加载文件
  if ((file = fopen(filename, "rb")) == NULL) {
    cout << "File Not Found : " << filename << endl;
    return 0;
  }
  
  // 读入图片像素等资料
  fseek(file, 18, SEEK_CUR);
  image->sizeX = getint (file);
  image->sizeY = getint (file);
  size = image->sizeX * image->sizeY * 3;
  planes = getshort(file);
  bpp = getshort(file);

  // 读取图片数据
  fseek(file, 24, SEEK_CUR);
  image->data = (char *) malloc(size);
  if (image->data == NULL) {
    cout << "Error allocating memory for color-corrected image data" << endl;
    return 0; 
  }
  if (fread(image->data, size, 1, file) != 1) {
    cout << "Error reading image data from " << filename << endl;
    return 0;
  }
  for (int i = 0; i < size; i += 3) { // 将原有的bgr颜色转为rgb格式
    temp = image->data[i];
    image->data[i] = image->data[i+2];
    image->data[i+2] = temp;
  }
  return 1;
}

// 读取图片并绑定到OpenGL
void LoadGLTextures(char *filename,GLuint *texture_num) { 
  Image image;
  if (!ImageLoad(filename, &image)) {
    exit(1);
  }          

  // 创建纹理
  glGenTextures(3, texture_num);
  glBindTexture(GL_TEXTURE_2D, *texture_num);
  
  // 纹理放缩
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
  
  // 绑定纹理  
  glTexImage2D(GL_TEXTURE_2D, 0, 3, image.sizeX, image.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);

}


