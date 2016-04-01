#include <stdio.h>
#include "type.h"
#include "err.h"

struct bmpheader
{
	int bmpsize;	
	int bmpreserved;	
	int bmpoffbits;	
	
	int imginfosize;
	int imgwidth;		
	int imgheight;	
	short imgplanes;	
	short imgbitcount;	
	int imgcompression;	
	int imgsize;		
	int imgxpelspermeter;	
	int imgypelspermeter;
	int imgclrused;	
	int imgclrimportant;
};

struct colorlist
{
	unsigned char color_blue;
	unsigned char color_green;
	unsigned char color_red;
	char reserved;
};

int imgSaveBMP_8(ImgMat *src,char *filename)
{
#ifdef DEBUG
	if(src->type != TYPE_8UC1)
	{
		printf("IMG Error:\n\tin imgSaveGrayBMP: Wrong src format\n");
		return 1;
	}
#endif

	FILE *f;
	f = fopen(filename,"wb");

	if(f == NULL)
	{
		printf("IMG Error:\n\tin imgSaveGrayBMP: save image error\n");
		return 1;
	}
		
	int img_width;
	img_width = *(src->hidinfo + 5);
	
	int img_height;
	img_height = *(src->hidinfo + 6);

	int data_width;
	data_width = ((img_width-1)&0xFFFFFFFC)+4;
	
	int img_size = img_height*data_width;
	
	int i;
 
	short bmptype = 0x4d42;
		
	fseek(f,0,SEEK_SET);
	fwrite(&bmptype,1,2,f);
// printf("%s\n",filename);
	struct bmpheader my_bmp;
	
	my_bmp.bmpsize = 1078 + img_size;
	my_bmp.bmpreserved = 0;
	my_bmp.bmpoffbits = 1078;
	my_bmp.imginfosize = 40;
	my_bmp.imgwidth = img_width;
	my_bmp.imgheight = img_height;
	my_bmp.imgplanes = 1;
	my_bmp.imgbitcount = 8;
	my_bmp.imgcompression =0;
	my_bmp.imgsize = img_size;
	my_bmp.imgxpelspermeter = 11811;
	my_bmp.imgypelspermeter = 11811;
	my_bmp.imgclrused = 0;
	my_bmp.imgclrimportant = 0;
	
	fwrite(&my_bmp,1,52,f);
	
	struct colorlist color_gray[256];
	for(i=0;i<256;i++)
	{
		color_gray[i].color_blue = (unsigned char)i;
		color_gray[i].color_green = (unsigned char)i;
		color_gray[i].color_red = (unsigned char)i;
		color_gray[i].reserved = 0;
	}
	
	fwrite(color_gray,1,1024,f);

	unsigned char *data;
	
	int data0 = 0;
	
	if(data_width==img_width)
	{
		for(i=img_height-1;i>=0;i--)
		{
			data = src->data.ptr + i*(src->step);			
			fwrite(data,1,img_width,f);				
		}
	}
	else
	{
		for(i=img_height-1;i>=0;i--)
		{
			data = src->data.ptr + i*(src->step);
			fwrite(data,1,img_width,f);				
			fwrite(&data0,1,data_width-img_width,f);
		}
	}	
	
	fclose(f);
	
	return 0;
}
	
int imgSaveBMP_24(ImgMat *src,char *filename)
{	
	#ifdef DEBUG
	if(src->type != TYPE_8UC3)
	{
		printf("Img Error:\n\tin imgSaveBMP: Wrong src format\n");
		return 1;
	}
	#endif

	FILE *f;
	f = fopen(filename,"wb");

	#ifdef DEBUG	
	if(f == NULL)
	{
		printf("Img Error:\n\tin imgSaveBMP: save image error\n");
		return 1;
	}
	#endif

	// printf("aaaaaaaaaaaaaaaaaaa\n");
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	int img_size;
	img_size = img_width*img_height;
	
	unsigned char *p_src0;
	unsigned char *p_src1;
	unsigned char *p_src2;
	
	int data_width;
	data_width = ((img_width*3-1)&0xFFFFFFFC)+4;

	int data_size;
	data_size = data_width * img_height;

	int i,j;
	 
	short bmptype = 0x4d42;
		
	fseek(f,0,SEEK_SET);
	fwrite(&bmptype,1,2,f);

	struct bmpheader my_bmp;
	
	my_bmp.bmpsize = 54 + data_size;
	my_bmp.bmpreserved = 0;
	my_bmp.bmpoffbits = 54;
	my_bmp.imginfosize = 40;
	my_bmp.imgwidth = img_width;
	my_bmp.imgheight = img_height;
	my_bmp.imgplanes = 1;
	my_bmp.imgbitcount = 24;
	my_bmp.imgcompression =0;
	my_bmp.imgsize = data_size;
	my_bmp.imgxpelspermeter = 11811;
	my_bmp.imgypelspermeter = 11811;
	my_bmp.imgclrused = 0;
	my_bmp.imgclrimportant = 0;
	
	// fseek(f,2,SEEK_SET);
	fwrite(&my_bmp,1,52,f);
	
	int data0[3] = {0,0,0};
	
	if(data_width==img_width*3)
	{		
		for(j=img_height-1;j>=0;j--)
		{
			p_src0 = src->data.ptr + j*img_width;
			p_src1 = p_src0+img_size;
			p_src2 = p_src1+img_size;
			
			for(i=0;i<img_width;i++)
			{				
				fwrite(p_src0,1,1,f);
				fwrite(p_src1,1,1,f);
				fwrite(p_src2,1,1,f);
				
				p_src0++;
				p_src1++;
				p_src2++;		
			}
		}
	}
	else
	{		
		for(j=img_height-1;j>=0;j--)
		{
			p_src0 = src->data.ptr + j*img_width;
			p_src1 = p_src0+img_size;
			p_src2 = p_src1+img_size;
		
			for(i=0;i<img_width;i++)
			{					
				fwrite(p_src0,1,1,f);
				fwrite(p_src1,1,1,f);
				fwrite(p_src2,1,1,f);
				
				// if((i==200)&&(j==300))
				// printf("data is %d,%d,%d",*p_src0,*p_src1,*p_src2);
				
				p_src0++;
				p_src1++;
				p_src2++;
			}		
			
			fwrite(data0,1,data_width-img_width*3,f);	
		}			
	}
	
	fclose(f);
	
	return 0;
}

int imgSaveBMP_32(ImgMat *src,char *filename)
{	
	#ifdef DEBUG
	if(src->type != TYPE_8UC3)
	{
		printf("Img Error:\n\tin imgSaveBMP: Wrong src format\n");
		return 1;
	}
	#endif

	FILE *f;
	f = fopen(filename,"wb");

	#ifdef DEBUG	
	if(f == NULL)
	{
		printf("Img Error:\n\tin imgSaveBMP: save image error\n");
		return 1;
	}
	#endif

	int img_width;
	img_width = *(src->hidinfo + 5);
	
	int img_height;
	img_height = *(src->hidinfo + 6);
	
	int img_size;
	img_size = img_width*img_height;
	
	unsigned char *p_src0;
	unsigned char *p_src1;
	unsigned char *p_src2;
	unsigned char *p_src3;
	
	int data_width;
	data_width = img_width*4;

	int data_size;
	data_size = data_width * img_height;

	int i,j;
	 
	short bmptype = 0x4d42;
		
	fseek(f,0,SEEK_SET);
	fwrite(&bmptype,1,2,f);

	struct bmpheader my_bmp;
	
	my_bmp.bmpsize = 54 + data_size;
	my_bmp.bmpreserved = 0;
	my_bmp.bmpoffbits = 54;
	my_bmp.imginfosize = 40;
	my_bmp.imgwidth = img_width;
	my_bmp.imgheight = img_height;
	my_bmp.imgplanes = 1;
	my_bmp.imgbitcount = 32;
	my_bmp.imgcompression =0;
	my_bmp.imgsize = data_size;
	my_bmp.imgxpelspermeter = 11811;
	my_bmp.imgypelspermeter = 11811;
	my_bmp.imgclrused = 0;
	my_bmp.imgclrimportant = 1;
	
	// fseek(f,2,SEEK_SET);
	fwrite(&my_bmp,1,52,f);
	
	unsigned char buff[4];
	
	for(j=img_height-1;j>=0;j--)
	{
		p_src0 = src->data.ptr + j*img_width;
		p_src1 = p_src0+img_size;
		p_src2 = p_src1+img_size;
		p_src3 = p_src2+img_size;
		
		for(i=0;i<img_width;i++)
		{
			buff[0] = *p_src0;
			buff[1] = *p_src1;
			buff[2] = *p_src2;
			buff[3] = *p_src3;				
			fwrite(buff,1,4,f);
			
			p_src0++;
			p_src1++;
			p_src2++;
			p_src3++;
		}
	}
		
	fclose(f);
	
	return 0;
}

int imgSaveBMP(ImgMat *src,char *filename)
{
	if(src->type == TYPE_8UC1)
		return imgSaveBMP_8(src,filename);
	else if(src->type == TYPE_8UC3)
		return imgSaveBMP_24(src,filename);
	else if(src->type == TYPE_8UC4)
		return imgSaveBMP_32(src,filename);
	#ifdef DEBUG
	else 
	{
		printf("Img Error:\n\tin imgSaveBMP.\n");
		return 1;
	}
	#endif
}

int imgReadBMP(const char *filename,ImgMat *dst)
{
	FILE *f;
	f = fopen(filename,"rb");
	if(f == NULL)
	{
		printf("read image error\n");
		return 1;
	}
 
	short filetype;
		
	fseek(f,0,SEEK_SET);
	fread(&filetype,1,2,f);
	
	if(filetype != 0x4d42)
	{
		printf("read image error\n");
		return 1;
	}
	
	struct bmpheader my_bmp;
	
	fseek(f,2,SEEK_SET);
	fread(&my_bmp,1,52,f);
	
	int img_width;
	img_width = my_bmp.imgwidth;
	
	int img_height;
	img_height = my_bmp.imgheight;
	
	int img_size;
	img_size = img_width*img_height;
	
	int cn;
	cn = my_bmp.imgbitcount>>3;
	
	int data_width;
	data_width = ((img_width*cn-1)&0xFFFFFFFC)+4;
	
	int type;
	type = ((cn-1)<<3);
	
	imgInitializeMatHeader(dst,img_height,img_width,type);
	
	if((dst->data.ptr != NULL))
		free(dst->data.ptr);
	
	imgCreateMatData(dst);
	
	unsigned char *p_dst0;
	p_dst0 = dst->data.ptr;
	unsigned char *p_dst1;
	p_dst1 = p_dst0+img_size;
	unsigned char *p_dst2;
	p_dst2 = p_dst1+img_size;
	unsigned char *p_dst3;
	p_dst3 = p_dst2+img_size;
	
	int i,j;
	
	int pos;
	pos = my_bmp.bmpoffbits;
	fseek(f,pos,SEEK_SET);
	
	unsigned char *buff;
	
	if(cn==1)
	{
		for(j=img_height-1;j>=0;j--)
		{
			fseek(f,pos,SEEK_SET);
			buff = dst->data.ptr + j*img_width;		
			fread(buff,1,img_width,f);
						
			pos = pos+data_width;
		}
	}
	else if(cn == 3)
	{
		buff = malloc(3);
		for(j=img_height-1;j>=0;j--)
		{
			p_dst0 = dst->data.ptr+j*img_width;
			p_dst1 = p_dst0+img_size;
			p_dst2 = p_dst1+img_size;
			
			fseek(f,pos,SEEK_SET);
			
			for(i=0;i<img_width;i++)
			{	
				fread(buff,1,3,f);
								
				*p_dst0 = buff[0];
				*p_dst1 = buff[1];
				*p_dst2 = buff[2];
				p_dst0++;
				p_dst1++;
				p_dst2++;
			}
						
			pos = pos+data_width;
		}
		free(buff);
	}
	else if(cn == 4)
	{
		buff = malloc(4);
		for(j=img_height-1;j>=0;j--)
		{
			p_dst0 = dst->data.ptr+j*img_width;
			p_dst1 = p_dst0+img_size;
			p_dst2 = p_dst1+img_size;
			p_dst3 = p_dst2+img_size;
			
			fseek(f,pos,SEEK_SET);
			
			for(i=0;i<img_width;i++)
			{	
				fread(buff,1,4,f);
				*p_dst0 = buff[0];
				*p_dst1 = buff[1];
				*p_dst2 = buff[2];
				*p_dst3 = buff[3];
				
				p_dst0++;
				p_dst1++;
				p_dst2++;
				p_dst3++;
			}
						
			pos = pos+data_width;
		}
		free(buff);
	}
 
	fclose(f);
	
	return 0;

}
