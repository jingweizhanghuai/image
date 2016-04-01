#include <stdio.h>
#include "type.h"
#include "err.h"

#define PTR_1(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
}\

#define PTR_3(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	p1##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p1##mat[i] = p0##mat[i]+mat->size;\
	p2##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p2##mat[i] = p1##mat[i]+mat->size;\
}\

#define PTR_4(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	p1##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p1##mat[i] = p0##mat[i]+mat->size;\
	p2##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p2##mat[i] = p1##mat[i]+mat->size;\
	p3##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p3##mat[i] = p2##mat[i]+mat->size;\
}\

#define SRC_0(x,y) *(p0src[y]+(x))
#define DST_0(x,y) *(p0dst[y]+(x))
#define SRC_1(x,y) *(p1src[y]+(x))
#define DST_1(x,y) *(p1dst[y]+(x))
#define SRC_2(x,y) *(p2src[y]+(x))
#define DST_2(x,y) *(p2dst[y]+(x))
#define SRC_3(x,y) *(p3src[y]+(x))
#define DST_3(x,y) *(p3dst[y]+(x))

#define ABS(x) (((x)>0)?(x):(0-(x)))

void imgFilter_gray(ImgMat *src,ImgMat *dst,int r,int thresh)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFilter,src);
	DESTINATION_ERROR_CHECK(imgFilter,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgFilter.\n");
		exit(0);
	}
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=1)
	{
		printf("IMG Error:\n\tin imgFilter.\n");
		exit(0);
	}
	#endif
	
	if(r==0)
	{
		imgCopyMat(src,dst);
		return;
	}
	
	if(thresh<=0)
	{
		imgMeanFilter(src,dst,r);
		return;
	}
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	int i,j;
	
	unsigned char **p0src;
	PTR_1(src);
	
	unsigned char **p0dst;
	PTR_1(dst);
	
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
			DST_0(i,j) = SRC_0(i,j);	
	
	for(j=r;j<img_height-r;j++)
	{
		for(x=1;x<=r*2;x++)	
			for(y=0;y<=r*2;y++)
				data[x][y] = SRC_0(x-1,j-r+y);
		
		for(x=0;x<r;x++)
			DST_0(x,j) = data[x+1][r];
			
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
				data[x][y] = SRC_0(i-r+x,j-r+y);
				
				if(ABS(data[x][y] - data0)<thresh)
				{
					sum = sum + data[x][y];
					num = num +1;
				}
			}
			
			DST_0(i,j) = (sum+(num>>1))/num;
		}
		for(x=0;x<r;x++)
			DST_0(i+x,j) = data[x+r][r];
		
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
			DST_0(i,j) = SRC_0(i,j);
		
	free(p0src);
	free(p0dst);
}

void imgFilter_3(ImgMat *src,ImgMat *dst,int r,int thresh)
{	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFilter,src);
	DESTINATION_ERROR_CHECK(imgFilter,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgFilter.\n");
		exit(0);
	}
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=3)
	{
		printf("IMG Error:\n\tin imgFilter.\n");
		exit(0);
	}
	#endif
	
	if(r==0)
	{
		imgCopyMat(src,dst);
		return;
	}
	
	if(thresh<=0)
	{
		imgMeanFilter(src,dst,r);
		return;
	}
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC3))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC3);
	
	int i,j;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	PTR_3(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	PTR_3(dst);
	
	if(r>10)
		r = 10;
	else if(r<1)
		r = 1;
	
	int data_0[21][21],data_1[21][21],data_2[21][21];
		
	int x,y;
	int data0_0,data0_1,data0_2;
	int sum_0,sum_1,sum_2;
	int num;
	int diff;
	
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
			num = 0;
			
			for(x=0;x<r*2;x++)
				for(y=0;y<=r*2;y++)
				{
					data_0[x][y] = data_0[x+1][y];
					data_1[x][y] = data_1[x+1][y];
					data_2[x][y] = data_2[x+1][y];
					
					diff = (ABS(data_0[x][y] - data0_0)+ABS(data_1[x][y] - data0_1)+ABS(data_2[x][y] - data0_2))/3;
					
					if(diff<thresh)
					{
						sum_0 = sum_0 + data_0[x][y];
						sum_1 = sum_1 + data_1[x][y];
						sum_2 = sum_2 + data_2[x][y];
						num = num +1;
					}
				}			
			
			for(y=0;y<=r*2;y++)
			{
				data_0[x][y] = SRC_0(i-r+x,j-r+y);
				data_1[x][y] = SRC_1(i-r+x,j-r+y);
				data_2[x][y] = SRC_2(i-r+x,j-r+y);
				
				diff = (ABS(data_0[x][y] - data0_0)+ABS(data_1[x][y] - data0_1)+ABS(data_2[x][y] - data0_2))/3;
				
				if(diff<thresh)
				{
					sum_0 = sum_0 + data_0[x][y];
					sum_1 = sum_1 + data_1[x][y];
					sum_2 = sum_2 + data_2[x][y];
					num = num +1;
				}
			}
			
			DST_0(i,j) = (sum_0+(num>>1))/num;
			DST_1(i,j) = (sum_1+(num>>1))/num;
			DST_2(i,j) = (sum_2+(num>>1))/num;
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
		
	free(p0src);
	free(p1src);
	free(p2src);
	free(p0dst);
	free(p1dst);
	free(p2dst);
}

void imgFilter_4(ImgMat *src,ImgMat *dst,int r,int thresh)
{	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFilter,src);
	DESTINATION_ERROR_CHECK(imgFilter,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgFilter.\n");
		exit(0);
	}
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=4)
	{
		printf("IMG Error:\n\tin imgFilter.\n");
		exit(0);
	}
	#endif
	
	if(r==0)
	{
		imgCopyMat(src,dst);
		return;
	}
	
	if(thresh<=0)
	{
		imgMeanFilter(src,dst,r);
		return;
	}
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC3))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC3);
	
	int i,j;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	unsigned char **p3src;
	PTR_4(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	unsigned char **p3dst;
	PTR_4(dst);
	
	if(r>10)
		r = 10;
	else if(r<1)
		r = 1;
	
	int data_0[21][21],data_1[21][21],data_2[21][21],data_3[21][21];
		
	int x,y;
	int data0_0,data0_1,data0_2,data0_3;
	int sum_0,sum_1,sum_2,sum_3;
	int num;
	int diff;
	
	for(j=0;j<r;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);	
			DST_2(i,j) = SRC_2(i,j);
			DST_3(i,j) = SRC_3(i,j);
		}
	
	for(j=r;j<img_height-r;j++)
	{
		for(x=1;x<=r*2;x++)	
			for(y=0;y<=r*2;y++)
			{
				data_0[x][y] = SRC_0(x-1,j-r+y);
				data_1[x][y] = SRC_1(x-1,j-r+y);
				data_2[x][y] = SRC_2(x-1,j-r+y);
				data_3[x][y] = SRC_3(x-1,j-r+y);
			}
		
		for(x=0;x<r;x++)
		{
			DST_0(x,j) = data_0[x+1][r];
			DST_1(x,j) = data_1[x+1][r];
			DST_2(x,j) = data_2[x+1][r];
			DST_3(x,j) = data_3[x+1][r];
		}

		for(i=r;i<img_width-r;i++)
		{
			data0_0 = data_0[r+1][r];
			data0_1 = data_1[r+1][r];
			data0_2 = data_2[r+1][r];
			data0_3 = data_3[r+1][r];
			
			sum_0 = 0;
			sum_1 = 0;
			sum_2 = 0;
			num = 0;
			
			for(x=0;x<r*2;x++)
				for(y=0;y<=r*2;y++)
				{
					data_0[x][y] = data_0[x+1][y];
					data_1[x][y] = data_1[x+1][y];
					data_2[x][y] = data_2[x+1][y];
					data_3[x][y] = data_3[x+1][y];
					
					diff = (ABS(data_0[x][y] - data0_0)+ABS(data_1[x][y] - data0_1)+ABS(data_2[x][y] - data0_2)+ABS(data_3[x][y] - data0_3))>>2;
					
					if(diff<thresh)
					{
						sum_0 = sum_0 + data_0[x][y];
						sum_1 = sum_1 + data_1[x][y];
						sum_2 = sum_2 + data_2[x][y];
						sum_3 = sum_3 + data_3[x][y];
						num = num +1;
					}
				}			
			
			for(y=0;y<=r*2;y++)
			{
				data_0[x][y] = SRC_0(i-r+x,j-r+y);
				data_1[x][y] = SRC_1(i-r+x,j-r+y);
				data_2[x][y] = SRC_2(i-r+x,j-r+y);
				data_3[x][y] = SRC_3(i-r+x,j-r+y);
				
				diff = (ABS(data_0[x][y] - data0_0)+ABS(data_1[x][y] - data0_1)+ABS(data_2[x][y] - data0_2)+ABS(data_3[x][y] - data0_3))>>2;
				
				if(diff<thresh)
				{
					sum_0 = sum_0 + data_0[x][y];
					sum_1 = sum_1 + data_1[x][y];
					sum_2 = sum_2 + data_2[x][y];
					sum_3 = sum_3 + data_3[x][y];
					num = num +1;
				}
			}
			
			DST_0(i,j) = (sum_0+(num>>1))/num;
			DST_1(i,j) = (sum_1+(num>>1))/num;
			DST_2(i,j) = (sum_2+(num>>1))/num;
			DST_3(i,j) = (sum_3+(num>>1))/num;
		}
		for(x=0;x<r;x++)
		{
			DST_0(i+x,j) = data_0[x+r][r];
			DST_1(i+x,j) = data_1[x+r][r];
			DST_2(i+x,j) = data_2[x+r][r];
			DST_3(i+x,j) = data_3[x+r][r];
		}
		
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
			DST_3(i,j) = SRC_3(i,j);
		}
		
	free(p0src);
	free(p1src);
	free(p2src);
	free(p3src);
	free(p0dst);
	free(p1dst);
	free(p2dst);
	free(p3dst);
}

void imgFilter(ImgMat *src,ImgMat *dst,int r,int thresh)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFilter,src);
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn==1)
		imgFilter_gray(src,dst,r,thresh);
	else if(cn == 3)
		imgFilter_3(src,dst,r,thresh);
	else if(cn == 4)
		imgFilter_3(src,dst,r,thresh);
	else 
	{
		printf("IMG Error:\n\tin imgFilter.\n");
		exit(0);
	}
}

ImgMat *imgCreateMat(int height,int width,char type);

void Filter(ImgMat *src,ImgMat *dst,int r,int thresh) 
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFilter,src);
	#endif
	
	if(dst==NULL)
	{
		if(r==0)
			return;

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
