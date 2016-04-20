#include <stdio.h>
#include "type.h"
#include "err.h"

#define PTR(mat) {\
	p##mat[0] = p_##mat;\
	for(i=1;i<mat->height;i++)\
		p##mat[i] = p##mat[i-1]+mat->width;\
}\

#define SRC(x,y) *(psrc[y]+(x))
#define DST(x,y) *(pdst[y]+(x))

ImgMat *imgCreateMat(int height,int width,char type);
void imgReleaseMatData(ImgMat *src);

void imgCrop(ImgMat *src,ImgMat *dst,ImgRect *rect)//Point point1,ImgPoint point2)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgColorInvert,src);
	DESTINATION_ERROR_CHECK(imgColorInvert,dst);
	if((rect->width ==0)||(rect->height ==0))
	{
		printf("IMG Error\n\tin imgCrop:size of crop is wrong.\n");
		exit(0);
	}
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	int p1x,p1y,p2x,p2y;
	
	p1x = rect->x;
	p2x = p1x+rect->width-1;
	p1y = rect->y;
	p2y = p1y+rect->height-1;
	
	if((dst->height != rect->height)||(dst->width != rect->width)||(dst->type != src->type))
		imgMatRedefine(dst,rect->height,rect->width,src->type);
	
	int dst_width;
	dst_width = dst->width;
	
	int dst_height;
	dst_height =dst->height;	
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	int i,j,k;
	
	unsigned char **psrc;
	psrc = malloc(img_height<<2);
	unsigned char **pdst;	
	pdst = malloc(img_height<<2);
	
	// printf("cn is %d\n",cn);
	// printf("dst_height is %d\n",dst_height);
	
	if((p1x>0)&&(p1y>0)&&(p2x<img_width)&&(p2y<img_height))
	{
		for(k=0;k<cn;k++)
		{
			PTR(src);
			PTR(dst);
			for(j=0;j<dst_height;j++)
				for(i=0;i<dst_width;i++)
					DST(i,j) = SRC(i+p1x,j+p1y);
			
			p_src = p_src+src->size;
			p_dst = p_dst+dst->size;			
		}
	}
	else
	{
		for(k=0;k<cn;k++)
		{
			PTR(src);
			PTR(dst);
			
			j=0;
			while(j+p1y<0)
			{
				for(i=0;i<dst_width;i++)
					DST(i,j) = 0;
				
				j=j+1;
			}		
			
			while((j+p1y<img_height)&&(j<dst_height))
			{
				i=0;
				while(i+p1x<0)
				{
					DST(i,j) = 0;
					i = i+1;
				}

				while((i+p1x<img_width)&&(i<dst_width))
				{
					DST(i,j) = SRC(i+p1x,j+p1y);
					i = i+1;
				}
				
				while(i<dst_width)
				{
					DST(i,j) = 0;
					i = i+1;
				}
				j=j+1;
			}
			
			while(j<dst_height)
			{
				for(i=0;i<dst_width;i++)
					DST(i,j) = 0;
				
				j=j+1;
			}
			
			p_src = p_src+src->size;
			p_dst = p_dst+dst->size;	
		}
	}
}

void Crop(ImgMat *src,ImgMat *dst,ImgRect *rect)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgCrop,src);
	if((rect->width ==0)||(rect->height ==0))
	{
		printf("IMG Error\n\tin imgCrop:size of crop is wrong.\n");
		exit(0);
	}
	#endif
	
	if(dst == NULL)
	{
		dst = imgCreateMat(rect->height,rect->width,src->type);
		imgCrop(src,dst,rect);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgCrop(src,dst,rect);
}
