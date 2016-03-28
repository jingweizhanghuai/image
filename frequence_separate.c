#include <stdio.h>
#include "type.h"
#include "err.h"

void imgFrequenceSeparate3(ImgMat *src,ImgMat *lf,ImgMat *hf)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFrequenceSeparate,src);
	DESTINATION_ERROR_CHECK(imgFrequenceSeparate,lf);
	DESTINATION_ERROR_CHECK(imgFrequenceSeparate,hf);
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((lf->height != img_height)||(lf->width != img_width)||(lf->type != src->type))
		imgMatRedefine(lf,img_height,img_width,src->type);
	
	int hf_type;
	hf_type = (src->type)&0x01;
	
	if((hf->height != img_height)||(hf->width != img_width)||(hf->type != hf_type))
		imgMatRedefine(hf,img_height,img_width,hf_type);
	
	int img_size;
	img_size = src->size;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_lf;
	p_lf = lf->data.ptr;
	
	char *p_hf;
	p_hf = (char *)hf->data.ptr;
	
	unsigned char *lf_data;
	char *hf_data;
	unsigned char *src_data0;
	unsigned char *src_data1;
	unsigned char *src_data2;
	
	int sum0,sum1,sum2;
	
	for(k=0;k<cn;k++)
	{
		src_data0 = p_src;
		src_data2 = p_src+img_width;
		lf_data = p_lf;
		hf_data = p_hf;
		
		for(i=0;i<img_width;i++)
		{
			lf_data[i] = src_data0[i];
			hf_data[i] = 0;
		}
		
		for(j=1;j<img_height-1;j++)
		{
			src_data1 = src_data0;
			src_data0 = src_data2;
			src_data2 = src_data0+img_width;
			lf_data = lf_data+img_width;
			hf_data = hf_data+img_width;
			
			sum1 = src_data0[0]+src_data1[0]+src_data2[0];
			sum2 = src_data0[1]+src_data1[1]+src_data2[1];
			
			lf_data[0] = src_data0[0];
			hf_data[0] = 0;
			
			for(i=1;i<img_width-1;i++)
			{
				sum0 = sum1;
				sum1 = sum2;
				sum2 = src_data0[i+1]+src_data1[i+1]+src_data2[i+1];
				
				lf_data[i] = (sum0+sum1+sum2+4)/9;
				hf_data[i] = src_data0[i] - lf_data[i];
			}
			lf_data[i] = src_data0[i];
			hf_data[i] = 0;
		}
		
		lf_data = lf_data+img_width;
		hf_data = hf_data+img_width;
		for(i=0;i<img_width;i++)
		{
			lf_data[i] = src_data2[i];
			hf_data[i] = 0;
		}
		
		p_src = p_src+img_size;
		p_lf = p_lf+img_size;
		p_hf = p_hf+img_size;
	}
}

void imgFrequenceSeparate5(ImgMat *src,ImgMat *lf,ImgMat *hf)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFrequenceSeparate,src);
	DESTINATION_ERROR_CHECK(imgFrequenceSeparate,lf);
	DESTINATION_ERROR_CHECK(imgFrequenceSeparate,hf);
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((lf->height != img_height)||(lf->width != img_width)||(lf->type != src->type))
		imgMatRedefine(lf,img_height,img_width,src->type);
	
	int hf_type;
	hf_type = (src->type)&0x01;
	
	if((hf->height != img_height)||(hf->width != img_width)||(hf->type != hf_type))
		imgMatRedefine(hf,img_height,img_width,hf_type);
	
	int img_size;
	img_size = src->size;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_lf;
	p_lf = lf->data.ptr;
	
	char *p_hf;
	p_hf = (char *)hf->data.ptr;
	
	unsigned char *lf_data;
	char *hf_data;
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
		lf_data = p_lf;
		hf_data = p_hf;
		
		for(i=0;i<2*img_width;i++)
		{
			lf_data[i] = src_data2[i];
			hf_data[i] = 0;
		}
		
		for(j=2;j<img_height-2;j++)
		{
			src_data1 = src_data2;
			src_data2 = src_data0;
			src_data0 = src_data3;
			src_data3 = src_data4;
			src_data4 = src_data3+img_width;
			lf_data = lf_data+img_width;
			hf_data = hf_data+img_width;
			
			sum1 = src_data0[0]+src_data1[0]+src_data2[0]+src_data3[0]+src_data4[0];
			sum2 = src_data0[1]+src_data1[1]+src_data2[1]+src_data3[1]+src_data4[1];
			sum2 = src_data0[2]+src_data1[2]+src_data2[2]+src_data3[2]+src_data4[2];
			sum2 = src_data0[3]+src_data1[3]+src_data2[3]+src_data3[3]+src_data4[3];
			
			lf_data[0] = src_data0[0];
			lf_data[1] = src_data0[1];
			hf_data[0] = 0;
			hf_data[1] = 0;
			
			for(i=2;i<img_width-2;i++)
			{
				sum1 = sum2;
				sum2 = sum0;
				sum0 = sum3;
				sum3 = sum4;
				sum4 = src_data0[i+2]+src_data1[i+2]+src_data2[i+2]+src_data3[i+2]+src_data4[i+2];
				
				lf_data[i] = (sum0+sum1+sum2+sum3+sum4+12)/25;
				hf_data[i] = src_data0[i]-lf_data[i];
			}
			
			lf_data[i] = src_data0[i];
			lf_data[i+1] = src_data0[i+1];
			hf_data[i] = 0;
			hf_data[i+1] = 0;
		}
		
		lf_data = lf_data+img_width;
		hf_data = hf_data+img_width;
		for(i=0;i<2*img_width;i++)
		{
			lf_data[i] = src_data3[i];
			hf_data[i] = 0;
		}
		
		p_src = p_src+img_size;
		p_lf = p_lf+img_size;
		p_hf = p_hf+img_size;
	}
}

#define PTR(mat) {\
	p##mat[0] = p_##mat;\
	for(i=1;i<mat->height;i++)\
		p##mat[i] = p##mat[i-1]+mat->width;\
}\

#define SRC(x,y) *(psrc[y]+(x))
#define LF(x,y) *(plf[y]+(x))
#define HF(x,y) *(phf[y]+(x))

void imgFrequenceSeparate(ImgMat *src,ImgMat *lf,ImgMat *hf,int r)
{
	if(r == 1)
	{
		imgFrequenceSeparate3(src,lf,hf);
		return;
	}
	
	if(r == 2)
	{
		imgFrequenceSeparate5(src,lf,hf);
		return;
	}
	
	if(r==0)
	{
		imgCopyMat(src,lf);
		imgCleanMat(hf);
		return;
	}
	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFrequenceSeparate,src);
	DESTINATION_ERROR_CHECK(imgFrequenceSeparate,lf);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgFrequenceSeparate.\n");
		exit(0);
	}
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((lf->height != img_height)||(lf->width != img_width)||(lf->type != src->type))
		imgMatRedefine(lf,img_height,img_width,src->type);
	
	int hf_type;
	hf_type = (src->type)&0x01;
	
	if((hf->height != img_height)||(hf->width != img_width)||(hf->type != hf_type))
		imgMatRedefine(hf,img_height,img_width,hf_type);
	
	int img_size;
	img_size = src->size;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k,m,n;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_lf;
	p_lf = lf->data.ptr;
	
	char *p_hf;
	p_hf = (char *)hf->data.ptr;
	
	unsigned char **psrc;
	psrc = malloc(img_height<<2);
	unsigned char **plf;	
	plf = malloc(img_height<<2);
	char **phf;	
	phf = malloc(img_height<<2);
	
	int sum[31];
	int data_sum;
	
	if(r>15)
		r=15;
	
	int area;
	area = (r<<1)+1;
	area = area*area;
	
	for(k=0;k<cn;k++)
	{
		PTR(src);
		PTR(lf);
		PTR(hf);
		
		for(i=0;i<img_width*r;i++)
		{
			LF(i,0) = SRC(i,0);
			HF(i,0) = 0;
		}
		
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
			{
				LF(i,j) = SRC(i,j);
				HF(i,j) = 0;
			}
			
			for(i=r;i<img_width-r;i++)
			{
				data_sum = data_sum-sum[0];
				
				for(n=0;n<2*r;n++)
					sum[n] = sum[n+1];
				
				sum[n] = SRC(i+r,j+r);
				for(m=-r;m<r;m++)
					sum[n] = sum[n] + SRC(i+r,j+m);
				
				data_sum = data_sum+sum[n];
				
				LF(i,j) = (data_sum+(area>>1))/area;	
				HF(i,j) = SRC(i,j)-LF(i,j);			
			}
			
			for(;i<img_width;i++)
			{
				LF(i,j) = SRC(i,j);			
				HF(i,j) = 0;
			}
		}
		
		for(i=0;i<img_width*r;i++)
		{
			LF(i,j) = SRC(i,j);
			HF(i,j) = 0;
		}
		
		p_src = p_src+img_size;
		p_lf = p_lf+img_size;
		p_hf = p_hf+img_size;
	}
	
	free(psrc);
	free(plf);
	free(phf);
}

ImgMat *imgCreateMat(int height,int width,char type);

void FrequenceSeparate(ImgMat *src,ImgMat *lf,ImgMat *hf,int r)
{
	if(lf==NULL)
		HighFrequency(src,hf,r);
	else if(hf==NULL)
		imgMeanFilter(src,lf,r);
	else
		imgFrequenceSeparate(src,lf,hf,r);
}
