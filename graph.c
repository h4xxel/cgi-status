#include <stdio.h>
#include <png.h>

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

#define W 32
#define H 32
#define DEPTH 8

static png_byte imagedata[H*W*4];

int write_png(png_FILE_p f) {
	png_structp png;
	png_infop info;
	png_uint_32 i;
	png_bytep rows[H];
	
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
	
	for(i=0; i<H; i++)
		rows[i]=imagedata+i*W*4;
	png_write_image(png, rows);
	png_write_end(png, info);
	return 0;
}

int main(int argc, char **argv) {
	unsigned int i;
	for(i=0; i<W*H; i++)
		*((unsigned int *) &imagedata[i*4])=i;
	write_png(stdout);
	return 0;
}