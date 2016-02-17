#include <stdio.h>
#include "type.h"

#define SRC(a,b) p_src[(b)*src_width+(a)]
#define DST(a,b) p_dst[(b)*dst_width+(a)]
#define SRC_0(a,b) p_src[(b)*src_width*cn+(a)*cn]
#define DST_0(a,b) p_dst[(b)*dst_width*cn+(a)*cn]
#define SRC_1(a,b) p_src[(b)*src_width*cn+(a)*cn+1]
#define DST_1(a,b) p_dst[(b)*dst_width*cn+(a)*cn+1]
#define SRC_2(a,b) p_src[(b)*src_width*cn+(a)*cn+2]
#define DST_2(a,b) p_dst[(b)*dst_width*cn+(a)*cn+2]

int imgLinearEquation(float *data,float *answer,int num);			
				
void imgAffine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int dst_width;
	dst_width = dst->width;
	
	int dst_height;
	dst_height = dst->height;
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
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
	
	int i,j;
	
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
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
				DST_2(i,j) =(int)((float)SRC_2(x1,y1)*w1+(float)SRC_2(x2,y1)*w2+(float)SRC_2(x2,y2)*w3+(float)SRC_2(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}	
}
