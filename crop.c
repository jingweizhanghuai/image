#include <stdio.h>
#include "type.h"

#define SRC(x,y,c) p_src[(y)*img_width*cn + (x)*cn+c]
#define DST(x,y,c) p_dst[(y)*dst_width*cn + (x)*cn+c]

ImgMat *imgCreateMat(int height,int width,char type);
void imgReleaseMatData(ImgMat *src);

void imgCrop(ImgMat *src,ImgPoint point1,ImgPoint point2)
{
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	int p1x,p1y,p2x,p2y;
	
	if(point1.x<point2.x)
	{
		p1x = (point1.x>0)?point1.x:0;
		p2x = (point2.x<img_width)?point2.x:img_width;
	}
	else
	{
		p1x = (point2.x>0)?point2.x:0;
		p2x = (point1.x<img_width)?point1.x:img_width;
	}
	
	if(point1.y<point2.y)
	{
		p1y = (point1.y>0)?point1.y:0;
		p2y = (point2.y<img_height)?point2.y:img_height;
	}
	else
	{
		p1y = (point2.y>0)?point2.y:0;
		p2y = (point1.y<img_height)?point1.y:img_height;
	}
	
	
	// printf("p1x is %d\n",p1x);
	// printf("p1y is %d\n",p1y);
	// printf("p2x is %d\n",p2x);
	// printf("p2y is %d\n",p2y);
	
	int dst_width;
	dst_width = (p2x-p1x)+1;
	
	int dst_height;
	dst_height =(p2y-p1y)+1;
	
	ImgMat *dst;
	dst = imgCreateMat(dst_height,dst_width,src->type);
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	int i,j;
	
	// printf("cn is %d\n",cn);
	// printf("dst_height is %d\n",dst_height);
	
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
	
	imgReleaseMatData(src);
	free(src->hidinfo);
	
	src->hidinfo = dst->hidinfo;
	src->step = dst->step;
	src->size = dst->size;
	src->width = dst_width;
	src->height = dst_height;
	src->data.ptr = p_dst;

	free(dst);	
	
}