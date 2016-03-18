#include <stdio.h>
#include "type.h"
#include "err.h"

#define SRC(x,y,c) p_src[(y)*img_width*cn + (x)*cn+c]
#define DST(x,y,c) p_dst[(y)*dst_width*cn + (x)*cn+c]

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
	
	int i,j;
	
	// printf("cn is %d\n",cn);
	// printf("dst_height is %d\n",dst_height);
	
	if((p1x>0)&&(p1y>0)&&(p2x<img_width)&&(p2y<img_height))
	{
		if(cn == 1)
		{	
			for(j=0;j<dst_height;j++)
				for(i=0;i<dst_width;i++)
					DST(i,j,0) = SRC(i+p1x,j+p1y,0);
		}
		else if(cn == 3)
		{
			for(j=0;j<dst_height;j++)
				for(i=0;i<dst_width;i++)
				{
					DST(i,j,0) = SRC(i+p1x,j+p1y,0);
					DST(i,j,1) = SRC(i+p1x,j+p1y,1);
					DST(i,j,2) = SRC(i+p1x,j+p1y,2);
				}
		}
	}
	else
	{
		if(cn == 1)
		{
			while(j+p1y<0)
			{
				for(i=0;i<dst_width;i++)
					DST(i,j,0) = 0;
				
				j=j+1;
			}
			
			while((j+p1y<img_height)&&(j<dst_height))
			{
				while(i+p1x<0)
				{
					DST(i,j,0) = 0;
					i = i+1;
				}
				
				while((i+p1x<img_width)&&(i<dst_width))
				{
					DST(i,j,0) = SRC(i+p1x,j+p1y,0);
					i = i+1;
				}
				
				while(i<dst_width)
				{
					DST(i,j,0) = 0;
					i = i+1;
				}
			}
			
			while(j<dst_height)
			{
				for(i=0;i<dst_width;i++)
					DST(i,j,0) = 0;
				
				j=j+1;
			}
		}
		
		if(cn == 3)
		{
			while(j+p1y<0)
			{
				for(i=0;i<dst_width;i++)
				{
					DST(i,j,0) = 0;
					DST(i,j,1) = 0;
					DST(i,j,2) = 0;
				}
				
				j=j+1;
			}
			
			while((j+p1y<img_height)&&(j<dst_height))
			{
				while(i+p1x<0)
				{
					DST(i,j,0) = 0;
					DST(i,j,1) = 0;
					DST(i,j,2) = 0;
					i = i+1;
				}
				
				while((i+p1x<img_width)&&(i<dst_width))
				{
					DST(i,j,0) = SRC(i+p1x,j+p1y,0);
					DST(i,j,1) = SRC(i+p1x,j+p1y,1);
					DST(i,j,2) = SRC(i+p1x,j+p1y,2);
					i = i+1;
				}
				
				while(i<dst_width)
				{
					DST(i,j,0) = 0;
					DST(i,j,1) = 0;
					DST(i,j,2) = 0;
					i = i+1;
				}
			}
			
			while(j<dst_height)
			{
				for(i=0;i<dst_width;i++)
				{
					DST(i,j,0) = 0;
					DST(i,j,1) = 0;
					DST(i,j,2) = 0;
				}
				
				j=j+1;
			}
		}
	}
}

void Crop(ImgMat *src,ImgMat *dst,ImgRect *rect)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgCrop,src1);
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
