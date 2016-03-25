#include <stdio.h>
#include "type.h"
#include "err.h"

void imgMirror(ImgMat *src,ImgMat *dst,int type)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMirror,src);
	DESTINATION_ERROR_CHECK(imgMirror,dst);
	if((type != 1)&&(type != 2))
	{
		printf("IMG Error:\n\tin imgMirror:\n");
		exit(0);
	}
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	int img_size;
	img_size = img_height*img_width;
	
	if((dst->width != src->width)||(dst->height != src->height)||(dst->type != src->type))
		imgMatRedefine(dst,src->height,src->width,src->type);

	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k;
	
	unsigned char *p_src;
	p_src= src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char *src_data;
	unsigned char *dst_data;
	
	if(type==1)
	{
		for(k=0;k<cn;k++)
		{
			dst_data = p_dst;
			src_data = p_src+img_width-1;
			
			for(j=0;j<img_height;j++)
			{				
				for(i=0;i<img_width;i++)
				{
					*dst_data = *src_data;
					dst_data = dst_data+1;
					src_data = src_data-1;
				}
				src_data = src_data+img_width+img_width;
				
			}
			
			p_src = p_src+img_size;
			p_dst = p_dst+img_size;
		}
	}
	else if(type == 2)
	{
		for(k=0;k<cn;k++)
		{
			dst_data = p_dst;
			src_data = p_src+img_size-img_width;
			
			for(j=0;j<img_height;j++)
			{				
				for(i=0;i<img_width;i++)
				{
					*dst_data = *src_data;
					dst_data = dst_data+1;
					src_data = src_data+1;
				}
				
				src_data = src_data-img_width-img_width;
			}
				
			p_src = p_src+img_size;
			p_dst = p_dst+img_size;
		}
	}
}

ImgMat *imgCreateMat(int height,int width,char type);

void Mirror(ImgMat *src,ImgMat *dst,int type)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMirror,src);
	#endif
	
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgMirror(src,dst,type);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgMirror(src,dst,type);
}
