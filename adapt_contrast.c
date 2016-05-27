#include <stdio.h>
#include "type.h"
#include "err.h"

#define PTR(mat,type) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = (type *)mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
}

#define SRC(x,y) (*(p0src[y]+(x)))
#define DST(x,y) (*(p0dst[y]+(x)))

void imgAdaptContrast(ImgMat *src,ImgMat *dst,int llv,int ulv,int r)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAdaptContrast,src);
	DESTINATION_ERROR_CHECK(imgAdaptContrast,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgAdaptContrast: r is %d\n",r);
		exit(0);
	}
	#endif
	
	if(src->memory_valid[0]==0)
		imgReadMatOCLMemory(src);
	
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
	#ifdef DEBUG
	if(cn != 1)
	{
		printf("IMG Error:\n\tin imgAdaptContrast: cn is not 1(which is %d)\n",cn);
		exit(0);
	}
	#endif
	
	
	int i,j,m,n;	
	
	unsigned char **p0src;
	PTR(src,unsigned char);
	
	unsigned char **p0dst;
	PTR(dst,unsigned char);
	
	int sum[31];
	int data_sum;
	float data_mean;
	
	int area;
	area = (r<<1)+1;
	area = area*area;
	
	int data;
	float k;
	
	for(i=0;i<img_width*r;i++)
	{
		data = SRC(i,0);
		if((data<llv))
		{
			k = (float)llv/(float)data;
			k = (k>2.5)?2.5:k;
			data = (int)(((float)data)*k);
			if(data>ulv)
				data = ulv;				
		}
		else if(data>ulv)
		{
			k = (float)ulv/(float)data;
			if(i==300)
			{
				printf("ulv is %d\n",ulv);
				printf("k is %f\n",k);
				printf("data is %d\n",data);
			}
			k = (k<0.4)?0.4:k;
			
			data = (int)(((float)data)*k);
			
			if(data<llv)
				data = llv;
		}
		DST(i,0) = data;
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
			data = SRC(i,j);
			if((data<llv))
			{
				k = (float)llv/(float)data;
				k = (k>2.5)?2.5:k;
				data = (int)(((float)data)*k);
				if(data>ulv)
					data = ulv;				
			}
			else if(data>ulv)
			{
				k = (float)ulv/(float)data;
				if(i==300)
				{
					printf("ulv is %d\n",ulv);
					printf("k is %f\n",k);
					printf("data is %d\n",data);
				}
				k = (k<0.4)?0.4:k;
				
				data = (int)(((float)data)*k);
				
				if(data<llv)
					data = llv;
			}
			DST(i,j) = data;
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
			data_mean = ((float)data_sum)/((float)area);
			
			data = SRC(i,j);	
			
			if((data_mean<llv))
			{
				k = (float)llv/data_mean;
				k = (k>2.5)?2.5:k;
				if((i==210)&&(j==198))
				{
					printf("data_mean is %f\n",data_mean);
					printf("k is %f\n",k);
					printf("data is %d\n",data);
				}
				data = (int)(((float)data)*k);
				if(data>ulv)
					data = ulv;				
			}
			else if(data_mean>ulv)
			{
				k = (float)ulv/data_mean;
				k = (k<0.4)?0.4:k;
				data = (int)(((float)data)*k);
				if(data<llv)
					data = llv;
			}
			
			DST(i,j) = data;
		}
		
		for(;i<img_width;i++)
		{
			data = SRC(i,j);
			if((data<llv))
			{
				k = (float)llv/(float)data;
				k = (k>2.5)?2.5:k;
				data = (int)(((float)data)*k);
				if(data>ulv)
					data = ulv;				
			}
			else if(data>ulv)
			{
				k = (float)ulv/(float)data;
				k = (k<0.4)?0.4:k;
				
				data = (int)(((float)data)*k);
				
				if(data<llv)
					data = llv;
			}
			DST(i,j) = data;
		}			
	}
	for(i=0;i<img_width*r;i++)
	{
		data = SRC(i,j);
		if((data<llv))
		{
			k = (float)llv/(float)data;
			k = (k>2.5)?2.5:k;
			data = (int)(((float)data)*k);
			if(data>ulv)
				data = ulv;				
		}
		else if(data>ulv)
		{
			k = (float)ulv/(float)data;
			k = (k<0.4)?0.4:k;
			
			data = (int)(((float)data)*k);
			
			if(data<llv)
				data = llv;
		}
		DST(i,j) = data;
	}
	
	free(p0src);
	free(p0dst);
	
}