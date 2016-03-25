#include <stdio.h>
#include <stdlib.h>

#include "type.h"

ImgMat *imgNewMatHeader();
int imgReadBMP(const char *filename,ImgMat *dst);
int imgSaveBMP(ImgMat *src,const char *filename);
void imgResize(ImgMat *src,ImgMat *dst);

void imgReadColorLUT(const char *file_name,ImgColorLUT *lut,int cn)
{
	#ifdef DEBUG
	if(lut==NULL)
	{
		printf("IMG Error:\n\tin imgReadColorLUT: color LUT is NULL.\n");
		exit(0);
	}
	#endif
	
	FILE *f;
	f = fopen(file_name,"r");	
	
	#ifdef DEBUG
	if(f == NULL)
	{
		printf("IMG Error:\n\tin imgReadColorLUT: file cannot open\n");
		exit(0);
	}
	#endif
	
	int *pr;
	int *pg;
	int *pb;
	int *p;
	pr = lut->r;
	pg = lut->g;
	pb = lut->b;
	p = lut->p;
	
	lut->cn = cn;
	
	int i;
	
	if(cn == 1)
	{
		for(i=0;i<256;i++)
		{
			fscanf(f,"%d\n",p);
			p++;
		}
	}
	else if(cn==3)
	{
		for(i=0;i<256;i++)
		{
			fscanf(f,"%d\t%d\t%d\n",pr,pg,pb);
			pr++;
			pg++;
			pb++;
		}
	}
	
	fclose(f);
}

void imgSaveColorLUT(ImgColorLUT *lut,const char *file_name)
{
	#ifdef DEBUG
	if(lut==NULL)
	{
		printf("IMG Error:\n\tin imgSaveColorLUT: color LUT is NULL.\n");
		exit(0);
	}
	#endif
	
	int cn;
	cn = lut->cn;
	
	#ifdef DEBUG
	if(cn==0)
	{
		printf("IMG Error:\n\tin imgSaveColorLUT: color LUT is NULL or format error.\n");
		exit(0);
	}
	#endif
	
	int *pr;
	int *pg;
	int *pb;
	int *p;
	pr = lut->r;
	pg = lut->g;
	pb = lut->b;
	p = lut->p;
	
	FILE *f;
	f = fopen(file_name,"w");
	
	#ifdef DEBUG	
	if(f == NULL)
	{
		printf("IMG Error:\n\tin imgSaveColorLUT: file cannot open\n");
		exit(0);
	}
	#endif
	
	int i;
	if(cn==1)
	{
		for(i=0;i<256;i++)
		{
			fprintf(f,"%d\n",*p);
			p++;
		}
	}
	else if(cn==3)
	{
		for(i=0;i<256;i++)
		{
			fprintf(f,"%d\t%d\t%d\n",*pr,*pg,*pb);
			pr++;
			pg++;
			pb++;
		}
	}
	
	fclose(f);
}

ImgMat *imgNewMatHeader();
ImgMat *imgCreateMat(int height,int width,char type);

void imgReadColorBar(const char *file_name,ImgColorLUT *lut)
{
	#ifdef DEBUG
	if(lut==NULL)
	{
		printf("IMG Error:\n\tin imgReadColorBar: color LUT is NULL.\n");
		exit(0);
	}
	#endif
	
	ImgMat *bar;
	bar = imgNewMatHeader();
	imgReadBMP(file_name,bar);
	
	int img_height;
	img_height = bar->height;
	
	int img_width;
	img_width = bar->width;
	
	if(img_height!=256)
		Resize(bar,NULL,256,img_width);
	
	unsigned char *p0_bar;
	unsigned char *p1_bar;
	unsigned char *p2_bar;
	p0_bar = bar->data.ptr;
	p1_bar = p0_bar + bar->size;
	p2_bar = p1_bar + bar->size;
	
	int cn;
	cn = ((bar->type&0xF8)>>3)+1;
	
	lut->cn = cn;
	
	int sum;
	int sum_r,sum_g,sum_b;
	
	int i,j;
	
	if(cn==1)
	{
		for(j=0;j<256;j++)
		{
			sum = 0;
			for(i=0;i<img_width;i++)
				sum = sum+p0_bar[i];
			
			p0_bar = p0_bar+32;
			lut->p[255-j] = sum/img_width;
		}
	}
	if(cn>=3)
	{
		for(j=0;j<256;j++)
		{
			sum_r = 0;
			sum_g = 0;
			sum_b = 0;
			for(i=0;i<img_width;i++)
			{
				sum_b = sum_b+p0_bar[i];
				sum_g = sum_g+p1_bar[i];
				sum_r = sum_r+p2_bar[i];
			}
			p0_bar = p0_bar+32;
			p1_bar = p1_bar+32;
			p2_bar = p2_bar+32;

			lut->r[255-j] = sum_r/img_width;
			lut->g[255-j] = sum_g/img_width;
			lut->b[255-j] = sum_b/img_width;
		}
	}
	
	imgReleaseMat(bar);
}	

void imgSaveColorBar(ImgColorLUT *lut,const char *file_name)
{
	#ifdef DEBUG
	if(lut==NULL)
	{
		printf("IMG Error:\n\tin imgSaveColorBar: color LUT is NULL.\n");
		exit(0);
	}
	#endif
	
	int img_height;
	img_height = 256;
	
	ImgMat *bar;
	bar = imgCreateMat(256,32,TYPE_8UC3);
	
	int i,j;

	unsigned char *p0_bar;
	unsigned char *p1_bar;
	unsigned char *p2_bar;
	p0_bar = bar->data.ptr;
	p1_bar = p0_bar + 8192;
	p2_bar = p1_bar + 8192;
	
	int r,g,b;

	for(j=0;j<256;j++)
	{		
		r = lut->r[255-j];
		g = lut->g[255-j];
		b = lut->b[255-j];
		
		for(i=0;i<32;i++)
		{
			p0_bar[i] = b;
			p1_bar[i] = g;
			p2_bar[i] = r;
		}
		
		p0_bar = p0_bar + 32;
		p1_bar = p1_bar + 32;
		p2_bar = p2_bar + 32;
		
	}	
	
	imgSaveBMP(bar,file_name);
	imgReleaseMat(bar);
}

void imgColorLUT(ImgMat *src,ImgMat *dst,ImgColorLUT *lut)
{
	#ifdef DEBUG
	if(src==NULL)
	{
		printf("IMG Error\n\tin imgColorLUT:source is NULL\n");
		exit(0);
	}	
	if(dst==NULL)
	{
		printf("IMG Error\n\tin imgColorLUT:destination is NULL\n");
		exit(0);
	}
	if(lut == NULL)
	{
		printf("IMG Error\n\tin imgColorLUT:color LUT is NULL\n");
		exit(0);
	}
	#endif	
	
	int cn_src;
	cn_src = ((src->type&0xF8)>>3)+1;
	
	int cn_dst;
	cn_dst = ((dst->type&0xF8)>>3)+1;
		
	#ifdef DEBUG
	if((cn_dst<3)&&(cn_src>=3))
	{
		printf("IMG Error\n\tin imgColorLUT:\n");
		exit(0);
	}
	#endif
	
	int img_height;
	img_height = src->height;
	
	int img_width;
	img_width = src->width;
	
	int img_size;
	img_size = img_height*img_width;
	
	if(cn_dst!=lut->cn)
		dst->type = (dst->type&0x07)+((lut->cn-1)<<3);
	
	if((dst->width != img_width)||(dst->height != img_height) || (cn_dst!=lut->cn))
	{
		imgMatRedefine(dst,img_height,img_width,dst->type);
		cn_dst = lut->cn;
	}	
	
	unsigned char *p0_src;
	unsigned char *p1_src;
	unsigned char *p2_src;
	p0_src = src->data.ptr;
	p1_src = p0_src +img_size;
	p2_src = p1_src +img_size;
	
	unsigned char *p0_dst;
	unsigned char *p1_dst;
	unsigned char *p2_dst;
	p0_dst = dst->data.ptr;
	p1_dst = p0_dst +img_size;
	p2_dst = p1_dst +img_size;
	
	int data,data_r,data_g,data_b;
	int i;
	
	if(cn_dst ==1)
	{
		for(i=0;i<img_size;i++)
		{
			data = p0_src[i];
			p0_dst[i] = lut->p[data];
		}
	}
	else if((cn_src ==1)&&(cn_dst >=3))
	{
		for(i=0;i<img_size;i++)
		{
			data = p0_src[i];
			p0_dst[i] = lut->b[data];
			p1_dst[i] = lut->g[data];
			p2_dst[i] = lut->r[data];
		}
	}
	else if((cn_src >=3)&&(cn_dst >=3))
	{
		for(i=0;i<img_size;i++)
		{
			data_b = p0_src[i]; 
			data_g = p1_src[i]; 
			data_r = p2_src[i]; 
			p0_dst[i] = lut->b[data];
			p1_dst[i] = lut->g[data];
			p2_dst[i] = lut->r[data];
		}
	}
}

void ColorLUT(ImgMat *src,ImgMat *dst,ImgColorLUT *lut)
{
	#ifdef DEBUG
	if(lut == NULL)
	{
		printf("IMG Error\n\tin imgColorLUT:color LUT is NULL\n");
		exit(0);
	}
	#endif	
	
	if(dst==NULL)
	{
		int dst_type;
		dst_type = ((src->type)&0x07)+((lut->cn-1)<<3);
		
		if(dst_type == src->type)
			imgColorLUT(src,src,lut);
		else
		{
			dst = imgCreateMat(src->height,src->width,dst_type);
			imgColorLUT(src,dst,lut);
			free(src->data.ptr);
			free(src->hidinfo);
			*src = *dst;
			free(dst);
		}
	}
	else
		imgColorLUT(src,dst,lut);
}

void imgGrayToPesu(ImgMat *src,ImgMat *dst)
{
	#ifdef DEBUG
	if(src==NULL)
	{
		printf("IMG Error\n\tin imgGrayToPesu:source is NULL\n");
		exit(0);
	}	
	if(dst==NULL)
	{
		printf("IMG Error\n\tin imgGrayToPesu:destination is NULL\n");
		exit(0);
	}
	if(src->type != TYPE_8UC1)
	{
		printf("IMG Error\n\tin imgGrayToPesu:sorce type is not TYPE_8UC1\n");
		exit(0);
	}
	#endif
	
	// if((dst->height != src->height)||(dst->width !=src->width)||(dst->type != TYPE_8UC3))
		// imgMatRedefine(dst,src->height,src->width,TYPE_8UC3);
	
	ImgColorLUT lut;
	imgReadColorLUT("./pesu.txt",&lut,3);
	imgColorLUT(src,dst,&lut);
}

void GrayToPesu(ImgMat *src,ImgMat *dst)
{
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,TYPE_8UC3);
		imgGrayToPesu(src,dst);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}	
	else 
		imgGrayToPesu(src,dst);
}

void imgGrayToRainbow(ImgMat *src,ImgMat *dst)
{
	#ifdef DEBUG
	if(src==NULL)
	{
		printf("IMG Error\n\tin imgGrayToRainbow:source is NULL\n");
		exit(0);
	}	
	if(dst==NULL)
	{
		printf("IMG Error\n\tin imgGrayToRainbow:destination is NULL\n");
		exit(0);
	}
	if(src->type != TYPE_8UC1)
	{
		printf("IMG Error\n\tin imgGrayToRainbow:sorce type is not TYPE_8UC1\n");
		exit(0);
	}
	#endif
	
	// if((dst->height != src->height)||(dst->width !=src->width)||(dst->type != TYPE_8UC3))
		// imgMatRedefine(dst,src->height,src->width,TYPE_8UC3);
	
	ImgColorLUT lut;
	imgReadColorLUT("./rainbow.txt",&lut,3);
	imgColorLUT(src,dst,&lut);
}

void GrayToRainbow(ImgMat *src,ImgMat *dst)
{
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,TYPE_8UC3);
		imgGrayToRainbow(src,dst);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}	
	else 
		imgGrayToRainbow(src,dst);
}




