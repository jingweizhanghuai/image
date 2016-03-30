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
	jpeg_create_compress(&cinfo);
	
	struct jpeg_error_mgr jerr;	
	cinfo.err = jpeg_std_error(&jerr);
	
	jpeg_stdio_dest(&cinfo, f);
	cinfo.image_width = img_width;   //* image width and height, in pixels   
	cinfo.image_height = img_height;   
	cinfo.input_components = 3;   //* # of color components per pixel   
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo,JPEG_QUALITY,1);
	
	jpeg_start_compress(&cinfo, TRUE);
	
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
}


int imgSaveJPG_8(ImgMat *src,char *filename)
{	
	#ifdef DEBUG
	if((src->type&0x07) != TYPE_8U)
	{
		printf("Img Error:\n\tin imgSaveJPG: Wrong src format\n");
		return 1;
	}
	#endif
	
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
	jpeg_create_compress(&cinfo);
	
	struct jpeg_error_mgr jerr;	
	cinfo.err = jpeg_std_error(&jerr);
	
	jpeg_stdio_dest(&cinfo, f);
	cinfo.image_width = img_width;   //* image width and height, in pixels   
	cinfo.image_height = img_height;   
	cinfo.input_components = 1;   //* # of color components per pixel   
	cinfo.in_color_space = JCS_GRAYSCALE;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo,JPEG_QUALITY,1);
	
	jpeg_start_compress(&cinfo, TRUE);
	
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
}

	
	
/*
int Bmp2Jpg(const char *bmp_file, const char *jeg_file, const int width, const int height) 
{   
	FILE *fd;   
	int ret;   
	unsigned char *data;   
	long sizeImage;   
	int depth = 3;   
	JSAMPROW * row_pointer;   
	long rgb_index = 0;   
	int i=0;   
	struct jpeg_compress_struct cinfo;   
	struct jpeg_error_mgr jerr;   
	FILE *outfile;               
	
	// Read bmp image data   
	sizeImage = width*height*3;   
	data = (unsigned char*)malloc(sizeImage);   
	fd = fopen(bmp_file, "rb");   
	if(!fd)   
	{     
		printf("ERROR1: Can not open the image.\n");     
		free(data);     
		return -1;   
	}   
	fseek(fd, 54, SEEK_SET);    
	ret = fread(data, sizeof(unsigned char)*sizeImage, 1, fd);   
	if(ret == 0)   
	{     
		if(ferror(fd))     
		{       
			printf("\nERROR2: Can not read the pixel data.\n");       
			free(data);       
			fclose(fd);       
			return -1;     
		}   
	}   
	//Convert BMP to JPG   
	cinfo.err = jpeg_std_error(&jerr);   //* Now we can initialize the JPEG compression object.
	jpeg_create_compress(&cinfo);   
	if ((outfile = fopen(jeg_file, "wb")) == NULL)   
	{     
		fprintf(stderr, "can't open %s\n", jeg_file);     
		return -1;   
	}
	
	jpeg_stdio_dest(&cinfo, outfile); 
	
	cinfo.image_width = width;   //* image width and height, in pixels   
	cinfo.image_height = height;   
	cinfo.input_components = depth;   //* # of color components per pixel   
	cinfo.in_color_space = JCS_RGB;   //* colorspace of input image 
	
	jpeg_set_defaults(&cinfo);    
	//Now you can set any non-default parameters you wish to.    
	//Here we just illustrate the use of quality (quantization table) scaling:   
	jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE );   //* limit to baseline-JPEG values   
	jpeg_start_compress(&cinfo, TRUE);
	
	//一次写入   
	int j=0;   
	row_pointer = malloc(height*width*3);   
	char *line[300];   
	for(i=0;i<height;i++)   
	{       
		unsigned char * lineData = NULL;     
		lineData = malloc(width*3);     
		line[i]=lineData;     
		for(j=0;j<width;j++)     
		{     
			lineData[j*3+2] = data[rgb_index];       
			rgb_index ++;       
			lineData[j*3+1] = data[rgb_index];       
			rgb_index ++;       
			lineData[j*3+0] = data[rgb_index];       
			rgb_index ++;     
		}     
		row_pointer[height-i-1] = lineData;      
	}   
	jpeg_write_scanlines(&cinfo, row_pointer, height);   
	jpeg_finish_compress(&cinfo);   
	jpeg_destroy_compress(&cinfo);   
	for (i=0; i<height; i++)   
	{     
		free(line[i]);   
	}    
	free(row_pointer);   
	free(data);    
	fclose(fd);   
	fclose(outfile);   
	return 0; 
} 
*/