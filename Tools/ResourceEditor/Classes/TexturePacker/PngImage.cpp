/*
 *  PngImage.cpp
 *  texturepack
 *
 *  Created by Vitaliy Borodovsky on 10/28/08.
 *  Copyright 2008 DAVA Consulting, LLC. All rights reserved.
 *
 */

#include "PngImage.h"
#include "CommandLineParser.h"

#ifndef WIN32
	#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>

#include <png.h>

#define PNG_DEBUG 3

static void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

//int x, y;

static void convert_rawpp_to_bytestream(int32 width, int32 height, png_bytepp row_pointers, uint8 * data)
{
	for (int y = 0; y < height; y++) 
	{
		png_byte* row = row_pointers[y];
		memcpy(data, row, width * 4);
		data += width * 4;
	}
}

static void convert_bytestream_to_rawpp(int32 width, int32 height, uint8 * data, png_bytepp row_pointers)
{
	for (int y = 0; y < height; y++)
	{
		png_byte* row = row_pointers[y];
		memcpy(row, data, width * 4);
		data += width * 4;
	}
}

int read_png_file(const char *file, int32 *pwidth, int32 *pheight, uint8 **image_data_ptr)
{
	FILE         *infile;         /* PNG file pointer */
	png_structp   png_ptr;        /* internally used by libpng */
	png_infop     info_ptr;       /* user requested transforms */
	
	uint8 *image_data;      /* raw png image data */
	char         sig[8];           /* PNG signature array */
	/*char         **row_pointers;   */
	
	int           bit_depth;
	int           color_type;
	
	png_uint_32 width;            /* PNG image width in pixels */
	png_uint_32 height;           /* PNG image height in pixels */
	unsigned int rowbytes;         /* raw bytes at row n in image */
	
	image_data = NULL;
	int i;
	png_bytepp row_pointers = NULL;
	
	/* Open the file. */
	infile = fopen(file, "rb");
	if (!infile) {
		
		return 0;
	}
	
	
	/*
	 * 		13.3 readpng_init()
	 */
	
	/* Check for the 8-byte signature */
	fread(sig, 1, 8, infile);
	
	if (png_sig_cmp((unsigned char *) sig, 0, 8) != 0) {
		fclose(infile);
		return 0;
	}
	
	/* 
	 * Set up the PNG structs 
	 */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(infile);
		return 4;    /* out of memory */
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(infile);
		return 4;    /* out of memory */
	}
	
	
	/*
	 * block to handle libpng errors, 
	 * then check whether the PNG file had a bKGD chunk
	 */
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(infile);
		return 0;
	}
	
	/* 
	 * takes our file stream pointer (infile) and 
	 * stores it in the png_ptr struct for later use.
	 */
	/* png_ptr->io_ptr = (png_voidp)infile;*/
	png_init_io(png_ptr, infile);
	
	/*
	 * lets libpng know that we already checked the 8 
	 * signature bytes, so it should not expect to find 
	 * them at the current file pointer location
	 */
	png_set_sig_bytes(png_ptr, 8);
	
	/* Read the image info.*/
	
	/*
	 * reads and processes not only the PNG file's IHDR chunk 
	 * but also any other chunks up to the first IDAT 
	 * (i.e., everything before the image data).
	 */
	
	/* read all the info up to the image data  */
	png_read_info(png_ptr, info_ptr);
	
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, 
				 &color_type, NULL, NULL, NULL);
	
	*pwidth = width;
	*pheight = height;

	if (bit_depth > 8) {
		png_set_strip_16(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
		png_set_filler(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
	}
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
		png_set_filler(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
		if (CommandLineParser::Instance()->GetVerbose())
			printf("*** TOCHECK: convert tRNS to alpha\n");
	}

	if (color_type == PNG_COLOR_TYPE_RGB)
	{	
		if (CommandLineParser::Instance()->GetVerbose())
			printf("*** Converting %s from RGB to RGBA\n", file);
		png_set_filler(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
	}
		
	/* Update the png info struct.*/
	png_read_update_info(png_ptr, info_ptr);
	
	/* Rowsize in bytes. */
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	
	char string_format[16];
	strcpy(string_format, "undefined");
	if (color_type == PNG_COLOR_TYPE_RGBA)
	{
		strcpy(string_format, "RGBA");
	}
	
	
	if (CommandLineParser::Instance()->GetVerbose())
	{
		printf("* Reading PNG file: %s format: %s bit_depth:%d bytes_per_pixel:%d\n", 
		   file,
		   string_format,
		   bit_depth,
		   rowbytes / width);
	}	
	
	/* Allocate the image_data buffer. */
	// printf("r/w: %d %d\n" , rowbytes / width, bit_depth); 
	
	
	if ((image_data = new uint8 [rowbytes * height]) ==NULL) 
	{
		memset(image_data, 0, rowbytes * height);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 4;
    }
	
	if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) 
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        delete [] (image_data);
        image_data = NULL;
        return 4;
    }
	
	
    /* set the individual row_pointers to point at the correct offsets */
	
    for (i = 0;  i < (int)height;  ++i)
        row_pointers[i] = image_data + i*rowbytes;
	
	
    /* now we can go ahead and just read the whole image */
    png_read_image(png_ptr, row_pointers);
	
    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */
	
	/* Clean up. */
	free(row_pointers);
	
	/* Clean up. */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(infile);
	
	*image_data_ptr = image_data;
	
	return 1;
}



void write_png_file(const char* file_name, int32 width, int32 height, uint8 * data)
{
	if (CommandLineParser::Instance()->GetVerbose())
		printf("* Writing PNG file (%d x %d): %s\n", width, height, file_name);
	
	png_byte color_type = PNG_COLOR_TYPE_RGBA;
	png_byte bit_depth = 8;
	png_color_8 sig_bit;
	
	png_structp png_ptr;
	png_infop info_ptr;
	
	png_bytep * row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++)
	{
		row_pointers[y] = (png_byte*) &data[y * width * 4];
	}
	
	
	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		abort_("[write_png_file] File %s could not be opened for writing", file_name);
	
	
	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		abort_("[write_png_file] png_create_write_struct failed");
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[write_png_file] png_create_info_struct failed");
	
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during init_io");
	
	png_init_io(png_ptr, fp);
	
	
	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing header");
	
	png_set_IHDR(png_ptr, info_ptr, width, height,
				 bit_depth, color_type, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	
	
	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	sig_bit.alpha = 8;

	png_set_sBIT(png_ptr, info_ptr, &sig_bit);
		
	png_write_info(png_ptr, info_ptr);
	png_set_shift(png_ptr, &sig_bit);
	png_set_packing(png_ptr);
	
	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing bytes");
	
	png_write_image(png_ptr, row_pointers);
	
	
	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during end of write");
	
	png_write_end(png_ptr, NULL);
	
	free(row_pointers);
	
	fclose(fp);
}

PngImageExt::PngImageExt()
:	width(0)
,	height(0)
,	data(0)
{
		
}

PngImageExt::~PngImageExt()
{
	SafeDeleteArray(data);
	width = 0;
	height = 0;
}

bool PngImageExt::Read(const char * filename)
{
	if (read_png_file(filename, &width, &height, &data) == 0)
	{
		if (CommandLineParser::Instance()->GetVerbose())
		{
			printf("FAILED TO OPEN PNG FILE: %s\n", filename);
		}
	}
	return true;
}

bool PngImageExt::Write(const char * filename)
{
	write_png_file(filename, width, height, data);
	return true;
}

bool PngImageExt::Create(int _width, int _height)
{
	width = _width;
	height = _height;
	data = new uint8[width * 4 * height];
	memset(data, 0, width * 4 * height);
	if (!data)return false;
	return true; 
}

void PngImageExt::DrawImage(int sx, int sy, PngImageExt * image, const Rect2i & srcRect)
{
	uint32 * destData32 = (uint32*)data;
	uint32 * srcData32 = (uint32*)image->data;
	
	int rx, ry;
	ry = sy;
	for (int y = srcRect.y; y < srcRect.y + srcRect.dy; ++y)
	{
		rx = sx;
		for (int x = srcRect.x; x < srcRect.x + srcRect.dx; ++x)
		{
			if ((rx) < 0)continue;
			if ((rx) >= width)continue;
			if ((ry) < 0)continue;
			if ((ry) >= height)continue;
			if (x < 0)continue;
			if (x >= image->width)continue;
			if (y < 0)continue;
			if (y >= image->height)continue;
			
			destData32[(rx) + (ry) * width] = srcData32[x + y * image->width];
			//printf("%04x ", srcData32[x + y * image->width]);
			rx++;
		}
		ry++;
	}
}

void PngImageExt::DrawImage(int sx, int sy, PngImageExt * image)
{
	// printf("0x%08x 0x%08x %d %d\n", data, image->data, sx, sy);
	
	uint32 * destData32 = (uint32*)data;
	uint32 * srcData32 = (uint32*)image->data;
	
	for (int y = 0; y < image->height; ++y)
		for (int x = 0; x < image->width; ++x)
		{
			if ((sx + x) < 0)continue;
			if ((sx + x) >= width)continue;
			if ((sy + y) < 0)continue;
			if ((sy + y) >= height)continue;
			
			destData32[(sx + x) + (sy + y) * width] = srcData32[x + y * image->width];
			//printf("%04x ", srcData32[x + y * image->width]);
		}
}

bool PngImageExt::IsHorzLineOpaque(int y)
{
	uint8 * line = data + y * width * 4;
	for (int x = 0; x < width; ++x)
		if (line[x * 4 + 3] != 0)
			return false;
	return true;
}

bool PngImageExt::IsVertLineOpaque(int x)
{
	uint8 * vertLine = data + x * 4;
	for (int x = 0; x < height; ++x)
	{
		if (vertLine[3] != 0)
			return false;
		
		vertLine += width * 4;
	}
	return true;
}

void PngImageExt::FindNonOpaqueRect(Rect2i & rect)
{
	rect = Rect2i(0, 0, width, height);
	for (int y = 0; y < height; ++y)
		if (IsHorzLineOpaque(y))
		{
			rect.y++;
			rect.dy--;
		}else break;
	
	for (int x = 0; x < width; ++x)
		if (IsVertLineOpaque(x))
		{
			rect.x++;
			rect.dx--;
		}else break;
		
	if ((rect.dx == 0) && (rect.dy == 0))
	{
		rect.x = rect.y = 0; 
		rect.dx = rect.dy = 1;
		return;
	}
	
	for (int y = height - 1; y >= 0; --y)
		if (IsHorzLineOpaque(y))rect.dy--;
		else break;
	
	for (int x = width - 1; x >= 0; --x)
		if (IsVertLineOpaque(x))rect.dx--;
		else break;
}

void PngImageExt::DrawRect(const Rect2i & rect, uint32 color)
{
	uint32 * destData32 = (uint32*)data;
	
	for (int i = 0; i < rect.dx; ++i)
	{
		destData32[rect.y * width + rect.x + i] = color; 
		destData32[(rect.y + rect.dy - 1) * width + rect.x + i] = color; 
	}
	for (int i = 0; i < rect.dy; ++i)
	{
		destData32[(rect.y + i) * width + rect.x] = color; 
		destData32[(rect.y + i) * width + rect.x + rect.dx - 1] = color; 
	}
}

/*void process_file(void)
{
	if (info_ptr->color_type != PNG_COLOR_TYPE_RGBA)
		abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (is %d)", info_ptr->color_type);
	
	for (y=0; y<height; y++) {
		png_byte* row = row_pointers[y];
		for (x=0; x<width; x++) {
			png_byte* ptr = &(row[x*4]);
			printf("Pixel at position [ %d - %d ] has the following RGBA values: %d - %d - %d - %d\n",
			       x, y, ptr[0], ptr[1], ptr[2], ptr[3]);
			
			
			ptr[0] = 0;
			ptr[1] = ptr[2];
		}
	}
	
}*/
