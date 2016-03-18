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

#define BAR_PTR {\
	p_bar = malloc(img_height<<2);\
	p_bar[0] = bar->data.ptr;\
	for(i=1;i<img_height;i++)\
		p_bar[i] = p_bar[i-1]+bar->step;\
}\

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
	
	unsigned char *p_bar;
	p_bar = bar->data.ptr;
	
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
			{
				sum = sum+(*p_bar);
				p_bar = p_bar+1;
			}
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
				sum_b = sum_b+p_bar[0];
				sum_g = sum_g+p_bar[1];
				sum_r = sum_r+p_bar[2];
				p_bar = p_bar+cn;
			}
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

	unsigned char **p_bar;
	BAR_PTR;
	
	unsigned char *p;
	
	int r,g,b;

	for(j=0;j<256;j++)
	{
		p = p_bar[j];
		r = lut->r[255-j];
		g = lut->g[255-j];
		b = lut->b[255-j];
		
		for(i=0;i<32;i++)
		{
			p[0] = b;
			p[1] = g;
			p[2] = r;
			p = p+3;
		}
	}
	
	free(p_bar);	
	
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
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int data,data_r,data_g,data_b;
	int i;
	
	if(cn_dst ==1)
	{
		for(i=0;i<img_size;i++)
		{
			data = *p_src;
			*p_dst = lut->p[data];
			p_src = p_src+1;
			p_dst = p_dst+1;
		}
	}
	else if((cn_src ==1)&&(cn_dst >=3))
	{
		for(i=0;i<img_size;i++)
		{
			data = *p_src;
			p_dst[0] = lut->b[data];
			p_dst[1] = lut->g[data];
			p_dst[2] = lut->r[data];
			
			p_src= p_src+1;
			p_dst= p_dst+cn_dst;
		}
	}
	else if((cn_src >=3)&&(cn_dst >=3))
	{
		for(i=0;i<img_size;i++)
		{
			data_b = p_src[0]; 
			data_g = p_src[1]; 
			data_r = p_src[2]; 
			p_dst[0] = lut->b[data_b];
			p_dst[1] = lut->g[data_g];
			p_dst[2] = lut->r[data_r];
			
			p_src= p_src+cn_src;
			p_dst= p_dst+cn_dst;
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




