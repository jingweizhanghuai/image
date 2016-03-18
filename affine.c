#include <stdio.h>
#include "type.h"
#include "err.h"

#define SRC_PTR {\
	p_src = malloc((src->height)<<2);\
	p_src[0] = src->data.ptr;\
	for(i=1;i<src->height;i++)\
		p_src[i] = p_src[i-1]+src->step;\
}\

#define DST_PTR {\
	p_dst = malloc((dst->height)<<2);\
	p_dst[0] = dst->data.ptr;\
	for(i=1;i<dst->height;i++)\
		p_dst[i] = p_dst[i-1]+dst->step;\
}\

#define SRC(x,y) *(p_src[y]+(x))
#define DST(x,y) *(p_dst[y]+(x))

#define SRC3_0(x,y) *(p_src[y]+(x)+(x)+(x))
#define DST3_0(x,y) *(p_dst[y]+(x)+(x)+(x))
#define SRC3_1(x,y) *(p_src[y]+(x)+(x)+(x)+1)
#define DST3_1(x,y) *(p_dst[y]+(x)+(x)+(x)+1)
#define SRC3_2(x,y) *(p_src[y]+(x)+(x)+(x)+2)
#define DST3_2(x,y) *(p_dst[y]+(x)+(x)+(x)+2)

#define SRC4_0(x,y) *(p_src[y]+(x)+(x)+(x)+(x))
#define DST4_0(x,y) *(p_dst[y]+(x)+(x)+(x)+(x))
#define SRC4_1(x,y) *(p_src[y]+(x)+(x)+(x)+(x)+1)
#define DST4_1(x,y) *(p_dst[y]+(x)+(x)+(x)+(x)+1)
#define SRC4_2(x,y) *(p_src[y]+(x)+(x)+(x)+(x)+2)
#define DST4_2(x,y) *(p_dst[y]+(x)+(x)+(x)+(x)+2)
#define SRC4_3(x,y) *(p_src[y]+(x)+(x)+(x)+(x)+3)
#define DST4_3(x,y) *(p_dst[y]+(x)+(x)+(x)+(x)+3)

int imgLinearEquation(float *data,float *answer,int num);			
				
void imgAffine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{
	if((ps==NULL)&&(pd==NULL))
	{
		Resize(src,dst,0,0);
		return;
	}
	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAffine,src);
	DESTINATION_ERROR_CHECK(imgAffine,dst);
	if(ps == NULL)
	{
		printf("IMG Error\n\tin imgAffine:no source points.\n");
		exit(0);
	}
	if(pd == NULL)
	{
		printf("IMG Error\n\tin imgAffine:no destination points.\n");
		exit(0);
	}
	#endif
	
	int i,j;
	
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int dst_width;
	dst_width = dst->width;
	
	int dst_height;
	dst_height = dst->height;
	
	if(dst->type != src->type)
		imgMatRedefine(dst,dst_height,dst_width,src->type);
	
	unsigned char **p_src;
	SRC_PTR;
	
	unsigned char **p_dst;
	DST_PTR;
	
	float answer[3];
	
	float data1[12] =  {(float)pd[0].x,(float)pd[0].y,1.0,0.0-ps[0].x,
						(float)pd[1].x,(float)pd[1].y,1.0,0.0-ps[1].x,
						(float)pd[2].x,(float)pd[2].y,1.0,0.0-ps[2].x};	
	
	imgLinearEquation(data1,answer,3);
	
	float k1,k2,b1;
	k1 = answer[0];
	k2 = answer[1];
	b1 = answer[2];
	
	float data2[12] =  {(float)pd[0].x,(float)pd[0].y,1.0,0.0-ps[0].y,
						(float)pd[1].x,(float)pd[1].y,1.0,0.0-ps[1].y,
						(float)pd[2].x,(float)pd[2].y,1.0,0.0-ps[2].y};
						
	imgLinearEquation(data2,answer,3);
	
	float k3,k4,b2;
	k3 = answer[0];
	k4 = answer[1];
	b2 = answer[2];
	
	// printf("x = %f*x + %f*y + %f\n",k1,k2,b1);
	// printf("y = %f*x + %f*y + %f\n",k3,k4,b2);
	
	float x,y;
	int x1,y1,x2,y2;
	float wx,wy;
	float w1,w2,w3,w4;
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn == 1)
	{
		for(j=0;j<dst_height;j++)
		{
			x = (float)j*k2+b1;
			y = (float)j*k4+b2;
			
			for(i=0;i<dst_width;i++)
			{
				if((x>src_width)||(x<0)||(y>src_height)||(y<0))
				{
					DST(i,j) = 0;
					
					x = x+k1;
					y = y+k3;
					
					continue;
				}
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST(i,j) =(int)((float)SRC(x1,y1)*w1+(float)SRC(x2,y1)*w2+(float)SRC(x2,y2)*w3+(float)SRC(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}
	
	else if(cn == 3)
	{
		for(j=0;j<dst_height;j++)
		{
			x = (float)j*k2+b1;
			y = (float)j*k4+b2;
			
			for(i=0;i<dst_width;i++)
			{
				if((x>src_width)||(x<0)||(y>src_height)||(y<0))
				{
					DST(i,j) = 0;
					
					x = x+k1;
					y = y+k3;
					
					continue;
				}
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST3_0(i,j) =(int)((float)SRC3_0(x1,y1)*w1+(float)SRC3_0(x2,y1)*w2+(float)SRC3_0(x2,y2)*w3+(float)SRC3_0(x1,y2)*w4);
				DST3_1(i,j) =(int)((float)SRC3_1(x1,y1)*w1+(float)SRC3_1(x2,y1)*w2+(float)SRC3_1(x2,y2)*w3+(float)SRC3_1(x1,y2)*w4);
				DST3_2(i,j) =(int)((float)SRC3_2(x1,y1)*w1+(float)SRC3_2(x2,y1)*w2+(float)SRC3_2(x2,y2)*w3+(float)SRC3_2(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}	
	else if(cn == 4)
	{
		for(j=0;j<dst_height;j++)
		{
			x = (float)j*k2+b1;
			y = (float)j*k4+b2;
			
			for(i=0;i<dst_width;i++)
			{
				if((x>src_width)||(x<0)||(y>src_height)||(y<0))
				{
					DST(i,j) = 0;
					
					x = x+k1;
					y = y+k3;
					
					continue;
				}
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST4_0(i,j) =(int)((float)SRC4_0(x1,y1)*w1+(float)SRC4_0(x2,y1)*w2+(float)SRC4_0(x2,y2)*w3+(float)SRC4_0(x1,y2)*w4);
				DST4_1(i,j) =(int)((float)SRC4_1(x1,y1)*w1+(float)SRC4_1(x2,y1)*w2+(float)SRC4_1(x2,y2)*w3+(float)SRC4_1(x1,y2)*w4);
				DST4_2(i,j) =(int)((float)SRC4_2(x1,y1)*w1+(float)SRC4_2(x2,y1)*w2+(float)SRC4_2(x2,y2)*w3+(float)SRC4_2(x1,y2)*w4);
				DST4_3(i,j) =(int)((float)SRC4_3(x1,y1)*w1+(float)SRC4_3(x2,y1)*w2+(float)SRC4_3(x2,y2)*w3+(float)SRC4_3(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}
	
}

ImgMat *imgCreateMat(int height,int width,char type);

void Affine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAffine,src);
	#endif
	
	if(dst == NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgAffine(src,ps,dst,pd);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgAffine(src,ps,dst,pd);
}
