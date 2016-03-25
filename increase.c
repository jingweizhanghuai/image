#include <stdio.h>
#include "type.h"
#include "err.h"

void imgCopyMat(ImgMat *src,ImgMat *dst);

void imgIncrease(ImgMat *src,ImgMat *dst,int *value)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgIncrease,src);
	DESTINATION_ERROR_CHECK(imgIncrease,dst);
	#endif
	
	int x[3];
	
	int cn;
	cn = (src->type>>3)+1;
	
	if(value==NULL)
	{
		x[0] = 5;
		x[1] = 5;
		x[2] = 5;
	}
	else
	{
		if(cn==3)
		{
			x[0] = value[0];
			x[1] = value[1];
			x[2] = value[2];
		}
		else if(cn == 1)
			*x = *value;
	}
		
	if((dst->width != src->width)||(dst->height != src->height)||(dst->type != src->type))
		imgMatRedefine(dst,src->height,src->width,src->type);	
	
	int img_size;
	img_size = src->height*src->width;
	
	int type;
	type = (src->type)&0x07;
	
	int i,k;
	int result;
	int data;
	
	if(type == TYPE_8U)
	{
		unsigned char *p_src_8u;
		p_src_8u = src->data.ptr;
		unsigned char *p_dst_8u;
		p_dst_8u = dst->data.ptr;
		
		for(k=0;k<cn;k++)
		{
			data = x[k];
			for(i=0;i<img_size;i++)
			{
				result = p_src_8u[i]+data;
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8u[i] = result;
			}
			
			p_src_8u = p_src_8u+img_size;
			p_dst_8u = p_dst_8u+img_size;			
		}
	}
	else if(type == TYPE_8S)
	{
		char *p_src_8s;
		p_src_8s = (char *)src->data.ptr;
		char *p_dst_8s;
		p_dst_8s = (char *)dst->data.ptr;
		
		for(k=0;k<cn;k++)
		{
			data = x[k];
			for(i=0;i<img_size;i++)
			{
				result = p_src_8s[i]+data;
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8s[i] = result;
			}
			
			p_src_8s = p_src_8s+img_size;
			p_dst_8s = p_dst_8s+img_size;			
		}
	}
}

ImgMat *imgCreateMat(int height,int width,char type);

void Increase(ImgMat *src,ImgMat *dst,int *x)
{
	if(dst == NULL)
		imgIncrease(src,src,x);
	else
		imgIncrease(src,dst,x);
}
