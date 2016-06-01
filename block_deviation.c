#include <stdio.h>
#include "type.h"

#define PTR(mat,type) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = (type *)mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
}

#define SRC(x,y) (*(p0src[y]+(x)))
#define DST(x,y) (*(p0dst[y]+(x)))

void imgBlockMean(ImgMat *src,int nx,int ny,float *mean,int *min,int *max)
{
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	int block_width;
	block_width = img_width/nx;

	int block_height;
	block_height = img_height/ny;
	
	int block_size;
	
	float block_mean;
	int block_min;
	int block_max;
	
	int i,j;
	int m,n;
	
	unsigned char **p0src;
	PTR(src,unsigned char);
	
	int lx1,lx2,ly1,ly2;
	
	int sum;
	int data;
	
	ly1 = 0;
	ly2 = block_height;
	for(n=0;n<ny;n++)
	{
		lx1 = 0;
		lx2 = block_width;
		
		for(m=0;m<nx;m++)
		{
			sum = 0;
			block_size = 0;
			block_min = 255;
			block_max = 0;
			for(j=ly1;j<ly2;j++)
				for(i=lx1;i<lx2;i++)
				{
					data = SRC(i,j);
					
					if(data==0)
						continue;
					sum = sum+data;
					block_min = (data<block_min)?data:block_min;
					block_max = (data>block_max)?data:block_max;
					block_size = block_size+1;
					
				}
				
			block_mean = (float)sum/(float)block_size;
			
			*mean = block_mean;
			*max = block_max;
			*min = block_min;
			
			mean = mean+1;
			max = max+1;
			min = min+1;
			
			// printf("(%d,%d) ",lx1,ly1);
			// printf("%f\t",block_variance);
			
			lx1 = lx2;
			lx2 = lx2+block_width;
		}
		// printf("\n");
		ly1 = ly2;
		ly2 = ly2+block_height;
	}
	
	free(p0src);
}

void imgBlockVariance(ImgMat *src,int nx,int ny,float *mean,float *variance)
{
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	int block_width;
	block_width = img_width/nx;

	int block_height;
	block_height = img_height/ny;
	
	int block_size;
	
	float block_mean;
	float block_variance;
	
	int i,j;
	int m,n;
	
	unsigned char **p0src;
	PTR(src,unsigned char);
	
	int lx1,lx2,ly1,ly2;
	
	int sum;
	int data;
	
	ly1 = 0;
	ly2 = block_height;
	for(n=0;n<ny;n++)
	{
		lx1 = 0;
		lx2 = block_width;
		
		for(m=0;m<nx;m++)
		{
			sum = 0;
			block_size = 0;
			for(j=ly1;j<ly2;j++)
				for(i=lx1;i<lx2;i++)
				{
					data = SRC(i,j);
					
					if(data==0)
						continue;
					
					block_size = block_size+1;
					sum = sum+data;
				}
				
			block_mean = (float)sum/(float)block_size;
				
			sum = 0;
			for(j=ly1;j<ly2;j++)
				for(i=lx1;i<lx2;i++)
				{
					data = SRC(i,j);
					
					if(data==0)
						continue;
					
					data = data-block_mean;
					sum = sum+data*data;
				}
			
			block_variance = (float)sum/(float)block_size;
			
			*mean = block_mean;
			*variance = block_variance;
			
			mean = mean+1;
			variance = variance+1;
			
			// printf("(%d,%d) ",lx1,ly1);
			// printf("%f\t",block_variance);
			
			lx1 = lx2;
			lx2 = lx2+block_width;
		}
		// printf("\n");
		ly1 = ly2;
		ly2 = ly2+block_height;
	}
	
	free(p0src);
}

void imgBlockContrast(ImgMat *src,ImgMat *dst,int nx,int ny,int llv,int ulv)
{
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	int block_width;
	block_width = img_width/nx;

	int block_height;
	block_height = img_height/ny;
	
	int i,j;
	int m,n;
	
	unsigned char **p0src;
	PTR(src,unsigned char);
	
	unsigned char **p0dst;
	PTR(dst,unsigned char);
	
	float *mean;
	mean = malloc(nx*ny*sizeof(float));
	float *p_mean;
	p_mean = mean;
	
	int *min;
	min = malloc(nx*ny*sizeof(int));
	int *p_min;
	p_min = min;
	
	int *max;
	max = malloc(nx*ny*sizeof(int));
	int *p_max;
	p_max = max;
	
	imgBlockMean(src,nx,ny,mean,min,max);
		
	float *k;
	k = malloc(nx*ny*sizeof(float));
	float *p_k;
	p_k = k;
	
	float *b;
	b = malloc(nx*ny*sizeof(float));
	float *p_b;
	p_b = b;
	
	float block_mean;
	int block_min;
	int block_max;
	float block_k;
	float block_b;
	
	for(n=0;n<ny;n++)
	{		
		for(m=0;m<nx;m++)
		{
			block_mean = *mean;
			block_min = *min;
			block_max = *max;
			
			mean = mean+1;
			max = max+1;
			min = min+1;
			
			if(block_mean<llv)
			{
				block_k = ((float)(128-block_min))/(block_mean-(float)block_min);
				if(block_k>3.0)
					block_k = 3.0;
				block_b = (float)block_min-(block_k*(float)block_min);
			}
			else if(block_mean>ulv)
			{
				block_k = ((float)(block_max-128))/((float)block_max-block_mean);
				if(block_k>3.0)
					block_k = 3.0;
				block_b = (float)block_max-(block_k*(float)block_max);
			}
			else
			{
				block_k = 1.0;
				block_b = 0.0;
			}
			
			if((m==4)&&(n==3))
			{
				printf("mean is %f\n",block_mean);
				printf("min is %d\n",block_min);
				printf("max is %d\n",block_max);
				printf("block_k is %f\n",block_k);
				printf("block_b is %f\n",block_b);
			}
			
			if((m==5)&&(n==3))
			{
				printf("mean is %f\n",block_mean);
				printf("min is %d\n",block_min);
				printf("max is %d\n",block_max);
				printf("block_k is %f\n",block_k);
				printf("block_b is %f\n",block_b);
			}
			
			*k = block_k;
			*b = block_b;
			
			k = k+1;
			b = b+1;
		}
	}
	
	// printf("aaaaaaaaaaaaaaaaaaaaa\n");
	
	float lx,ly;
	int lx1,lx2,ly1,ly2;
	float wx,wy;
	float w1,w2,w3,w4;
	float k1,k2,k3,k4;
	float b1,b2,b3,b4;
	float lk,lb;
	float data;
	
	for(j=0;j<img_height;j++)
	{
		ly = (float)j/(float)block_height-0.5;
		if(ly<=0)
		{
			ly1 = 0;
			ly2 = 0;
		}
		else if(ly>=(float)(ny-1))
		{
			ly1 = ny-1;
			ly2 = ny-1;
		}
		else
		{
			ly1 = (int)ly;
			ly2 = ly1+1;
		}
		wy = ly-(float)ly1;
		
		// if((j&0x01)==0)
		// printf("%d:  ly is %f,%d,%d\n",j,ly,ly1,ly2);
		// return;
		
		for(i=0;i<img_width;i++)
		{
			lx = (float)i/(float)block_width-0.5;
			if(lx<=0)
			{
				lx1 = 0;
				lx2 = 0;
			}
			else if(lx>=(float)(nx-1))
			{
				lx1 = nx-1;
				lx2 = nx-1;
			}	
			else
			{
				lx1 = (int)lx;
				lx2 = lx1+1;
			}
			wx = lx-(float)lx1;
			
			
			
			w1 = (1.0-wx)*(1.0-wy);
			w2 = wx*(1.0-wy);
			w3 = (1.0-wx)*wy;
			w4 = wx*wy;
			
			k1 = p_k[ly1*nx+lx1];
			b1 = p_b[ly1*nx+lx1];
			k2 = p_k[ly1*nx+lx2];
			b2 = p_b[ly1*nx+lx2];
			k3 = p_k[ly2*nx+lx1];
			b3 = p_b[ly2*nx+lx1];
			k4 = p_k[ly2*nx+lx2];
			b4 = p_b[ly2*nx+lx2];
			
			lk = k1*w1+k2*w2+k3*w3+k4*w4;
			lb = b1*w1+b2*w2+b3*w3+b4*w4;
			
			data = (float)SRC(i,j)*lk+lb;
			
			if(((i==164)||(i==165))&&(j==141))
			{
				printf("(%d,%d)\n",i,j);
				printf("lx is %d,%d\n",lx1,lx2);
				printf("ly is %d,%d\n",ly1,ly2);
				printf("lk is %f\t%f,%f,%f,%f\n",lk,k1,k2,k3,k4);
				printf("lb is %f\t%f,%f,%f,%f\n",lb,b1,b2,b3,b4);
				printf("w is %f,%f,%f,%f\n",w1,w2,w3,w4);
				printf("SRC(i,j) is %d\n",SRC(i,j));
				printf("data is %f\n",data);
			}
			
			
			
			if(data>255)
				DST(i,j) = 255;
			else if(data<0)
				DST(i,j) = 0;
			else
				DST(i,j) = (int)data;
			
			
		}
	}
	
	// printf("aaaaaaaaaaaaaaaaaaaaaaaa\n");
	free(p_mean);
	free(p_max);
	free(p_min);
	free(p_k);
	free(p_b);
	
	dst->memory_valid[0] = 1;
	dst->memory_valid[1] = 0;
}
			
			
			
			