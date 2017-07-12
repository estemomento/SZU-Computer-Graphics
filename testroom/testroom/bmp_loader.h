#include <cstdlib>
struct Image
{
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
void LoadGLTextures(char *filename, GLuint *texture_num);
int ImageLoad(char *filename, Image *image);