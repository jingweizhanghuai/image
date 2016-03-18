#include <stdio.h>
#include "type.h"

#define SRC_PTR(p) {\
	p_src = malloc(img_height<<2);\
	p_src[0] = src->data.ptr;\
	for(i=1;i<img_height;i++)\
		p_src[i] = p_src[i-1]+src->step;\
}\

#define DST_PTR(p) {\
	p_dst = malloc(img_height<<2);\
	p_dst[0] = dst->data.ptr;\
	for(i=1;i<img_height;i++)\
		p_dst[i] = p_dst[i-1]+dst->step;\
}\

#define SRC(x,y) *(p_src[y]+(x))
#define DST(x,y) *(p_dst[y]+(x))

#define ABS(x) (((x)>0)?(x):(1-(x)))

void imgFilter_gray(ImgMat *src,ImgMat *dst,int r,int thresh)
{
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int i,j;
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);
	
	if(r>10)
		r = 10;
	else if(r<1)
		r = 1;
	
	int data[21][21];
		
	int x,y;
	int data0;
	int sum,num;
	
	for(j=0;j<r;j++)
		for(i=0;i<img_width;i++)
			DST(i,j) = SRC(i,j);	
	
	for(j=r;j<img_height-r;j++)
	{
		for(x=1;x<=r*2;x++)	
			for(y=0;y<=r*2;y++)
				data[x][y] = SRC(x-1,j-r+y);
		
		for(x=0;x<r;x++)
			DST(x,j) = data[x+1][r];
			
		for(i=r;i<img_width-r;i++)
		{
			data0 = data[r+1][r];
			
			sum = 0;
			num = 0;
			
			for(x=0;x<r*2;x++)
				for(y=0;y<=r*2;y++)
				{
					data[x][y] = data[x+1][y];
					
					if(ABS(data[x][y] - data0)<thresh)
					{
						sum = sum + data[x][y];
						num = num +1;
					}
				}			
			
			for(y=0;y<=r*2;y++)
			{
				data[x][y] = SRC(i-r+x,j-r+y);
				
				if(ABS(data[x][y] - data0)<thresh)
				{
					sum = sum + data[x][y];
					num = num +1;
				}
			}
			
			DST(i,j) = (sum+(num>>1))/num;
		}
		for(x=0;x<r;x++)
			DST(i+x,j) = data[x+r][r];
		
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
			DST(i,j) = SRC(i,j);
		
	free(p_src);
	free(p_dst);
}

#define SRC_0(x,y) *(p_src[y]+(x)*cn)
#define SRC_1(x,y) *(p_src[y]+(x)*cn+1)
#define SRC_2(x,y) *(p_src[y]+(x)*cn+2)

#define DST_0(x,y) *(p_dst[y]+(x)*cn)
#define DST_1(x,y) *(p_dst[y]+(x)*cn+1)
#define DST_2(x,y) *(p_dst[y]+(x)*cn+2)

void imgFilter(ImgMat *src,ImgMat *dst,int r,int thresh)
{	
	printf("r is %d\n",r);
	printf("thresh is %d\n",thresh);
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn==1)
	{
		imgFilter_gray(src,dst,r,thresh);
		return;
	}
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC3))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC3);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int i,j;
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);
	
	if(r>5)
		r = 5;
	else if(r<1)
		r = 1;
	
	int data_0[11][11],data_1[11][11],data_2[11][11];
		
	int x,y;
	int data0_0,data0_1,data0_2;
	int sum_0,sum_1,sum_2,num_0,num_1,num_2;
	
	for(j=0;j<r;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);	
			DST_2(i,j) = SRC_2(i,j);
		}
	
	for(j=r;j<img_height-r;j++)
	{
		for(x=1;x<=r*2;x++)	
			for(y=0;y<=r*2;y++)
			{
				data_0[x][y] = SRC_0(x-1,j-r+y);
				data_1[x][y] = SRC_1(x-1,j-r+y);
				data_2[x][y] = SRC_2(x-1,j-r+y);
			}
		
		for(x=0;x<r;x++)
		{
			DST_0(x,j) = data_0[x+1][r];
			DST_1(x,j) = data_1[x+1][r];
			DST_2(x,j) = data_2[x+1][r];
		}
			
			
		for(i=r;i<img_width-r;i++)
		{
			data0_0 = data_0[r+1][r];
			data0_1 = data_1[r+1][r];
			data0_2 = data_2[r+1][r];
			
			sum_0 = 0;
			sum_1 = 0;
			sum_2 = 0;
			num_0 = 0;
			num_1 = 0;
			num_2 = 0;
			
			for(x=0;x<r*2;x++)
				for(y=0;y<=r*2;y++)
				{
					data_0[x][y] = data_0[x+1][y];
					data_1[x][y] = data_1[x+1][y];
					data_2[x][y] = data_2[x+1][y];
					
					if(ABS(data_0[x][y] - data0_0)<thresh)
					{
						sum_0 = sum_0 + data_0[x][y];
						num_0 = num_0 +1;
					}
					if(ABS(data_1[x][y] - data0_1)<thresh)
					{
						sum_1 = sum_1 + data_1[x][y];
						num_1 = num_1 +1;
					}
					if(ABS(data_2[x][y] - data0_2)<thresh)
					{
						sum_2 = sum_2 + data_2[x][y];
						num_2 = num_2 +1;
					}
				}			
			
			for(y=0;y<=r*2;y++)
			{
				data_0[x][y] = SRC_0(i-r+x,j-r+y);
				data_1[x][y] = SRC_1(i-r+x,j-r+y);
				data_2[x][y] = SRC_2(i-r+x,j-r+y);
				
				if(ABS(data_0[x][y] - data0_0)<thresh)
				{
					sum_0 = sum_0 + data_0[x][y];
					num_0 = num_0 +1;
				}
				if(ABS(data_1[x][y] - data0_1)<thresh)
				{
					sum_1 = sum_1 + data_1[x][y];
					num_1 = num_1 +1;
				}
				if(ABS(data_2[x][y] - data0_2)<thresh)
				{
					sum_2 = sum_2 + data_2[x][y];
					num_2 = num_2 +1;
				}
			}
			
			DST_0(i,j) = (sum_0+(num_0>>1))/num_0;
			DST_1(i,j) = (sum_1+(num_1>>1))/num_1;
			DST_2(i,j) = (sum_2+(num_2>>1))/num_2;
		}
		for(x=0;x<r;x++)
		{
			DST_0(i+x,j) = data_0[x+r][r];
			DST_1(i+x,j) = data_1[x+r][r];
			DST_2(i+x,j) = data_2[x+r][r];
		}
		
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
		}
		
	free(p_src);
	free(p_dst);
}

ImgMat *imgCreateMat(int height,int width,char type);

void Filter(ImgMat *src,ImgMat *dst,int r,int thresh) 
{
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgFilter(src,dst,r,thresh);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgFilter(src,dst,r,thresh);
}
