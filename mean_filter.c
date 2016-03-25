#include <stdio.h>
#include "type.h"
#include "err.h"

void imgMeanFilter3(ImgMat *src,ImgMat *dst)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMeanFilter,src);
	DESTINATION_ERROR_CHECK(imgMeanFilter,dst);
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((dst->height != img_height)||(dst->width != img_width)||(dst->type != src->type))
		imgMatRedefine(dst,img_height,img_width,src->type);
	
	int img_size;
	img_size = src->size;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char *dst_data;
	unsigned char *src_data0;
	unsigned char *src_data1;
	unsigned char *src_data2;
	
	int sum0,sum1,sum2;
	
	for(k=0;k<cn;k++)
	{
		src_data0 = p_src;
		src_data2 = p_src+img_width;
		dst_data = p_dst;
		for(i=0;i<img_width;i++)
			dst_data[i] = src_data0[i];
		
		for(j=1;j<img_height-1;j++)
		{
			src_data1 = src_data0;
			src_data0 = src_data2;
			src_data2 = src_data0+img_width;
			dst_data = dst_data+img_width;
			
			sum1 = src_data0[0]+src_data1[0]+src_data2[0];
			sum2 = src_data0[1]+src_data1[1]+src_data2[1];
			
			dst_data[0] = src_data0[0];
			
			for(i=1;i<img_width-1;i++)
			{
				sum0 = sum1;
				sum1 = sum2;
				sum2 = src_data0[i+1]+src_data1[i+1]+src_data2[i+1];
				
				dst_data[i] = (sum0+sum1+sum2+4)/9;
			}
			dst_data[i] = src_data0[i];
		}
		
		dst_data = dst_data+img_width;
		for(i=0;i<img_width;i++)
			dst_data[i] = src_data2[i];
		
		p_src = p_src+img_size;
		p_dst = p_dst+img_size;
	}
}

void imgMeanFilter5(ImgMat *src,ImgMat *dst)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMeanFilter,src);
	DESTINATION_ERROR_CHECK(imgMeanFilter,dst);
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((dst->height != img_height)||(dst->width != img_width)||(dst->type != src->type))
		imgMatRedefine(dst,img_height,img_width,src->type);
	
	int img_size;
	img_size = src->size;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char *dst_data;
	unsigned char *src_data0;
	unsigned char *src_data1;
	unsigned char *src_data2;
	unsigned char *src_data3;
	unsigned char *src_data4;
	
	int sum0,sum1,sum2,sum3,sum4;
	
	for(k=0;k<cn;k++)
	{
		src_data2 = p_src;
		src_data0 = src_data2+img_width;
		src_data3 = src_data0+img_width;
		src_data4 = src_data3+img_width;
		dst_data = p_dst;
		
		for(i=0;i<2*img_width;i++)
			dst_data[i] = src_data2[i];
		
		for(j=2;j<img_height-2;j++)
		{
			src_data1 = src_data2;
			src_data2 = src_data0;
			src_data0 = src_data3;
			src_data3 = src_data4;
			src_data4 = src_data3+img_width;
			dst_data = dst_data+img_width;
			
			sum1 = src_data0[0]+src_data1[0]+src_data2[0]+src_data3[0]+src_data4[0];
			sum2 = src_data0[1]+src_data1[1]+src_data2[1]+src_data3[1]+src_data4[1];
			sum2 = src_data0[2]+src_data1[2]+src_data2[2]+src_data3[2]+src_data4[2];
			sum2 = src_data0[3]+src_data1[3]+src_data2[3]+src_data3[3]+src_data4[3];
			
			dst_data[0] = src_data0[0];
			dst_data[1] = src_data0[1];
			
			for(i=2;i<img_width-2;i++)
			{
				sum1 = sum2;
				sum2 = sum0;
				sum0 = sum3;
				sum3 = sum4;
				sum4 = src_data0[i+2]+src_data1[i+2]+src_data2[i+2]+src_data3[i+2]+src_data4[i+2];
				
				dst_data[i] = (sum0+sum1+sum2+sum3+sum4+12)/25;
			}
			
			dst_data[i] = src_data0[i];
			dst_data[i+1] = src_data0[i+1];
		}
		
		dst_data = dst_data+img_width;
		for(i=0;i<2*img_width;i++)
			dst_data[i] = src_data3[i];
		
		p_src = p_src+img_size;
		p_dst = p_dst+img_size;
	}
}

#define PTR(mat) {\
	p##mat[0] = p_##mat;\
	for(i=1;i<mat->height;i++)\
		p##mat[i] = p##mat[i-1]+mat->width;\
}\

#define SRC(x,y) *(psrc[y]+(x))
#define DST(x,y) *(pdst[y]+(x))

void imgMeanFilter(ImgMat *src,ImgMat *dst,int r)
{
	if(r == 1)
	{
		imgMeanFilter3(src,dst);
		return;
	}
	
	if(r == 2)
	{
		imgMeanFilter5(src,dst);
		return;
	}
	
	if(r==0)
	{
		imgCopyMat(src,dst);
		return;
	}
	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMeanFilter,src);
	DESTINATION_ERROR_CHECK(imgMeanFilter,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgMeanFilter.\n");
		exit(0);
	}
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((dst->height != img_height)||(dst->width != img_width)||(dst->type != src->type))
		imgMatRedefine(dst,img_height,img_width,src->type);
	
	int img_size;
	img_size = src->size;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k,m,n;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char **psrc;
	psrc = malloc(img_height<<2);
	unsigned char **pdst;	
	pdst = malloc(img_height<<2);
	
	int sum[31];
	int data_sum;
	
	int area;
	area = (r<<1)+1;
	area = area*area;
	
	for(k=0;k<cn;k++)
	{
		PTR(src);
		PTR(dst);
		
		for(i=0;i<img_width*r;i++)
			DST(i,0) = SRC(i,0);
		
		for(j=r;j<img_height-r;j++)
		{
			data_sum = 0;
			sum[0] = 0;
			for(n=1;n<2*r+1;n++)
			{
				sum[n] = SRC(n-1,j+r);
				for(m=-r;m<r;m++)
					sum[n] = sum[n] + SRC(n-1,j+m);
				
				data_sum = data_sum + sum[n];
			}
			
			for(i=0;i<r;i++)
				DST(i,j) = SRC(i,j);
			
			for(i=r;i<img_width-r;i++)
			{
				data_sum = data_sum-sum[0];
				
				for(n=0;n<2*r;n++)
					sum[n] = sum[n+1];
				
				sum[n] = SRC(i+r,j+r);
				for(m=-r;m<r;m++)
					sum[n] = sum[n] + SRC(i+r,j+m);
				
				data_sum = data_sum+sum[n];
				
				DST(i,j) = (data_sum+(area>>1))/area;		
			}
			
			for(;i<img_width;i++)
				DST(i,j) = SRC(i,j);			
		}
		for(i=0;i<img_width*r;i++)
			DST(i,j) = SRC(i,j);
		
		p_src = p_src+img_size;
		p_dst = p_dst+img_size;
	}
	
	free(psrc);
	free(pdst);
}

ImgMat *imgCreateMat(int height,int width,char type);

void MeanFilter(ImgMat *src,ImgMat *dst,int r)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMeanFilter,src);
	#endif
	
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgMeanFilter(src,dst,r);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgMeanFilter(src,dst,r);
}
