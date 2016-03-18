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
	
	int i;
	int result;
	
	if(type == TYPE_8U)
	{
		unsigned char *p_src_8u;
		p_src_8u = src->data.ptr;
		unsigned char *p_dst_8u;
		p_dst_8u = dst->data.ptr;
		
		if(cn = 1)
		{
			for(i=0;i<img_size;i++)
			{
				result = (*p_src_8u)+(*x);
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				*p_dst_8u = result;
				
				p_src_8u++;
				p_dst_8u++;
			}
		}
		else if(cn >= 3)
		{
			for(i=0;i<img_size;i++)
			{
				result = p_src_8u[0]+x[0];
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8u[0] = result;
				
				result = p_src_8u[1]+x[1];
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8u[1] = result;
				
				result = p_src_8u[2]+x[2];
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8u[2] = result;
				
				p_src_8u = p_src_8u+cn;
				p_dst_8u = p_dst_8u+cn;
			}
		}
	}
	else if(type == TYPE_8S)
	{
		char *p_src_8s;
		p_src_8s = (char *)src->data.ptr;
		char *p_dst_8s;
		p_dst_8s = (char *)dst->data.ptr;
		
		if(cn = 1)
		{
			for(i=0;i<img_size;i++)
			{
				result = (*p_src_8s)+(*x);
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				*p_dst_8s = result;
				
				p_src_8s++;
				p_dst_8s++;
			}
		}
		else if(cn >= 3)
		{
			for(i=0;i<img_size;i++)
			{
				result = p_src_8s[0]+x[0];
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8s[0] = result;
				
				result = p_src_8s[1]+x[1];
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8s[1] = result;
				
				result = p_src_8s[2]+x[2];
				
				if(result>255)
					result = 255;
				else if(result<0)
					result = 0;
				
				p_dst_8s[2] = result;
				
				p_src_8s = p_src_8s+cn;
				p_dst_8s = p_dst_8s+cn;
			}
		}
	}
}

ImgMat *imgCreateMat(int height,int width,char type);

void Increase(ImgMat *src,ImgMat *dst,int *x)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgIncrease,src);
	#endif
	
	if(dst == NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgIncrease(src,dst,x);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgIncrease(src,dst,x);
}
