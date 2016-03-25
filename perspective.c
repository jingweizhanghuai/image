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

void imgPerspective(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{
	if((ps==NULL)&&(pd==NULL))
	{
		Resize(src,dst,0,0);
		return;
	}
	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgPerspective,src);
	DESTINATION_ERROR_CHECK(imgPerspective,dst);
	#endif
	
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
	
	int i,j;
	
	float w = (float)dst_width;
	float h = (float)dst_height;
	
	float x[4],y[4];
	if(ps==NULL)
	{
		x[0] = 0.0;
		y[0] = 0.0;
		x[1] = src->width-1;
		y[1] = 0.0;
		x[2] = 0.0;
		y[2] = src->height-1;
		x[3] = src->width-1;
		y[3] = src->height-1;
	}
	else
	{
		x[0] = (float)ps[0].x;
		x[1] = (float)ps[1].x;
		x[2] = (float)ps[2].x;
		x[3] = (float)ps[3].x;
		y[0] = (float)ps[0].y;
		y[1] = (float)ps[1].y;
		y[2] = (float)ps[2].y;
		y[3] = (float)ps[3].y;
	}
	
	float m[4],n[4];
	if(pd!=NULL)
	{
		m[0] = (float)pd[0].x;
		m[1] = (float)pd[1].x;
		m[2] = (float)pd[2].x;
		m[3] = (float)pd[3].x;
		n[0] = (float)pd[0].y;
		n[1] = (float)pd[1].y;
		n[2] = (float)pd[2].y;
		n[3] = (float)pd[3].y;
	}
	
	unsigned char **p_src;
	SRC_PTR;
	
	unsigned char **p_dst;
	DST_PTR;
	
	float answer[8];
	
	if(pd==NULL)
	{
		//				  a11,	a12,a21,a22,a31,		a32,		b
		float data1[42] ={w,	0,	0,	0,	0-x[1]*w,	0,			x[0]-x[1],
						  0,	0,	w,	0,	0-y[1]*w,	0,			y[0]-y[1],
						  w,	h,	0,	0,	0-x[2]*w,	0-x[2]*h,	x[0]-x[2],
						  0,	0,	w,	h,	0-y[2]*w,	0-y[2]*h,	y[0]-y[2],
						  0,	h,	0,	0,	0,			0-x[3]*h,	x[0]-x[3],
						  0,	0,	0,	h,	0,			0-y[3]*h,	y[0]-y[3]};

		if(imgLinearEquation(data1,answer,6)==0)
		{
			printf("IMG Error\n\tin imgPerspective:points given is wrong.\n");
			exit(0);
		}
		answer[6] = x[0];
		answer[7] = y[0];
	}
	else
	{
		float data2[72] = {	m[0],	n[0],	0,		0,		0-m[0]*x[0],	0-n[0]*x[0],	1,	0,	0-x[0],
							0,		0,		m[0],	n[0],	0-m[0]*y[0],	0-n[0]*y[0],	0,	1,	0-y[0],
							m[1],	n[1],	0,		0,		0-m[1]*x[1],	0-n[1]*x[1],	1,	0,	0-x[1],
							0,		0,		m[1],	n[1],	0-m[1]*y[1],	0-n[1]*y[1],	0,	1,	0-y[1],
							m[2],	n[2],	0,		0,		0-m[2]*x[2],	0-n[2]*x[2],	1,	0,	0-x[2],
							0,		0,		m[2],	n[2],	0-m[2]*y[2],	0-n[2]*y[2],	0,	1,	0-y[2],
							m[3],	n[3],	0,		0,		0-m[3]*x[3],	0-n[3]*x[3],	1,	0,	0-x[3],
							0,		0,		m[3],	n[3],	0-m[3]*y[3],	0-n[3]*y[3],	0,	1,	0-y[3]};
				
		if(imgLinearEquation(data2,answer,8)==0)
		{
			printf("IMG Error\n\tin imgPerspective:points given is wrong.\n");
			exit(0);
		}
	}		
			
	// printf("a11 is %f\n",answer[0]);
	// printf("a12 is %f\n",answer[1]);
	// printf("a13 is %f\n",answer[6]);
	// printf("a21 is %f\n",answer[2]);
	// printf("a22 is %f\n",answer[3]);
	// printf("a23 is %f\n",answer[7]);
	// printf("a31 is %f\n",answer[4]);
	// printf("a32 is %f\n",answer[5]);
	
	float u,v;
	int x1,y1,x2,y2;
	float wx,wy;
	float w1,w2,w3,w4;
	float u_1,u_2,v_1,v_2;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	if(cn==1)
	{
		for(j=0;j<dst_height;j++)
		{
			u_1 = (answer[1]*(float)j+answer[6]);
			u_2 = (answer[5]*(float)j+1.0);
			v_1 = (answer[3]*(float)j+answer[7]);
			v_2 = (answer[5]*(float)j+1.0);
			
			for(i=0;i<dst_width;i++)
			{
				// u= (answer[0]*(float)i+answer[1]*(float)j+answer[6])/(answer[4]*(float)i+answer[5]*(float)j+1.0);
				// v= (answer[2]*(float)i+answer[3]*(float)j+answer[7])/(answer[4]*(float)i+answer[5]*(float)j+1.0);
				
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
	else if(cn==3)
	{
		for(j=0;j<dst_height;j++)
		{
			u_1 = (answer[1]*(float)j+answer[6]);
			u_2 = (answer[5]*(float)j+1.0);
			v_1 = (answer[3]*(float)j+answer[7]);
			v_2 = (answer[5]*(float)j+1.0);
			
			for(i=0;i<dst_width;i++)
			{
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
				
				DST3_0(i,j) =(int)((float)SRC3_0(x1,y1)*w1+(float)SRC3_0(x2,y1)*w2+(float)SRC3_0(x2,y2)*w3+(float)SRC3_0(x1,y2)*w4);
				DST3_1(i,j) =(int)((float)SRC3_1(x1,y1)*w1+(float)SRC3_1(x2,y1)*w2+(float)SRC3_1(x2,y2)*w3+(float)SRC3_1(x1,y2)*w4);
				DST3_2(i,j) =(int)((float)SRC3_2(x1,y1)*w1+(float)SRC3_2(x2,y1)*w2+(float)SRC3_2(x2,y2)*w3+(float)SRC3_2(x1,y2)*w4);
				
				u_1 = u_1+answer[0];
				u_2 = u_2+answer[4];
				v_1 = v_1+answer[2];
				v_2 = v_2+answer[4];
			}
		}
	}
	else if(cn==4)
	{
		for(j=0;j<dst_height;j++)
		{
			u_1 = (answer[1]*(float)j+answer[6]);
			u_2 = (answer[5]*(float)j+1.0);
			v_1 = (answer[3]*(float)j+answer[7]);
			v_2 = (answer[5]*(float)j+1.0);
			
			for(i=0;i<dst_width;i++)
			{
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
				
				DST4_0(i,j) =(int)((float)SRC4_0(x1,y1)*w1+(float)SRC4_0(x2,y1)*w2+(float)SRC4_0(x2,y2)*w3+(float)SRC4_0(x1,y2)*w4);
				DST4_1(i,j) =(int)((float)SRC4_1(x1,y1)*w1+(float)SRC4_1(x2,y1)*w2+(float)SRC4_1(x2,y2)*w3+(float)SRC4_1(x1,y2)*w4);
				DST4_2(i,j) =(int)((float)SRC4_2(x1,y1)*w1+(float)SRC4_2(x2,y1)*w2+(float)SRC4_2(x2,y2)*w3+(float)SRC4_2(x1,y2)*w4);
				DST4_3(i,j) =(int)((float)SRC4_3(x1,y1)*w1+(float)SRC4_3(x2,y1)*w2+(float)SRC4_3(x2,y2)*w3+(float)SRC4_3(x1,y2)*w4);
				
				u_1 = u_1+answer[0];
				u_2 = u_2+answer[4];
				v_1 = v_1+answer[2];
				v_2 = v_2+answer[4];
			}
		}
	}	
	free(p_src);
	free(p_dst);
}

ImgMat *imgCreateMat(int height,int width,char type);

void Perspective(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgPerspective,src);
	#endif
	
	if(dst == NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgPerspective(src,ps,dst,pd);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgPerspective(src,ps,dst,pd);
}
