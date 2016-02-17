#include <stdio.h>
#include "type.h"

#define SRC(a,b) p_src[(b)*src_width+(a)]
#define DST(a,b) p_dst[(b)*dst_width+(a)]

#define SRC_0(a,b) p_src[((b)*src_width+(a))*cn]
#define DST_0(a,b) p_dst[((b)*dst_width+(a))*cn]
#define SRC_1(a,b) p_src[((b)*src_width+(a))*cn+1]
#define DST_1(a,b) p_dst[((b)*dst_width+(a))*cn+1]
#define SRC_2(a,b) p_src[((b)*src_width+(a))*cn+2]
#define DST_2(a,b) p_dst[((b)*dst_width+(a))*cn+2]

int imgLinearEquation(float *data,float *answer,int num);

void imgPerspective(ImgMat *src,ImgPoint *vertex,ImgMat *dst)
{
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int dst_width;
	dst_width = dst->width;
	
	int dst_height;
	dst_height = dst->height;
	
	float w = (float)dst_width;
	float h = (float)dst_height;
	
	float x[4],y[4];
	x[0] = (float)vertex[0].x;
	x[1] = (float)vertex[1].x;
	x[2] = (float)vertex[2].x;
	x[3] = (float)vertex[3].x;
	y[0] = (float)vertex[0].y;
	y[1] = (float)vertex[1].y;
	y[2] = (float)vertex[2].y;
	y[3] = (float)vertex[3].y;
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	float answer[6];
	
	//				  a11,	a12,a21,a22,a31,		a32,		b
	float data[72] = {w,	0,	0,	0,	0-x[1]*w,	0,			x[0]-x[1],
					  0,	0,	w,	0,	0-y[1]*w,	0,			y[0]-y[1],
					  w,	h,	0,	0,	0-x[2]*w,	0-x[2]*h,	x[0]-x[2],
					  0,	0,	w,	h,	0-y[2]*w,	0-y[2]*h,	y[0]-y[2],
					  0,	h,	0,	0,	0,			0-x[3]*h,	x[0]-x[3],
					  0,	0,	0,	h,	0,			0-y[3]*h,	y[0]-y[3]};

	imgLinearEquation(data,answer,6);
	
	// printf("a11 is %f\n",answer[0]);
	// printf("a12 is %f\n",answer[1]);
	// printf("a13 is %f\n",x[0]);
	// printf("a21 is %f\n",answer[2]);
	// printf("a22 is %f\n",answer[3]);
	// printf("a23 is %f\n",y[0]);
	// printf("a31 is %f\n",answer[4]);
	// printf("a32 is %f\n",answer[5]);
	
	float u,v;
	int x1,y1,x2,y2;
	float wx,wy;
	float w1,w2,w3,w4;
	float u_1,u_2,v_1,v_2;
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;

	int i,j;
	
	if(cn == 1)
	{
		for(j=0;j<dst_height;j++)
		{
			u_1 = (answer[1]*(float)j+x[0]);
			u_2 = (answer[5]*(float)j+1.0);
			v_1 = (answer[3]*(float)j+y[0]);
			v_2 = (answer[5]*(float)j+1.0);
			
			for(i=0;i<dst_width;i++)
			{
				// u= (answer[0]*(float)i+answer[1]*(float)j+x[0])/(answer[4]*(float)i+answer[5]*(float)j+1.0);
				// v= (answer[2]*(float)i+answer[3]*(float)j+y[0])/(answer[4]*(float)i+answer[5]*(float)j+1.0);
				
				u = u_1/u_2;
				v = v_1/v_2;
		
				x1 = (int)u;
				x2 = x1+1;
				y1 = (int)v;
				y2 = y1+1;
				
				wx = u-(float)x1;
				wy = v-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST(i,j) =(int)((float)SRC(x1,y1)*w1+(float)SRC(x2,y1)*w2+(float)SRC(x2,y2)*w3+(float)SRC(x1,y2)*w4);
				
				u_1 = u_1+answer[0];
				u_2 = u_2+answer[4];
				v_1 = v_1+answer[2];
				v_2 = v_2+answer[4];
			}
		}
	}
	else if(cn == 3)
	{
		for(j=0;j<dst_height;j++)
		{
			u_1 = (answer[1]*(float)j+x[0]);
			u_2 = (answer[5]*(float)j+1.0);
			v_1 = (answer[3]*(float)j+y[0]);
			v_2 = (answer[5]*(float)j+1.0);
			
			for(i=0;i<dst_width;i++)
			{
				// u= (answer[0]*(float)i+answer[1]*(float)j+x[0])/(answer[4]*(float)i+answer[5]*(float)j+1.0);
				// v= (answer[2]*(float)i+answer[3]*(float)j+y[0])/(answer[4]*(float)i+answer[5]*(float)j+1.0);
				
				u = u_1/u_2;
				v = v_1/v_2;
		
				x1 = (int)u;
				x2 = x1+1;
				y1 = (int)v;
				y2 = y1+1;
				
				wx = u-(float)x1;
				wy = v-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
				DST_2(i,j) =(int)((float)SRC_2(x1,y1)*w1+(float)SRC_2(x2,y1)*w2+(float)SRC_2(x2,y2)*w3+(float)SRC_2(x1,y2)*w4);
				
				u_1 = u_1+answer[0];
				u_2 = u_2+answer[4];
				v_1 = v_1+answer[2];
				v_2 = v_2+answer[4];
			}
		}
	}
}
