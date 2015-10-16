#include <stdio.h>
#include <GL/glut.h>
#include <string.h>
#include "global.h"

// Global variables
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3]; 

/*********************************************************
 * This function saves the current image to a ppm file
 *		
 * DO NOT CHANGE
 *********************************************************/
void save_image() {
	int w = win_width;
	int h = win_height;

	unsigned char * bImg = new unsigned char[w * h * 3];

	int index = 0;
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++) {

		float r = frame[y][x][0]; 
		float g = frame[y][x][1]; 
		float b = frame[y][x][2];

		bImg[index] = (b > 1.f) ? 255 : (unsigned char)(b*255); index++;
		bImg[index] = (g > 1.f) ? 255 : (unsigned char)(g*255); index++;
		bImg[index] = (r > 1.f) ? 255 : (unsigned char)(r*255); index++;
		}
	}

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	int filesize = 54 + 3*w*h;

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);


	FILE *fp;
	char fname[32];

	strcpy(fname, "scene.bmp");
	printf("Saving image %s: %d x %d\n", fname, w, h);
	fp = fopen(fname, "wb");
	if (!fp) {
	printf("Unable to open file '%s'\n",fname);
	return;
	}


	fwrite(bmpfileheader,1,14,fp);
	fwrite(bmpinfoheader,1,40,fp);

	for(int y = h-1; y >= 0; y--)
	{
		int offset = w * (h - 1 - y) * 3;
		fwrite(bImg + offset, 3, w, fp);
		fwrite(bmppad,1,(4-(w*3)%4)%4,fp);
	}

 
	fclose(fp);
}

/**************************************************************
 * This function normalizes the frame resulting from ray
 * tracing so that the maximum R, G, or B value is 1.0
 *
 * DO NOT CHANGE
 **************************************************************/
void histogram_normalization() {
  GLfloat max_val = 0.0;
  int i, j;

  for (i=0; i<win_height; i++) 
    for (j=0; j<win_width; j++) {
      if (frame[i][j][0] > max_val) max_val = frame[i][j][0];
      if (frame[i][j][1] > max_val) max_val = frame[i][j][1];
      if (frame[i][j][2] > max_val) max_val = frame[i][j][2];
    }

  for (i=0; i<win_height; i++) 
    for (j=0; j<win_width; j++) {
      frame[i][j][0] /= max_val;
      frame[i][j][1] /= max_val;
      frame[i][j][2] /= max_val;
    }
}
