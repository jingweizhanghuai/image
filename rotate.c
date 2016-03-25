#include <stdio.h>
#include "type.h"
#include "err.h"

void imgRotate_x(ImgMat *src,ImgMat *dst,int angle);

void imgRotate(ImgMat *src,ImgMat *dst,int angle)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgRotate,src);
	DESTINATION_ERROR_CHECK(imgRotate,dst);	
	#endif
	
	if(angle ==180)
		if((dst->width != src->width)||(dst->height != src->height)||(dst->type != src->type))
			imgMatRedefine(dst,src->height,src->width,src->type);

	if((angle ==90)||(angle == 270))
		if((dst->height != src->width)||(dst->width != src->height)||(dst->type != src->type))
			imgMatRedefine(dst,src->width,src->height,src->type);
		
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;	
	
	int dst_width;
	dst_width = dst->width;
	
	int dst_height;
	dst_height = dst->height;
	
	int img_size;
	img_size = src_height*src_width;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k;
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char *src_data;	
	unsigned char *dst_data;
	
	if(angle == 90)
	{
		p_src = p_src + (src_width*(src_height-1));
		
		for(k=0;k<cn;k++)
		{
			dst_data = p_dst;
			for(j=0;j<dst_height;j++)
			{
				src_data = p_src+j;
			
				for(i=0;i<dst_width;i++)
				{
					dst_data[0] = src_data[0];
					dst_data = dst_data+1;
					src_data = src_data-src_width;
				}
			}
			
			p_src = p_src+img_size;
			p_dst = p_dst+img_size;
		}
	}
	else if(angle == 270)
	{
		p_src = p_src + src_width;
		for(k=0;k<cn;k++)
		{		
			dst_data = p_dst;
			for(j=0;j<dst_height;j++)
			{
				src_data = p_src-j;
			
				for(i=0;i<dst_width;i++)
				{
					dst_data[0] = src_data[0];
					dst_data = dst_data+1;
					src_data = src_data+src_width;
				}
			}
		
			p_src = p_src+img_size;
			p_dst = p_dst+img_size;
		}
	}
	else if(angle == 180)
	{
		p_src = p_src + img_size-1;
		for(k=0;k<cn;k++)
		{	
			dst_data = p_dst;
			src_data = p_src;
			for(j=0;j<dst_height;j++)
			{
				for(i=0;i<dst_width;i++)
				{
					dst_data[0] = src_data[0];
					dst_data = dst_data+1;
					src_data = src_data-1;
				}
			}
			
			p_src = p_src+img_size;
			p_dst = p_dst+img_size;
		}
	}
	else
		imgRotate_x(src,dst,angle);
		
}

float imgSin(int x);
float imgCos(int x);
void imgAffine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd);

void imgRotate_x(ImgMat *src,ImgMat *dst,int angle)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgRotate,src);
	DESTINATION_ERROR_CHECK(imgRotate,dst);	
	#endif
	
	ImgPoint ps[3];
	ImgPoint pd[3];
	
	float dx,dy;
	
	int half_width;
	half_width = (src->width+1)/2;
	
	int half_height;
	half_height = (src->height+1)/2;
	
	ps[0].x = half_width;
	ps[0].y = half_height;
	pd[0].x = (dst->width+1)/2;
	pd[0].y = (dst->height+1)/2;
	
	ps[1].x = 0;
	ps[1].y = half_height;
	
	dx = (float)half_width*imgCos(angle);
	dy = (float)half_width*imgSin(angle);
	
	pd[1].x = pd[0].x-(int)(dx+0.5);
	pd[1].y = pd[0].y-(int)(dy+0.5);
	
	ps[2].x = half_width;
	ps[2].y = 0;
	
	dx = (float)half_height*imgSin(angle);
	dy = (float)half_height*imgCos(angle);
	
	pd[2].x = pd[0].x+(int)(dx+0.5);
	pd[2].y = pd[0].y-(int)(dy+0.5);
	
	imgAffine(src,ps,dst,pd);
}	

ImgMat *imgCreateMat(int height,int width,char type);

void Rotate(ImgMat *src,ImgMat *dst,int angle)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgRotate,src);
	#endif
	
	if(dst==NULL)
	{
		if((angle ==90)||(angle == 270))
			dst = imgCreateMat(src->width,src->height,src->type);
		else if(angle == 180)
			dst = imgCreateMat(src->height,src->width,src->type);
		else
		{
			int length;
			length = (src->height>src->width)?src->height:src->width;
			length = (int)((float)length*1.414);
			dst = imgCreateMat(length,length,src->type);
		}
		
		imgRotate(src,dst,angle);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgRotate(src,dst,angle);
}
