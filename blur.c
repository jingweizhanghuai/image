#include <stdio.h>
#include "type.h"

#define SRC_PTR(p) {\
	p_src = malloc(img_height<<2);\
	p_src[0] = src->data.ptr;\
	for(i=1;i<img_height;i++)\
		p_src[i] = p_src[i-1]+src->width;\
}\

#define DST_PTR(p) {\
	p_dst = malloc(img_height<<2);\
	p_dst[0] = dst->data.ptr;\
	for(i=1;i<img_height;i++)\
		p_dst[i] = p_dst[i-1]+dst->width;\
}\

#define SRC(x,y) *(p_src[y]+(x))
#define DST(x,y) *(p_dst[y]+(x))

#define BLUR4(x,y) {\
	lx1 = (x)-3;\
	lx2 = (x)+3;\
	ly1 = (y)-3;\
	ly2 = (y)+3;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = *(p_src[ly1]+lx1);\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	lx1 = (x)-2;\
	lx2 = (x)+2;\
	ly1 = (y)-2;\
	ly2 = (y)+2;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	*(p_dst[(y)]+(x)) = (sum+4)>>3;\
}\
	
void imgBlur4(ImgMat *src,ImgMat *dst,int r)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int i,j;
	int lx1,ly1,lx2,ly2;
	int sum;
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);	
	
	int data_a[3][3];
	int data_b[2][2];
	
	int data0,data1,data2,data3;
	
	// int s;
	
	// r= 6;
	// na = 2*(r/4)+1;
	// nb = 2*((r+2)/4);
	
	BLUR4(0,0);
	BLUR4(0,2);
	DST(0,1) = (DST(0,0)+DST(0,2)+1)>>1;
	for(i=2;i<img_width;i++)
	{
		BLUR4(i,0);
		BLUR4(i,2);
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		
		DST(i-1,0) = (DST(i-2,0)+DST(i,0)+1)>>1;
		DST(i-1,2) = (DST(i-2,2)+DST(i,2)+1)>>1;		
		DST(i-1,1) = (DST(i-1,0)+DST(i-1,2)+1)>>1;
	}
	if(i!=img_width+1)
	{
		BLUR4(img_width-1,0);
		BLUR4(img_width-1,2);
		DST(img_width-1,1) = (DST(img_width-1,0)+DST(img_width-1,2)+1)>>1;
	}
		
	for(j=4;j<img_height-4;j=j+2)
	{
		data_a[1][0] = SRC(0,j-4);
		data_a[1][1] = SRC(0,j);
		data_a[1][2] = SRC(0,j+4);
		
		data_a[2][0] = SRC(4,j-4);
		data_a[2][1] = SRC(4,j);
		data_a[2][2] = SRC(4,j+4);
		
		data_b[1][0] = SRC(2,j-2);
		data_b[1][1] = SRC(2,j+2);
		
		sum = data_a[1][0]+data_a[1][1]+data_a[1][2]+data_b[1][0]+data_b[1][1]+data_a[2][1];
		
		data1 = (sum+3)/6;
		data3 = (sum+data_a[2][1]+data_a[2][2]+4)/8;
		data2 = (data1+data3+1)>>1;
		
		DST(0,j) = data1;
		DST(1,j) = data2;
		DST(2,j) = data3;
		
		DST(0,j-1) = (data1+DST(0,j-2)+1)>>1;
		DST(1,j-1) = (data1+DST(1,j-2)+1)>>1;
		DST(2,j-1) = (data1+DST(2,j-2)+1)>>1;
		
		for(i=4;i<img_width-4;i=i+4)
		{
			data_a[0][0] = data_a[1][0];
			data_a[0][1] = data_a[1][1];
			data_a[0][2] = data_a[1][2];
			
			data_a[1][0] = data_a[2][0];
			data_a[1][1] = data_a[2][1];
			data_a[1][2] = data_a[2][2];
			
			data_a[2][0] = SRC(i+4,j-4);
			data_a[2][1] = SRC(i+4,j);
			data_a[2][2] = SRC(i+4,j+4);
			
			data_b[0][0] = data_b[1][0];
			data_b[0][1] = data_b[1][1];
			
			data_b[1][0] = SRC(i+2,j-2);
			data_b[1][1] = SRC(i+2,j+2);
			
			sum = data_a[1][0]+data_a[1][1]+data_a[1][2]+data_b[1][0]+data_b[1][1]+data_a[2][1];
			
			data1 = (sum+data_a[0][1]+data_b[0][0]+data_b[0][1]+4)/9;
			data0 = (data3+data1+1)>>1;
			data3 = (sum+data_a[2][0]+data_a[2][2]+4)>>3;
			data2 = (data1+data3+1)>>1;
			
			// if(((i==404)||(i==408))&&(j==148))
			// {
				// printf("sum is %d\n",sum);
				// printf("left is %d,%d,%d\n",data_a[0][0],data_b[0][0],data_b[0][1]);
				// printf("data1 is %d\n\n",data1);
				// printf("right is %d,%d\n",data_a[2][0],data_a[2][2]);
				// printf("data3 is %d\n\n",data3);
				
			// }
						
			DST(i-1,j) = data0;
			DST(i,j) = data1;
			DST(i+1,j) = data2;
			DST(i+2,j) = data3;
			
			DST(i-1,j-1) = (data0+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data1+DST(i,j-2)+1)>>1;
			DST(i+1,j-1) = (data2+DST(i+1,j-2)+1)>>1;
			DST(i+2,j-1) = (data3+DST(i+2,j-2)+1)>>1;			
		}
		
		for(i=i-2;i<img_width;i=i+2)
		{
			BLUR4(i,j);
			
			data3 = DST(i,j);
			data2 = (data1+data3+1)>>1;
			
			DST(i-1,j) = data2;
			
			DST(i-1,j-1) = (data2+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data3+DST(i,j-2)+1)>>1;
		}
		if(i!=img_width+1)
		{
			BLUR4(img_width-1,j);
			DST(img_width-1,j-1) = (DST(img_width-1,j)+DST(img_width-1,j-2)+1)>>1;
		}
	}
	for(;j<img_height;j=j+2)
	{
		BLUR4(0,j);
		DST(0,j-1) = (DST(0,j)+DST(0,j-2)+1)>>1;
		for(i=2;i<img_width;i=i+2)
		{
			BLUR4(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;		
		
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
			DST(i-1,j-1) = (DST(i-1,j)+DST(i-1,j-2)+1)>>1;			
		}
		if(i!=img_width+1)
		{
			i = img_width-1;
			BLUR4(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;
		}
	}
	if(j!=img_height+1)
	{
		j=img_height-1;
		
		BLUR4(0,j);
		for(i=2;i<img_width;i=i+2)
		{
			BLUR4(i,j);
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
		}
		BLUR4(img_width-1,j);
	}
	
	free(p_src);
	free(p_dst);
}
	
#define BLUR5(x,y) {\
	lx1 = (x)-4;\
	lx2 = (x)+4;\
	ly1 = (y)-4;\
	ly2 = (y)+4;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = *(p_src[ly1]+lx1);\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	lx1 = (x)-3;\
	lx2 = (x)+3;\
	ly1 = (y)-3;\
	ly2 = (y)+3;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	*(p_dst[(y)]+(x)) = (sum+4)>>3;\
}\
	
void imgBlur5(ImgMat *src,ImgMat *dst,int r)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int i,j;
	int lx1,ly1,lx2,ly2;
	int sum;
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);	
	
	int sum_a[3];
	int sum_b[3];
	
	int data0,data1,data2,data3;
	
	BLUR5(0,0);
	BLUR5(0,2);
	DST(0,1) = (DST(0,0)+DST(0,2)+1)>>1;
	for(i=2;i<img_width;i=i+2)
	{
		BLUR5(i,0);
		BLUR5(i,2);
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		
		DST(i-1,0) = (DST(i-2,0)+DST(i,0)+1)>>1;
		DST(i-1,2) = (DST(i-2,2)+DST(i,2)+1)>>1;		
		DST(i-1,1) = (DST(i-1,0)+DST(i-1,2)+1)>>1;
	}
	if(i!=img_width+1)
	{
		BLUR5(img_width-1,0);
		BLUR5(img_width-1,2);
		DST(img_width-1,1) = (DST(img_width-1,0)+DST(img_width-1,2)+1)>>1;
	}
		
	for(j=4;j<img_height-4;j=j+2)
	{
		sum_a[1] = SRC(0,j-4)+SRC(0,j)+SRC(0,j+4);
		sum_a[2] = SRC(4,j-4)+SRC(4,j)+SRC(4,j+4);	
		
		sum_b[1] = SRC(2,j-2)+SRC(2,j+2);		
		sum_b[2] = SRC(6,j-2)+SRC(6,j+2);
		
		sum = sum_a[1]+sum_a[2]+sum_b[1];		
		data1 = (sum+4)>>3;
		DST(0,j) = data1;
		DST(0,j-1) = (data1+DST(0,j-2)+1)>>1;
		
		sum = sum+sum_b[2];
		data3 = (sum+5)/10;
		DST(2,j) = data3;
		DST(2,j-1) = (data3+DST(2,j-2)+1)>>1;		
		
		data2 = (data1+data3+1)>>1;		
		DST(1,j) = data2;
		DST(1,j-1) = (data2+DST(1,j-2)+1)>>1;
		
		for(i=4;i<img_width-6;i=i+4)
		{
			sum_a[0] = sum_a[1];
			sum_a[1] = sum_a[2];
			sum_a[2] = SRC(i+4,j-4)+SRC(i+4,j)+SRC(i+4,j+4);
			
			sum_b[0] = sum_b[1];
			sum_b[1] = sum_b[2];
			sum_b[2] = SRC(i+6,j-2)+SRC(i+6,j+2);
			
			sum = sum+sum_a[2];
			data1 = (sum+6)/13;
			DST(i,j) = data1;
			DST(i,j-1) = (data1+DST(i,j-2)+1)>>1;
			
			data0 = (data3+data1+1)>>1;
			DST(i-1,j) = data0;
			DST(i-1,j-1) = (data0+DST(i-1,j-2)+1)>>1;
			
			sum = sum-sum_a[0]+sum_b[2];
			data3 = (sum+6)/12;
			DST(i+2,j) = data3;
			DST(i+2,j-1) = (data3+DST(i+2,j-2)+1)>>1;
		
			data2 = (data1+data3+1)>>1;
			DST(i+1,j) = data2;
			DST(i+1,j-1) = (data2+DST(i+1,j-2)+1)>>1;	
			
			sum = sum-sum_b[0];
		}
		
		for(i=i-2;i<img_width;i=i+2)
		{
			BLUR5(i,j);
			
			data3 = DST(i,j);
			data2 = (data1+data3+1)>>1;
			
			DST(i-1,j) = data2;
			
			DST(i-1,j-1) = (data2+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data3+DST(i,j-2)+1)>>1;
		}
		if(i!=img_width+1)
		{
			BLUR5(img_width-1,j);
			DST(img_width-1,j-1) = (DST(img_width-1,j)+DST(img_width-1,j-2)+1)>>1;
		}
	}
	for(;j<img_height;j=j+2)
	{
		BLUR5(0,j);
		DST(0,j-1) = (DST(0,j)+DST(0,j-2)+1)>>1;
		for(i=2;i<img_width;i=i+2)
		{
			BLUR5(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;		
		
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
			DST(i-1,j-1) = (DST(i-1,j)+DST(i-1,j-2)+1)>>1;			
		}
		if(i!=img_width+1)
		{
			i = img_width-1;
			BLUR5(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;
		}
	}
	if(j!=img_height+1)
	{
		j=img_height-1;
		
		BLUR5(0,j);
		for(i=2;i<img_width;i=i+2)
		{
			BLUR5(i,j);
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
		}
		BLUR5(img_width-1,j);
	}
	
	free(p_src);
	free(p_dst);
	dst->memory_valid[0] = 1;
	dst->memory_valid[1] = 0;
}
		



#define BLUR6(x,y) {\
	lx1 = (x)-6;\
	lx2 = (x)+6;\
	ly1 = (y)-6;\
	ly2 = (y)+6;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = *(p_src[ly1]+lx1);\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	lx1 = (x)-4;\
	lx2 = (x)+4;\
	ly1 = (y)-4;\
	ly2 = (y)+4;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	*(p_dst[(y)]+(x)) = (sum+4)>>3;\
}\
	
void imgBlur6(ImgMat *src,ImgMat *dst)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int i,j;
	int lx1,ly1,lx2,ly2;
	int sum;
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);	
	
	int sum_a[4];
	int sum_b[4];
	
	int data0,data1,data2,data3;
	
	BLUR6(0,0);
	BLUR6(0,2);
	BLUR6(0,4);
	DST(0,1) = (DST(0,0)+DST(0,2)+1)>>1;
	DST(0,3) = (DST(0,2)+DST(0,4)+1)>>1;
	for(i=2;i<img_width;i=i+2)
	{
		BLUR6(i,0);
		BLUR6(i,2);
		BLUR6(i,4);
		
		DST(i-1,0) = (DST(i-2,0)+DST(i,0)+1)>>1;
		DST(i-1,2) = (DST(i-2,2)+DST(i,2)+1)>>1;
		DST(i-1,4) = (DST(i-2,4)+DST(i,4)+1)>>1;
		
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		DST(i,3) = (DST(i,2)+DST(i,4)+1)>>1;
		
		DST(i-1,1) = (DST(i-1,0)+DST(i-1,2)+1)>>1;
		DST(i-1,3) = (DST(i-1,2)+DST(i-1,4)+1)>>1;
	}
	if(i!=img_width+1)
	{
		i = img_width-1;
		
		BLUR6(i,0);
		BLUR6(i,2);
		BLUR6(i,4);
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		DST(i,3) = (DST(i,2)+DST(i,4)+1)>>1;
	}
		
	for(j=6;j<img_height-6;j=j+2)
	{
		sum_a[1] = SRC(2,j-4)+SRC(2,j)+SRC(2,j+4);
		sum_a[2] = SRC(6,j-4)+SRC(6,j)+SRC(6,j+4);
		sum_a[3] = SRC(10,j-4)+SRC(10,j)+SRC(10,j+4);
		
		sum_b[1] = SRC(0,j-6)+SRC(0,j-2)+SRC(0,j+2)+SRC(0,j+6);
		sum_b[2] = SRC(4,j-6)+SRC(4,j-2)+SRC(4,j+2)+SRC(4,j+6);
		sum_b[3] = SRC(8,j-6)+SRC(8,j-2)+SRC(8,j+2)+SRC(8,j+6);
		
		sum = sum_a[1]+sum_a[2]+sum_b[1]+sum_b[2];		
		data3 = (sum+7)/14;
		DST(0,j) = data3;
		DST(0,j-1) = (data3+DST(0,j-2)+1)>>1;
		
		sum = sum+sum_b[3];
		data1 = (sum+9)/18;
		DST(2,j) = data1;
		DST(2,j-1) = (data1+DST(2,j-2)+1)>>1;
		
		data0 = (data1+data3+1)>>1;
		DST(1,j) = data0;
		DST(1,j-1) = (data0+DST(1,j-2)+1)>>1;
		
		sum = sum+sum_a[3];
		data3 = (sum+10)/21;
		DST(4,j) = data3;
		DST(4,j-1) = (data3+DST(4,j-2)+1)>>1;
		
		data2 = (data1+data3+1)>>1;
		DST(3,j) = data2;
		DST(3,j-1) = (data2+DST(3,j-2)+1)>>1;
		
		for(i=6;i<img_width-8;i=i+4)
		{
			sum_a[0] = sum_a[1];
			sum_a[1] = sum_a[2];
			sum_a[2] = sum_a[3];
			sum_a[3] = SRC(i+8,j-4)+SRC(i+8,j)+SRC(i+8,j+4);
			
			sum_b[0] = sum_b[1];
			sum_b[1] = sum_b[2];
			sum_b[2] = sum_b[3];
			sum_b[3] = SRC(i+6,j-6)+SRC(i+6,j-2)+SRC(i+6,j+2)+SRC(i+6,j+6);			
			
			sum = sum+sum_b[3];				 
			data1 = (sum+12)/25;
			data0 = (data3+data1+1)>>1;
			
			sum = sum-sum_b[0]+sum_a[3];				 
			data3 = (sum+12)/24;
			data2 = (data1+data3+1)>>1;
			
			sum = sum-sum_a[0];
			
			// if((i==14)&&(j==6))
				// printf("data0~3 is %d,%d,%d,%d\n",data0,data1,data2,data3);
			
			DST(i-1,j) = data0;
			DST(i,j) = data1;
			DST(i+1,j) = data2;
			DST(i+2,j) = data3;
			
			DST(i-1,j-1) = (data0+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data1+DST(i,j-2)+1)>>1;
			DST(i+1,j-1) = (data2+DST(i+1,j-2)+1)>>1;
			DST(i+2,j-1) = (data3+DST(i+2,j-2)+1)>>1;			
		}
		
		for(i=i-2;i<img_width;i=i+2)
		{
			BLUR6(i,j);
			
			data3 = DST(i,j);
			data2 = (data1+data3+1)>>1;
			
			DST(i-1,j) = data2;
			
			DST(i-1,j-1) = (data2+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data3+DST(i,j-2)+1)>>1;
		}
		if(i!=img_width+1)
		{
			BLUR6(img_width-1,j);
			DST(img_width-1,j-1) = (DST(img_width-1,j)+DST(img_width-1,j-2)+1)>>1;
		}
	}
	for(;j<img_height;j=j+2)
	{
		BLUR6(0,j);
		DST(0,j-1) = (DST(0,j)+DST(0,j-2)+1)>>1;
		for(i=2;i<img_width;i=i+2)
		{
			BLUR6(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;		
		
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
			DST(i-1,j-1) = (DST(i-1,j)+DST(i-1,j-2)+1)>>1;			
		}
		if(i!=img_width+1)
		{
			i = img_width-1;
			BLUR6(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;
		}
	}
	if(j!=img_height+1)
	{
		j=img_height-1;
		
		BLUR6(0,j);
		for(i=2;i<img_width;i=i+2)
		{
			BLUR6(i,j);
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
		}
		BLUR6(img_width-1,j);
	}
	
	free(p_src);
	free(p_dst);
	dst->memory_valid[0] = 1;
	dst->memory_valid[1] = 0;
}


#define BLUR7(x,y) {\
	lx1 = (x)-8;\
	lx2 = (x)+8;\
	ly1 = (y)-8;\
	ly2 = (y)+8;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = *(p_src[ly1]+lx1);\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	lx1 = (x)-5;\
	lx2 = (x)+5;\
	ly1 = (y)-5;\
	ly2 = (y)+5;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	*(p_dst[(y)]+(x)) = (sum+4)>>3;\
}\
	
void imgBlur7(ImgMat *src,ImgMat *dst)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int i,j;
	int lx1,ly1,lx2,ly2;
	int sum;
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);	
	
	int sum_a[5];
	int sum_b[5];
	
	int data0,data1,data2,data3;
	
	BLUR7(0,0);
	BLUR7(0,2);
	BLUR7(0,4);
	BLUR7(0,6);
	DST(0,1) = (DST(0,0)+DST(0,2)+1)>>1;
	DST(0,3) = (DST(0,2)+DST(0,4)+1)>>1;
	DST(0,5) = (DST(0,4)+DST(0,6)+1)>>1;
	for(i=2;i<img_width;i=i+2)
	{
		BLUR7(i,0);
		BLUR7(i,2);
		BLUR7(i,4);
		BLUR7(i,6);
		
		DST(i-1,0) = (DST(i-2,0)+DST(i,0)+1)>>1;
		DST(i-1,2) = (DST(i-2,2)+DST(i,2)+1)>>1;
		DST(i-1,4) = (DST(i-2,4)+DST(i,4)+1)>>1;
		DST(i-1,6) = (DST(i-2,6)+DST(i,6)+1)>>1;
		
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		DST(i,3) = (DST(i,2)+DST(i,4)+1)>>1;
		DST(i,5) = (DST(i,4)+DST(i,6)+1)>>1;
		
		DST(i-1,1) = (DST(i-1,0)+DST(i-1,2)+1)>>1;
		DST(i-1,3) = (DST(i-1,2)+DST(i-1,4)+1)>>1;
		DST(i-1,5) = (DST(i-1,4)+DST(i-1,6)+1)>>1;
	}
	if(i!=img_width+1)
	{
		i = img_width-1;
		
		BLUR7(i,0);
		BLUR7(i,2);
		BLUR7(i,4);
		BLUR7(i,6);
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		DST(i,3) = (DST(i,2)+DST(i,4)+1)>>1;
		DST(i,5) = (DST(i,4)+DST(i,6)+1)>>1;
	}
		
	for(j=8;j<img_height-8;j=j+2)
	{
		sum_a[1] = SRC(0,j-8)+SRC(0,j-4)+SRC(0,j)+SRC(0,j+4)+SRC(0,j+8);
		sum_a[2] = SRC(4,j-8)+SRC(4,j-4)+SRC(4,j)+SRC(4,j+4)+SRC(4,j+8);
		sum_a[3] = SRC(8,j-8)+SRC(8,j-4)+SRC(8,j)+SRC(8,j+4)+SRC(8,j+8);
		sum_a[4] = SRC(12,j-8)+SRC(12,j-4)+SRC(12,j)+SRC(12,j+4)+SRC(12,j+8);
		
		sum_b[1] = SRC(2,j-6)+SRC(2,j-2)+SRC(2,j+2)+SRC(2,j+6);
		sum_b[2] = SRC(6,j-6)+SRC(6,j-2)+SRC(6,j+2)+SRC(6,j+6);
		sum_b[3] = SRC(10,j-6)+SRC(10,j-2)+SRC(10,j+2)+SRC(10,j+6);		
		sum_b[4] = SRC(14,j-6)+SRC(14,j-2)+SRC(14,j+2)+SRC(14,j+6);
		
		sum = sum_a[1]+sum_a[2]+sum_a[3]+sum_b[1]+sum_b[2];
		
		data1 = (sum+11)/23;
		DST(0,j) = data1;
		DST(0,j-1) = (data1+DST(0,j-2)+1)>>1;
		
		sum = sum+sum_b[3];
		data3 = (sum+13)/27;
		DST(2,j) = data3;
		DST(2,j-1) = (data3+DST(2,j-2)+1)>>1;
		
		data2 = (data1+data3+1)>>1;
		DST(1,j) = data2;
		DST(1,j-1) = (data2+DST(1,j-2)+1)>>1;
		
		sum = sum+sum_a[4];
		data1 = (sum+16)>>5;
		DST(4,j) = data1;
		DST(4,j-1) = (data1+DST(4,j-2)+1)>>1;
		
		data0 = (data1+data3+1)>>1;
		DST(3,j) = data0;
		DST(3,j-1) = (data0+DST(3,j-2)+1)>>1;
		
		sum = sum+sum_b[4];
		data3 = (sum+18)/36;
		DST(6,j) = data3;
		DST(6,j-1) = (data3+DST(2,j-2)+1)>>1;
		
		data2 = (data1+data3+1)>>1;
		DST(5,j) = data2;
		DST(5,j-1) = (data2+DST(1,j-2)+1)>>1;
		
		for(i=8;i<img_width-10;i=i+4)
		{
			sum_a[0] = sum_a[1];
			sum_a[1] = sum_a[2];
			sum_a[2] = sum_a[3];
			sum_a[3] = sum_a[4];
			sum_a[4] = SRC(i+8,j-8)+SRC(i+8,j-4)+SRC(i+8,j)+SRC(i+8,j+4)+SRC(i+8,j+8);
			
			sum_b[0] = sum_b[1];
			sum_b[1] = sum_b[2];
			sum_b[2] = sum_b[3];
			sum_b[3] = sum_b[4];
			sum_b[4] = SRC(i+10,j-6)+SRC(i+10,j-2)+SRC(i+10,j+2)+SRC(i+10,j+6);			
			
			sum = sum+sum_a[4];				 
			data1 = (sum+20)/41;
			data0 = (data3+data1+1)>>1;
			
			sum = sum-sum_a[0]+sum_b[4];
				 
			data3 = (sum+20)/40;
			data2 = (data1+data3+1)>>1;
			
			sum = sum-sum_b[0];
			
			// if((i==256)&&(j==520))
				// printf("data0~3 is %d,%d,%d,%d\n",data0,data1,data2,data3);
			
			DST(i-1,j) = data0;
			DST(i,j) = data1;
			DST(i+1,j) = data2;
			DST(i+2,j) = data3;
			
			DST(i-1,j-1) = (data0+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data1+DST(i,j-2)+1)>>1;
			DST(i+1,j-1) = (data2+DST(i+1,j-2)+1)>>1;
			DST(i+2,j-1) = (data3+DST(i+2,j-2)+1)>>1;			
		}
		
		for(i=i-2;i<img_width;i=i+2)
		{
			BLUR7(i,j);
			
			data3 = DST(i,j);
			data2 = (data1+data3+1)>>1;
			
			DST(i-1,j) = data2;
			
			DST(i-1,j-1) = (data2+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data3+DST(i,j-2)+1)>>1;
		}
		if(i!=img_width+1)
		{
			BLUR7(img_width-1,j);
			DST(img_width-1,j-1) = (DST(img_width-1,j)+DST(img_width-1,j-2)+1)>>1;
		}
	}
	for(;j<img_height;j=j+2)
	{
		BLUR7(0,j);
		DST(0,j-1) = (DST(0,j)+DST(0,j-2)+1)>>1;
		for(i=2;i<img_width;i=i+2)
		{
			BLUR7(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;		
		
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
			DST(i-1,j-1) = (DST(i-1,j)+DST(i-1,j-2)+1)>>1;			
		}
		if(i!=img_width+1)
		{
			i = img_width-1;
			BLUR7(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;
		}
	}
	if(j!=img_height+1)
	{
		j=img_height-1;
		
		BLUR7(0,j);
		for(i=2;i<img_width;i=i+2)
		{
			BLUR7(i,j);
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
		}
		BLUR7(img_width-1,j);
	}
	
	free(p_src);
	free(p_dst);
}
	

#define BLUR8(x,y) {\
	lx1 = (x)-9;\
	lx2 = (x)+9;\
	ly1 = (y)-9;\
	ly2 = (y)+9;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = *(p_src[ly1]+lx1);\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	lx1 = (x)-8;\
	lx2 = (x)+8;\
	ly1 = (y)-8;\
	ly2 = (y)+8;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	lx1 = (x)-5;\
	lx2 = (x)+5;\
	ly1 = (y)-5;\
	ly2 = (y)+5;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[ly1]+lx1));\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	lx1 = (x)-3;\
	lx2 = (x)+3;\
	ly1 = (y)-3;\
	ly2 = (y)+3;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	*(p_dst[(y)]+(x)) = (sum+8)>>4;\
}\
	
void imgBlur8(ImgMat *src,ImgMat *dst)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int i,j;
	int lx1,ly1,lx2,ly2;
	int sum;
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);
	
	int sum_a[6];
	int sum_b[6];
	
	int data0,data1,data2,data3;
	
	BLUR8(0,0);
	BLUR8(0,2);
	BLUR8(0,4);
	BLUR8(0,6);
	BLUR8(0,8);
	DST(0,1) = (DST(0,0)+DST(0,2)+1)>>1;
	DST(0,3) = (DST(0,2)+DST(0,4)+1)>>1;
	DST(0,5) = (DST(0,4)+DST(0,6)+1)>>1;
	DST(0,7) = (DST(0,6)+DST(0,8)+1)>>1;
	for(i=2;i<img_width;i=i+2)
	{
		BLUR8(i,0);
		BLUR8(i,2);
		BLUR8(i,4);
		BLUR8(i,6);
		BLUR8(i,8);
		
		DST(i-1,0) = (DST(i-2,0)+DST(i,0)+1)>>1;
		DST(i-1,2) = (DST(i-2,2)+DST(i,2)+1)>>1;
		DST(i-1,4) = (DST(i-2,4)+DST(i,4)+1)>>1;
		DST(i-1,6) = (DST(i-2,6)+DST(i,6)+1)>>1;
		DST(i-1,8) = (DST(i-2,8)+DST(i,8)+1)>>1;
		
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		DST(i,3) = (DST(i,2)+DST(i,4)+1)>>1;
		DST(i,5) = (DST(i,4)+DST(i,6)+1)>>1;
		DST(i,7) = (DST(i,6)+DST(i,8)+1)>>1;
		
		DST(i-1,1) = (DST(i-1,0)+DST(i-1,2)+1)>>1;
		DST(i-1,3) = (DST(i-1,2)+DST(i-1,4)+1)>>1;
		DST(i-1,5) = (DST(i-1,4)+DST(i-1,6)+1)>>1;
		DST(i-1,7) = (DST(i-1,6)+DST(i-1,8)+1)>>1;
	}
	if(i!=img_width+1)
	{
		i = img_width-1;
		
		BLUR8(i,0);
		BLUR8(i,2);
		BLUR8(i,4);
		BLUR8(i,6);
		BLUR8(i,8);
		DST(i,1) = (DST(i,0)+DST(i,2)+1)>>1;
		DST(i,3) = (DST(i,2)+DST(i,4)+1)>>1;
		DST(i,5) = (DST(i,4)+DST(i,6)+1)>>1;
		DST(i,7) = (DST(i,6)+DST(i,8)+1)>>1;
	}
		
	for(j=10;j<img_height-10;j=j+2)
	{
		sum_a[1] = SRC(2,j-8)+SRC(2,j-4)+SRC(2,j)+SRC(2,j+4)+SRC(2,j+8);
		sum_a[2] = SRC(6,j-8)+SRC(6,j-4)+SRC(6,j)+SRC(6,j+4)+SRC(6,j+8);
		sum_a[3] = SRC(10,j-8)+SRC(10,j-4)+SRC(10,j)+SRC(10,j+4)+SRC(10,j+8);
		sum_a[4] = SRC(14,j-8)+SRC(14,j-4)+SRC(14,j)+SRC(14,j+4)+SRC(14,j+8);		
		sum_a[5] = SRC(18,j-8)+SRC(18,j-4)+SRC(18,j)+SRC(18,j+4)+SRC(18,j+8);
		
		sum_b[1] = SRC(0,j-10)+SRC(0,j-6)+SRC(0,j-2)+SRC(0,j+2)+SRC(0,j+6)+SRC(0,j+10);
		sum_b[2] = SRC(4,j-10)+SRC(4,j-6)+SRC(4,j-2)+SRC(4,j+2)+SRC(4,j+6)+SRC(4,j+10);
		sum_b[3] = SRC(8,j-10)+SRC(8,j-6)+SRC(8,j-2)+SRC(8,j+2)+SRC(8,j+6)+SRC(8,j+10);
		sum_b[4] = SRC(12,j-10)+SRC(12,j-6)+SRC(12,j-2)+SRC(12,j+2)+SRC(12,j+6)+SRC(12,j+10);		
		sum_b[5] = SRC(16,j-10)+SRC(16,j-6)+SRC(16,j-2)+SRC(16,j+2)+SRC(16,j+6)+SRC(16,j+10);
			 
		sum = sum_a[1]+sum_a[2]+sum_a[3]+sum_b[1]+sum_b[2]+sum_b[3];
		
		data3 = (sum+16)/33;
		DST(0,j) = data3;
		DST(0,j-1) = (data3+DST(0,j-2)+1)>>1;
		
		sum = sum+sum_b[4];
		data1 = (sum+19)/39;
		DST(2,j) = data1;
		DST(2,j-1) = (data1+DST(2,j-2)+1)>>1;
		
		data0 = (data1+data3+1)>>1;
		DST(1,j) = data0;
		DST(1,j-1) = (data0+DST(1,j-2)+1)>>1;
		
		sum = sum+sum_a[4];
		data3 = (sum+22)/44;
		DST(4,j) = data3;
		DST(4,j-1) = (data3+DST(4,j-2)+1)>>1;
		
		data2 = (data1+data3+1)>>1;
		DST(3,j) = data2;
		DST(3,j-1) = (data2+DST(3,j-2)+1)>>1;
		
		sum = sum+sum_b[5];
		data1 = (sum+25)/50;
		DST(6,j) = data1;
		DST(6,j-1) = (data1+DST(2,j-2)+1)>>1;
		
		data0 = (data1+data3+1)>>1;
		DST(5,j) = data0;
		DST(5,j-1) = (data0+DST(5,j-2)+1)>>1;
		
		sum = sum+sum_a[5];
		data3 = (sum+27)/55;
		DST(8,j) = data3;
		DST(8,j-1) = (data3+DST(4,j-2)+1)>>1;
		
		data2 = (data1+data3+1)>>1;
		DST(7,j) = data2;
		DST(7,j-1) = (data2+DST(3,j-2)+1)>>1;
		
		
		for(i=10;i<img_width-12;i=i+4)
		{
			sum_a[0] = sum_a[1];
			sum_a[1] = sum_a[2];
			sum_a[2] = sum_a[3];
			sum_a[3] = sum_a[4];
			sum_a[4] = sum_a[5];
			sum_a[5] = SRC(i+12,j-8)+SRC(i+12,j-4)+SRC(i+12,j)+SRC(i+12,j+4)+SRC(i+12,j+8);
			
			sum_b[0] = sum_b[1];
			sum_b[1] = sum_b[2];
			sum_b[2] = sum_b[3];
			sum_b[3] = sum_b[4];
			sum_b[4] = sum_b[5];
			sum_b[5] = SRC(i+10,j-10)+SRC(i+10,j-6)+SRC(i+10,j-2)+SRC(i+10,j+2)+SRC(i+10,j+6)+SRC(i+10,j+10);			
			
			sum = sum+sum_b[5];
				 
			data1 = (sum+30)/61;
			data0 = (data3+data1+1)>>1;
			
			sum = sum-sum_b[0]+sum_a[5];
				 
			data3 = (sum+30)/60;
			data2 = (data1+data3+1)>>1;
			
			sum = sum-sum_a[0];
			
			DST(i-1,j) = data0;
			DST(i,j) = data1;
			DST(i+1,j) = data2;
			DST(i+2,j) = data3;
			
			DST(i-1,j-1) = (data0+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data1+DST(i,j-2)+1)>>1;
			DST(i+1,j-1) = (data2+DST(i+1,j-2)+1)>>1;
			DST(i+2,j-1) = (data3+DST(i+2,j-2)+1)>>1;			
		}
		
		for(i=i-2;i<img_width;i=i+2)
		{
			BLUR8(i,j);
			
			data3 = DST(i,j);
			data2 = (data1+data3+1)>>1;
			
			DST(i-1,j) = data2;
			
			DST(i-1,j-1) = (data2+DST(i-1,j-2)+1)>>1;
			DST(i,j-1) = (data3+DST(i,j-2)+1)>>1;
		}
		if(i!=img_width+1)
		{
			BLUR8(img_width-1,j);
			DST(img_width-1,j-1) = (DST(img_width-1,j)+DST(img_width-1,j-2)+1)>>1;
		}
	}
	for(;j<img_height;j=j+2)
	{
		BLUR8(0,j);
		DST(0,j-1) = (DST(0,j)+DST(0,j-2)+1)>>1;
		for(i=2;i<img_width;i=i+2)
		{
			BLUR8(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;		
		
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
			DST(i-1,j-1) = (DST(i-1,j)+DST(i-1,j-2)+1)>>1;			
		}
		if(i!=img_width+1)
		{
			i = img_width-1;
			BLUR8(i,j);
			DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;
		}
	}
	if(j!=img_height+1)
	{
		j=img_height-1;
		
		BLUR8(0,j);
		for(i=2;i<img_width;i=i+2)
		{
			BLUR8(i,j);
			DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
		}
		BLUR8(img_width-1,j);
	}
	
	free(p_src);
	free(p_dst);
}

#define BLURn(n,x,y) {\
	s=n*7/8;\
	\
	lx1 = (x)-s;\
	lx2 = (x)+s;\
	ly1 = (y)-s;\
	ly2 = (y)+s;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = *(p_src[ly1]+lx1);\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	s=n*6/8;\
	\
	lx1 = (x)-8;\
	lx2 = (x)+8;\
	ly1 = (y)-8;\
	ly2 = (y)+8;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	s=n*4/8;\
	\
	lx1 = (x)-5;\
	lx2 = (x)+5;\
	ly1 = (y)-5;\
	ly2 = (y)+5;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	s=n*3/8;\
	\
	sum = sum+(*(p_src[ly1]+lx1));\
	sum = sum+(*(p_src[ly1]+lx2));\
	sum = sum+(*(p_src[ly2]+lx1));\
	sum = sum+(*(p_src[ly2]+lx2));\
	\
	lx1 = (x)-3;\
	lx2 = (x)+3;\
	ly1 = (y)-3;\
	ly2 = (y)+3;\
	\
	if(lx1<0)\
		lx1 = 0-lx1;\
	else if(lx1>img_width-1)\
		lx1 = (img_width<<1)-2-lx1;\
	\
	if(lx2<0)\
		lx2 = 0-lx2;\
	else if(lx2>img_width-1)\
		lx2 = (img_width<<1)-2-lx2;\
	\
	if(ly1<0)\
		ly1 = 0-ly1;\
	else if(ly1>img_height-1)\
		ly1 = (img_height<<1)-2-ly1;\
	\
	if(ly2<0)\
		ly2 = 0-ly2;\
	else if(ly2>img_height-1)\
		ly2 = (img_height<<1)-2-ly2;\
	\
	sum = sum+(*(p_src[(y)]+lx1));\
	sum = sum+(*(p_src[(y)]+lx2));\
	sum = sum+(*(p_src[ly2]+(x)));\
	sum = sum+(*(p_src[ly2]+(x)));\
	\
	*(p_dst[(y)]+(x)) = (sum+8)>>4;\
}\
	
// void imgBlur_n(ImgMat *src,ImgMat *dst,int n)
// {
	// int cn;
	// cn = ((src->type&0xF8)>>3)+1;
	
	// if(cn!=1)
		// return;
	
	// if(n>20)
		// n=20;
	
	// n = (n-3)<<1;
	// s = n-2;
	
	// int img_width;
	// img_width = src->width;
	
	// int img_height;
	// img_height = src->height;
	
	// if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		// imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	// if(dst->data.ptr == NULL)
		// imgCreateMatData(dst);
	
	// int i,j;
	// int lx1,ly1,lx2,ly2;
	// int sum;
	
	// unsigned char **p_src;
	// SRC_PTR(p_src);
	
	// unsigned char **p_dst;
	// DST_PTR(p_dst);
	
	// int sum_a[20];
	// int sum_b[20];
	
	// int data0,data1,data2,data3;
	
	// BLURn(n,0,0);
	// for(k=2;k<=n;k=k+2)
	// {
		// BLURn(n,0,k);
		// DST(0,k-1) = (DST(0,k-2)+DST(0,k)+1)>>1;
	// }
	// for(i=2;i<img_width;i=i+2)
	// {
		// BLURn(n,i,0);
		// DST(i-1,0) = (DST(i-2,0)+DST(i,0)+1)>>1;
		// for(k=2;k<n;k=k+2)
		// {
			// BLURn(n,i,k);
			// DST(i-1,k) = (DST(i-2,k)+DST(i,k)+1)>>1;
			// DST(i,k-1) = (DST(i,k-2)+DST(i,k)+1)>>1;
			// DST(i-1,k-1) = (DST(i-1,k-2)+DST(i-1,k)+1)>>1;
		// }
	// }
	// if(i!=img_width+1)
	// {
		// i = img_width-1;
		
		// BLURn(n,i,0);
		// for(k=2;k<n;k=k+2)
		// {
			// BLURn(n,i,k);
			// DST(i,k-1) = (DST(i,k-2)+DST(i,k)+1)>>1;
		// }
	// }
		
	// for(j=10;j<img_height-10;j=j+2)
	// {
		// for(k=1;k<s;k++)
		// {
			// lx = k*4-2;
			
			// sum_a[k] = SRC(lx,j);
			// for(ly=0;ly<=n;ly=ly+4)
				// sum_a[k] = sum_a[k]+SRC(lx,j+ly)+SRC(lx,j-ly);
			
			// sum_b[k] = 0;
			// for(ly=0;ly<=n;ly=ly+4)
				// sum_b[k] = sum_b[k]+SRC(lx,j+ly)+SRC(lx,j-ly);
		// }
			 
		// sum = sum_a[1]+sum_a[2]+sum_a[3]+sum_b[1]+sum_b[2]+sum_b[3];
		
		// data3 = (sum+16)/33;
		// DST(0,j) = data3;
		// DST(0,j-1) = (data3+DST(0,j-2)+1)>>1;
		
		// sum = sum+sum_b[4];
		// data1 = (sum+19)/39;
		// DST(2,j) = data1;
		// DST(2,j-1) = (data1+DST(2,j-2)+1)>>1;
		
		// data0 = (data1+data3+1)>>1;
		// DST(1,j) = data0;
		// DST(1,j-1) = (data0+DST(1,j-2)+1)>>1;
		
		// sum = sum+sum_a[4];
		// data3 = (sum+22)/44;
		// DST(4,j) = data3;
		// DST(4,j-1) = (data3+DST(4,j-2)+1)>>1;
		
		// data2 = (data1+data3+1)>>1;
		// DST(3,j) = data2;
		// DST(3,j-1) = (data2+DST(3,j-2)+1)>>1;
		
		// sum = sum+sum_b[5];
		// data1 = (sum+25)/50;
		// DST(6,j) = data1;
		// DST(6,j-1) = (data1+DST(2,j-2)+1)>>1;
		
		// data0 = (data1+data3+1)>>1;
		// DST(5,j) = data0;
		// DST(5,j-1) = (data0+DST(5,j-2)+1)>>1;
		
		// sum = sum+sum_a[5];
		// data3 = (sum+27)/55;
		// DST(8,j) = data3;
		// DST(8,j-1) = (data3+DST(4,j-2)+1)>>1;
		
		// data2 = (data1+data3+1)>>1;
		// DST(7,j) = data2;
		// DST(7,j-1) = (data2+DST(3,j-2)+1)>>1;
		
		
		// for(i=10;i<img_width-12;i=i+4)
		// {
			// sum_a[0] = sum_a[1];
			// sum_a[1] = sum_a[2];
			// sum_a[2] = sum_a[3];
			// sum_a[3] = sum_a[4];
			// sum_a[4] = sum_a[5];
			// sum_a[5] = SRC(i+12,j-8)+SRC(i+12,j-4)+SRC(i+12,j)+SRC(i+12,j+4)+SRC(i+12,j+8);
			
			// sum_b[0] = sum_b[1];
			// sum_b[1] = sum_b[2];
			// sum_b[2] = sum_b[3];
			// sum_b[3] = sum_b[4];
			// sum_b[4] = sum_b[5];
			// sum_b[5] = SRC(i+10,j-10)+SRC(i+10,j-6)+SRC(i+10,j-2)+SRC(i+10,j+2)+SRC(i+10,j+6)+SRC(i+10,j+10);			
			
			// sum = sum+sum_b[5];
				 
			// data1 = (sum+30)/61;
			// data0 = (data3+data1+1)>>1;
			
			// sum = sum-sum_b[0]+sum_a[5];
				 
			// data3 = (sum+30)/60;
			// data2 = (data1+data3+1)>>1;
			
			// sum = sum-sum_a[0];
			
			// DST(i-1,j) = data0;
			// DST(i,j) = data1;
			// DST(i+1,j) = data2;
			// DST(i+2,j) = data3;
			
			// DST(i-1,j-1) = (data0+DST(i-1,j-2)+1)>>1;
			// DST(i,j-1) = (data1+DST(i,j-2)+1)>>1;
			// DST(i+1,j-1) = (data2+DST(i+1,j-2)+1)>>1;
			// DST(i+2,j-1) = (data3+DST(i+2,j-2)+1)>>1;			
		// }
		
		// for(i=i-2;i<img_width;i=i+2)
		// {
			// BLURn(n,i,j);
			
			// data3 = DST(i,j);
			// data2 = (data1+data3+1)>>1;
			
			// DST(i-1,j) = data2;
			
			// DST(i-1,j-1) = (data2+DST(i-1,j-2)+1)>>1;
			// DST(i,j-1) = (data3+DST(i,j-2)+1)>>1;
		// }
		// if(i!=img_width+1)
		// {
			// BLURn(n,img_width-1,j);
			// DST(img_width-1,j-1) = (DST(img_width-1,j)+DST(img_width-1,j-2)+1)>>1;
		// }
	// }
	// for(;j<img_height;j=j+2)
	// {
		// BLURn(n,0,j);
		// DST(0,j-1) = (DST(0,j)+DST(0,j-2)+1)>>1;
		// for(i=2;i<img_width;i=i+2)
		// {
			// BLURn(n,i,j);
			// DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;		
		
			// DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
			// DST(i-1,j-1) = (DST(i-1,j)+DST(i-1,j-2)+1)>>1;			
		// }
		// if(i!=img_width+1)
		// {
			// i = img_width-1;
			// BLURn(n,i,j);
			// DST(i,j-1) = (DST(i,j)+DST(i,j-2)+1)>>1;
		// }
	// }
	// if(j!=img_height+1)
	// {
		// j=img_height-1;
		
		// BLURn(n,0,j);
		// for(i=2;i<img_width;i=i+2)
		// {
			// BLURn(n,i,j);
			// DST(i-1,j) = (DST(i,j)+DST(i-2,j)+1)>>1;
		// }
		// BLURn(n,img_width-1,j);
	// }
	
	// free(p_src);
	// free(p_dst);
// }	

	

/*
void imgBlur(ImgMat *src,ImgMat *dst,int s)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int data_r0,data_r1,data_r2,data_r3,data_r4,data_r5;
	int data_l1,data_l2,data_l3,data_l4,data_l5;
	
	int i,j;
	
	if(s==2)
	{
		data_r2 = SRC(0,0);
		DST(0,0) = data_r2;
		for(i=2;i<img_width;i=i+2)
		{
			data_l2 = data_r2;
			data_r2 = SRC(i,0);
			
			DST(i-1,0) = (data_l2+data_r2+1)>>1;
			DST(i,0) = data_r2;
		}
		
		DST(i-1,0) = SRC(i-1,0);
		
		for(j=2;j<img_height;j=j+2)
		{
			data_r0 = SRC(0,j-2);
			data_r2 = SRC(0,j);
			
			data_r1 = (data_r0+data_r2+1)>>1;

			DST(0,j-1) = data_r1;
			DST(0,j) = data_r2;
			
			for(i=2;i<img_width;i=i+2)
			{
				data_l1 = data_r1;
				data_l2 = data_r2;
				
				data_r0 = SRC(i,j-2);
				data_r2 = SRC(i,j);
				
				data_r1 = (data_r0+data_r2+1)>>1;
				
				DST(i,j-1) = data_r1;
				DST(i,j) = data_r2;
				
				DST(i-1,j-1) = (data_l1+data_r1+1)>>1;
				DST(i-1,j) = (data_l2+data_r2+1)>>1;				
			}
			
			if(i==img_width)
			{
				DST(i-1,j) = SRC(i-1,j);
				DST(i-1,j-1) = (DST(i-1,j-2)+DST(i-1,j)+1)>>1;
			}
		}
		
		if(j==img_height)
		{
			data_r2 = SRC(0,j-1);
			DST(0,j-1) = data_r2;
			for(i=2;i<img_width;i=i+2)
			{
				data_l2 = data_r2;
				data_r2 = SRC(i,j-1);
				
				DST(i-1,j-1) = (data_l2+data_r2+1)>>1;
				DST(i,j-1) = data_r2;
			}
			DST(i-1,j-1) = SRC(i-1,j-1);
		}
	}
	else if(s == 3)
	{
		#define INTERPOLATION_3(x1,x2,x3,x4) {\
												(x2)=(((x1)<<1)+(x4)+1)/3;\
												(x3)=((x2)+(x4)+1)>>1;\
											 }
		
		data_r3 = SRC(0,0);
		DST(0,0) = data_r3;
		for(i=3;i<img_width;i=i+3)
		{
			data_l3 = data_r3;
			data_r3 = SRC(i,0);	
			DST(i,0) = data_r3;
			
			INTERPOLATION_3(data_l3,DST(i-2,0),DST(i-1,0),data_r3);
		}
		
		DST(i-2,0) = SRC(i-2,0);
		DST(i-1,0) = SRC(i-1,0);
				
		for(j=3;j<img_height;j=j+3)
		{
			data_r0 = SRC(0,j-3);
			data_r3 = SRC(0,j);

			INTERPOLATION_3(data_r0,data_r1,data_r2,data_r3);
			
			DST(0,j-2) = data_r1;
			DST(0,j-1) = data_r2;
			DST(0,j) = data_r3;
			
			for(i=3;i<img_width;i=i+3)
			{
				data_l1 = data_r1;
				data_l2 = data_r2;
				data_l3 = data_r3;
				
				data_r0 = SRC(i,j-3);
				data_r3 = SRC(i,j);
				
				INTERPOLATION_3(data_r0,data_r1,data_r2,data_r3);
				
				DST(i,j-2) = data_r1;
				DST(i,j-1) = data_r2;
				DST(i,j) = data_r3;
				
				INTERPOLATION_3(data_l1,DST(i-2,j-2),DST(i-1,j-2),data_r1);
				INTERPOLATION_3(data_l2,DST(i-2,j-1),DST(i-1,j-1),data_r2);
				INTERPOLATION_3(data_l3,DST(i-2,j),DST(i-1,j),data_r3);
			}
			
			if(i!=img_width+2)
			{
				i = img_width-1;
				
				data_l1 = DST(i-3,j-2);
				data_l2 = DST(i-3,j-1);
				data_l3 = DST(i-3,j);
				
				data_r0 = SRC(i,j-3);
				data_r3 = SRC(i,j);
				
				INTERPOLATION_3(data_r0,data_r1,data_r2,data_r3);
				
				DST(i,j-2) = data_r1;
				DST(i,j-1) = data_r2;
				DST(i,j) = data_r3;
				
				INTERPOLATION_3(data_l1,DST(i-2,j-2),DST(i-1,j-2),data_r1);
				INTERPOLATION_3(data_l2,DST(i-2,j-1),DST(i-1,j-1),data_r2);
				INTERPOLATION_3(data_l3,DST(i-2,j),DST(i-1,j),data_r3);
			}
		}
		
		if(j!=img_height+2)
		{
			j = img_height-1;
			
			data_r0 = DST(0,j-3);
			data_r3 = SRC(0,j);
			
			INTERPOLATION_3(data_r0,data_r1,data_r2,data_r3);

			DST(0,j-2) = data_r1;
			DST(0,j-1) = data_r2;
			DST(0,j) = data_r3;			
			
			for(i=3;i<img_width;i=i+3)
			{
				data_l1 = data_r1;
				data_l2 = data_r2;
				data_l3 = data_r3;
				
				data_r0 = SRC(i,j-3);
				data_r3 = SRC(i,j);
				
				INTERPOLATION_3(data_r0,data_r1,data_r2,data_r3);
				
				DST(i,j-2) = data_r1;
				DST(i,j-1) = data_r2;
				DST(i,j) = data_r3;
				
				INTERPOLATION_3(data_l1,DST(i-2,j-2),DST(i-1,j-2),data_r1);
				INTERPOLATION_3(data_l2,DST(i-2,j-1),DST(i-1,j-1),data_r2);
				INTERPOLATION_3(data_l3,DST(i-2,j),DST(i-1,j),data_r3);
			}
			
			if(i!=img_width+2)
			{
				i = img_width-1;
				
				data_l1 = DST(i-3,j-2);
				data_l2 = DST(i-3,j-1);
				data_l3 = DST(i-3,j);
				
				data_r0 = SRC(i,j-3);
				data_r3 = SRC(i,j);
				
				INTERPOLATION_3(data_r0,data_r1,data_r2,data_r3);
				
				DST(i,j-2) = data_r1;
				DST(i,j-1) = data_r2;
				DST(i,j) = data_r3;
				
				INTERPOLATION_3(data_l1,DST(i-2,j-2),DST(i-1,j-2),data_r1);
				INTERPOLATION_3(data_l2,DST(i-2,j-1),DST(i-1,j-1),data_r2);
				INTERPOLATION_3(data_l3,DST(i-2,j),DST(i-1,j),data_r3);
			}
		}
	}
}	
*/				
			


