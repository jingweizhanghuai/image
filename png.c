#include <libpng/png.h>
#include <stdio.h>
#include "type.h"
#include "err.h"

int imgReadPNG(const char *filename,ImgMat *dst)
{
	FILE *f;
	f = fopen(filename, "rb");
	if(f == NULL)
		return 1;

	png_structp png_ptr;
	png_infop info_ptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	info_ptr = png_create_info_struct(png_ptr);

	char buf[4];
	fread(buf,1,4,f); 
	if(png_sig_cmp((void*)buf, (png_size_t)0,4)!=0)
		return 1;
 
	fseek(f,0,SEEK_SET);
	png_init_io(png_ptr,f);
	png_read_png(png_ptr,info_ptr,PNG_TRANSFORM_EXPAND,0);

	// int color_type;
	// color_type = png_get_color_type(png_ptr, info_ptr);
	
	int img_width;
	img_width = png_get_image_width(png_ptr, info_ptr); 	
	
	int img_height;
	img_height = png_get_image_height(png_ptr, info_ptr);
	
	int cn;
	cn = png_get_channels(png_ptr,info_ptr);
	
	// int depth;
	// bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	
	int dst_type;
	dst_type = ((cn-1)<<3);
	
	if((dst->width != img_width)||(dst->height != img_height)||(dst->type != dst_type))
		imgMatRedefine(dst,img_height,img_width,dst_type);

	int i,j;
	
	int img_size;
	img_size = dst->size;
	
	unsigned char *p0_dst;
	p0_dst = dst->data.ptr;
	unsigned char *p1_dst;
	p1_dst = p0_dst+img_size;
	unsigned char *p2_dst;
	p2_dst = p1_dst+img_size;
	unsigned char *p3_dst;
	p3_dst = p2_dst+img_size;
	
	png_bytep* p_data;
	p_data = png_get_rows(png_ptr,info_ptr);

	if(cn==1)
		copy_mat_data(p_data,p0_dst,img_size);
	else if(cn == 3)
	{
		for(j=0;j<img_height;j++)
			for(i=0;i<img_width*3;i=i+3)
			{
				*p0_dst = p_data[j][i];
				*p1_dst = p_data[j][i+1];
				*p2_dst = p_data[j][i+2];
				
				p0_dst = p0_dst+1;
				p1_dst = p1_dst+1;
				p2_dst = p2_dst+1;
			}
	}
	else if(cn == 4)
	{
		for(j=0;j<img_height;j++)
			for(i=0;i<img_width*4;i=i+4)
			{
				*p2_dst = p_data[j][i];
				*p1_dst = p_data[j][i+1];
				*p0_dst = p_data[j][i+2];
				*p3_dst = p_data[j][i+3];
				
				p0_dst = p0_dst+1;
				p1_dst = p1_dst+1;
				p2_dst = p2_dst+1;
				p3_dst = p3_dst+1;
			}
	}
	free(p_data);
	png_destroy_read_struct(&png_ptr,&info_ptr,0); 	
	fclose(f);
	return 0;
}

int imgSavePNG(ImgMat *src,char *filename)
{
	FILE *f = fopen(filename,"wb");
	if (!f)
	{
		printf("Img Error:\n\tin imgSavePNG: save image error\n");
		return 1;
	}

	png_structp png_ptr;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (!png_ptr)
	{
		printf("Img Error:\n\tin imgSavePNG: save image error\n");
		return 1;
	}
	
	png_infop info_ptr; 
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("Img Error:\n\tin imgSavePNG: save image error\n");
		return 1;
	}

	png_init_io(png_ptr,f);
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int color_type;
	if(cn == 4)
		color_type = PNG_COLOR_TYPE_RGB_ALPHA;
	else if(cn==3)
		color_type = PNG_COLOR_TYPE_RGB;
	else if(cn==1)
		color_type = PNG_COLOR_TYPE_GRAY;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;

	png_set_IHDR(png_ptr,info_ptr,img_width,img_height,8,color_type,PNG_INTERLACE_NONE,  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr,info_ptr);
	
	png_bytep *p_data;
	p_data = (png_bytep *)malloc(img_height*4);
	
	unsigned char *p0_src;
	p0_src = src->data.ptr;
	unsigned char *p1_src;
	p1_src = p0_src+src->size;
	unsigned char *p2_src;
	p2_src = p1_src+src->size;
	unsigned char *p3_src;
	p3_src = p2_src+src->size;
	
	int i,j;
	
	if(cn==1)
	{
		for(j=0;j<img_height;j++)
		{
			p_data[j] = p0_src;
			p0_src = p0_src+img_width;
		}
	}
	else if(cn==3)
	{
		for(j=0;j<img_height;j++)
		{
			p_data[j] = malloc(img_width*3);
			for(i=0;i<img_width*3;i=i+3)
			{
				p_data[j][i] = *p2_src;
				p_data[j][i+1] = *p1_src;
				p_data[j][i+2] = *p0_src;
				
				p0_src = p0_src+1;
				p1_src = p1_src+1;
				p2_src = p2_src+1;
			}
		}
	}
	else if(cn==4)
	{
		for(j=0;j<img_height;j++)
		{
			p_data[j] = malloc(img_width*4);
			for(i=0;i<img_width*4;i=i+4)
			{
				p_data[j][i] = *p2_src;
				p_data[j][i+1] = *p1_src;
				p_data[j][i+2] = *p0_src;
				p_data[j][i+3] = *p3_src;
				
				p0_src = p0_src+1;
				p1_src = p1_src+1;
				p2_src = p2_src+1;
				p3_src = p3_src+1;				
			}
		}
	}
	else
	{
		printf("Img Error:\n\tin imgSavePNG: save image error\n");
		return 1;
	}
	
	png_write_image(png_ptr,p_data);
	
	png_write_end(png_ptr,info_ptr);
	
	if(cn!=1)
	{
		for(j=0;j<img_height;j++)
			free(p_data[j]);
	}
	free(p_data);
	
	png_destroy_write_struct(&png_ptr,&info_ptr); 
    fclose(f);
    return 0;
}


