#include <stdio.h> 
#include <libjpeg/jpeglib.h> 
#include "type.h"
#include "err.h"

#define JPEG_QUALITY 100 //图片质量 

int imgSaveJPG_24(ImgMat *src,char *filename)
{	
	#ifdef DEBUG
	if((src->type&0x07) != TYPE_8U)
	{
		printf("Img Error:\n\tin imgSaveJPG: Wrong src format\n");
		return 1;
	}
	int cn;
	cn = ((src->type)>>3)+1;
	if(cn <3)
	{
		printf("Img Error:\n\tin imgSaveJPG: Wrong src format\n");
		return 1;
	}
	if((src->memory_valid[0] == 0)&&(src->memory_valid[1] == 0))
	{
		printf("IMG Error:\n\tin imgSaveJPG: The src data is invlid\n");
		return 1;
	}
#endif
	
	if(src->memory_valid[0] == 0)
		imgReadMatOCLMemory(src);

	FILE *f;
	f = fopen(filename,"wb");

	#ifdef DEBUG	
	if(f == NULL)
	{
		printf("Img Error:\n\tin imgSaveJPG: save image error\n");
		return 1;
	}
	#endif

	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;	
	cinfo.err = jpeg_std_error(&jerr);
	
	jpeg_create_compress(&cinfo);	
	jpeg_stdio_dest(&cinfo,f);
	
	cinfo.image_width = img_width;     
	cinfo.image_height = img_height;   
	cinfo.input_components = 3;  
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo,JPEG_QUALITY,1);
	
	jpeg_start_compress(&cinfo,TRUE);
	
	JSAMPROW data_buff;
	data_buff = malloc(img_width*12);
	
	JSAMPROW p_buff;
	
	unsigned char *p0_src;
	p0_src = src->data.ptr;
	unsigned char *p1_src;
	p1_src = p0_src+src->size;
	unsigned char *p2_src;
	p2_src = p1_src+src->size;
	
	int i,j;
	for(j=0;j<img_height;j++)
	{
		p_buff = data_buff;
		for(i=0;i<img_width;i++)
		{
			p_buff[0] = p2_src[i];
			p_buff[1] = p1_src[i];
			p_buff[2] = p0_src[i];
			
			p_buff = p_buff+3;
		}
		
		jpeg_write_scanlines(&cinfo,&data_buff,1); 		
			
		p0_src = p0_src+img_width;
		p1_src = p1_src+img_width;
		p2_src = p2_src+img_width;
	}
	
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	free(data_buff);
	fclose(f);
	return 0;
}


int imgSaveJPG_8(ImgMat *src,char *filename)
{	
	#ifdef DEBUG
	if((src->type&0x07) != TYPE_8U)
	{
		printf("Img Error:\n\tin imgSaveJPG: Wrong src format\n");
		return 1;
	}
	if((src->memory_valid[0] == 0)&&(src->memory_valid[1] == 0))
	{
		printf("IMG Error:\n\tin imgSaveJPG: The src data is invlid\n");
		return 1;
	}
#endif
	
	if(src->memory_valid[0] == 0)
		imgReadMatOCLMemory(src);
	
	int cn;
	cn = ((src->type)>>3)+1;
	if(cn >= 3)
		ColorToGray(src,NULL);
	#ifdef DEBUG
	cn = ((src->type)>>3)+1;
	if(cn !=1)
	{
		printf("Img Error:\n\tin imgSaveJPG: Wrong src format\n");
		return 1;
	}	
	#endif
	
	FILE *f;
	f = fopen(filename,"wb");

	#ifdef DEBUG	
	if(f == NULL)
	{
		printf("Img Error:\n\tin imgSaveJPG: save image error\n");
		return 1;
	}
	#endif

	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	struct jpeg_compress_struct cinfo; 
	struct jpeg_error_mgr jerr;	
	cinfo.err = jpeg_std_error(&jerr);
	
	jpeg_create_compress(&cinfo);	
	jpeg_stdio_dest(&cinfo,f);
	
	cinfo.image_width = img_width;   //* image width and height, in pixels   
	cinfo.image_height = img_height;   
	cinfo.input_components = 1;   //* # of color components per pixel   
	cinfo.in_color_space = JCS_GRAYSCALE;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo,JPEG_QUALITY,1);
	
	jpeg_start_compress(&cinfo,TRUE);
	
	JSAMPROW data_buff;
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	int i,j;
	for(j=0;j<img_height;j++)
	{
		data_buff = p_src;		
		jpeg_write_scanlines(&cinfo,&data_buff,1); 				
		p_src = p_src+img_width;
	}
	
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(f);
	return 0;
}

int imgSaveJPG(ImgMat *src,char *filename)
{
	int cn;
	cn = ((src->type)>>3)+1;
	
	if(cn==1)
		return imgSaveJPG_8(src,filename);
	else if(cn >= 3)
		return imgSaveJPG_24(src,filename);
	else
	{
		printf("Img Error:\n\tin imgSaveJPG:read image error\n");
		return 1;
	}
}

int imgReadJPG(const char *filename,ImgMat *dst)
{
	FILE *f;
	f = fopen(filename,"rb");
	if(f == NULL)
	{
		printf("Img Error:\n\tin imgReadJPG:read image error\n");
		return 1;
	}
	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo,f);
	jpeg_read_header(&cinfo,TRUE);		 
	jpeg_start_decompress(&cinfo);
	
	int img_width;
	img_width = cinfo.output_width;
	
	int img_height;
	img_height = cinfo.output_height;
	
	int img_size;
	img_size = img_width*img_height;
	
	int cn;
	cn = cinfo.output_components;
	
	int dst_type;
	dst_type = ((cn-1)<<3);
	
	if((dst->width != img_width)||(dst->height != img_height)||(dst->type != dst_type))
		imgMatRedefine(dst,img_height,img_width,dst_type);
	
	JSAMPROW data_buff;
	data_buff = malloc(img_width*cn);
	
	JSAMPROW p_buff;
	
	unsigned char *p0_dst;
	p0_dst = dst->data.ptr;
	unsigned char *p1_dst;
	p1_dst = p0_dst+img_size;
	unsigned char *p2_dst;
	p2_dst = p1_dst+img_size;
	
	int i,j;
	
	if(cn==1)
	{
		p_buff = p0_dst;
		for(j=0;j<img_height;j++)
		{			
			jpeg_read_scanlines(&cinfo,&p_buff,1);
			p_buff = p_buff + img_width;
		}
	}
	else if(cn == 3)
	{
		for(j=0;j<img_height;j++)
		{
			p_buff = data_buff;
			jpeg_read_scanlines(&cinfo,&p_buff,1);
			
			for(i=0;i<img_width;i++)
			{
				p2_dst[i] = p_buff[0];
				p1_dst[i] = p_buff[1];
				p0_dst[i] = p_buff[2];
				
				p_buff = p_buff +3;
			}
			p0_dst = p0_dst+img_width;
			p1_dst = p1_dst+img_width;
			p2_dst = p2_dst+img_width;
		}
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	dst->memory_valid[0] = 1;
	
	free(data_buff);
	fclose(f);
	return 0;
}
