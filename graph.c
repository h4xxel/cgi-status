#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <png.h>

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

#define W 256
#define H 128
#define DEPTH 8

static unsigned int imagedata[H][W];

int write_png(png_FILE_p f) {
	png_structp png;
	png_infop info;
	png_uint_32 y;
	
	if(!(png=png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
		return 1;
	if(!(info=png_create_info_struct(png))) {
		png_destroy_write_struct(&png, NULL);
		return 1;
	}
	if(setjmp(png_jmpbuf(png))) {
		png_destroy_write_struct(&png, &info);
		return 1;
	}
	
	png_init_io(png, f);
	png_set_IHDR(png, info, W, H, DEPTH, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png, info);
	png_set_filler(png, 0, PNG_FILLER_AFTER);
	
	for(y=0; y<H; y++)
		png_write_row(png, (png_bytep) imagedata[y]);
	
	png_write_end(png, info);
	return 0;
}

int main(int argc, char **argv) {
	srand(time(NULL));
	printf("Content-type: image/png\nStatus: 200 OK\n\n");
	
	int x, y=H/2;
	for(x=0; x<W; x++) {
		y+=rand()%3-1;
		y=y<0?0:y>=H?H-1:y;
		imagedata[y][x]=0x00FF00;
	}
	write_png(stdout);
	return 0;
}
