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

void imgResize(ImgMat *src,ImgMat *dst)
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
	
	float kx,ky;
	kx = (float)src_width/(float)dst_width;
	ky = (float)src_height/(float)dst_height;	
	
	int x1,y1,x2,y2;
	float w1,w2,w3,w4;
	
	float x,y;
	float wx,wy;
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	int i,j;	
	
	if(cn == 1)
	{
		for(j=0;j<dst_height;j++)
			for(i=0;i<dst_width;i++)
			{
				x = ((float)i)*kx;
				y = ((float)j)*ky;
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;
				
				w1 = (1.0-wx)*(1.0-wy);
				w2 = wx*(1.0-wy);
				w3 = (1.0-wx)*wy;
				w4 = wx*wy;
				
				DST(i,j) = SRC(x1,y1)*w1+SRC(x2,y1)*w2+SRC(x1,y2)*w3+SRC(x2,y2)*w4;
			}
	}
	else if(cn==3)
	{
		for(j=0;j<dst_height;j++)
			for(i=0;i<dst_width;i++)
			{
				x = ((float)i)*kx;
				y = ((float)j)*ky;
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;
				
				w1 = (1.0-wx)*(1.0-wy);
				w2 = wx*(1.0-wy);
				w3 = (1.0-wx)*wy;
				w4 = wx*wy;
				
				DST_0(i,j) = SRC_0(x1,y1)*w1+SRC_0(x2,y1)*w2+SRC_0(x1,y2)*w3+SRC_0(x2,y2)*w4;
				DST_1(i,j) = SRC_1(x1,y1)*w1+SRC_1(x2,y1)*w2+SRC_1(x1,y2)*w3+SRC_1(x2,y2)*w4;
				DST_2(i,j) = SRC_2(x1,y1)*w1+SRC_2(x2,y1)*w2+SRC_2(x1,y2)*w3+SRC_2(x2,y2)*w4;
			}
	}
}
/*
ImgMat *imgCreateMat(int height,int width,char type);

ImgMat *imgReshape(ImgMat *src,ImgPoint *vertex)
{
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int x_max;
	x_max = vertex[0].x;
	x_max = (vertex[1].x>x_max)?vertex[1].x:x_max;
	x_max = (vertex[2].x>x_max)?vertex[2].x:x_max;
	x_max = (vertex[3].x>x_max)?vertex[3].x:x_max;
	
	int x_min;
	x_min = vertex[0].x;
	x_min = (vertex[1].x<x_min)?vertex[1].x:x_min;
	x_min = (vertex[2].x<x_min)?vertex[2].x:x_min;
	x_min = (vertex[3].x<x_min)?vertex[3].x:x_min;
	
	int y_max;
	y_max = vertex[0].y;
	y_max = (vertex[1].y>y_max)?vertex[1].y:y_max;
	y_max = (vertex[2].y>y_max)?vertex[2].y:y_max;
	y_max = (vertex[3].y>y_max)?vertex[3].y:y_max;
	
	int y_min;
	y_min = vertex[0].y;
	y_min = (vertex[1].y<y_min)?vertex[1].y:y_min;
	y_min = (vertex[2].y<y_min)?vertex[2].y:y_min;
	y_min = (vertex[3].y<y_min)?vertex[3].y:y_min;
	
	vertex[0].x = vertex[0].x-x_min;
	vertex[0].y = vertex[0].y-y_min;
	vertex[1].x = vertex[1].x-x_min;
	vertex[1].y = vertex[1].y-y_min;
	vertex[2].x = vertex[2].x-x_min;
	vertex[2].y = vertex[2].y-y_min;
	vertex[3].x = vertex[3].x-x_min;
	vertex[3].y = vertex[3].y-y_min;	
	
	// printf("vertex[0] is (%d,%d)\n",vertex[0].x,vertex[0].y);
	// printf("vertex[1] is (%d,%d)\n",vertex[1].x,vertex[1].y);
	// printf("vertex[2] is (%d,%d)\n",vertex[2].x,vertex[2].y);
	// printf("vertex[3] is (%d,%d)\n",vertex[3].x,vertex[3].y);
	
	int dst_width;
	dst_width = x_max-x_min;
	
	int dst_height;
	dst_height = y_max-y_min;
	
	ImgMat *dst;
	dst = imgCreateMat(dst_height,dst_width,TYPE_8UC1);
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	// printf("src_width is %d\n",src_width);
	// printf("src_height is %d\n",src_height);
	// printf("dst_width is %d\n",dst_width);
	// printf("dst_height is %d\n",dst_height);
	
	float k1_x[3],k1_y[3],k2_x[3],k2_y[3];
	float k1[3],k2[3];
	
	float p1_x,p1_y,p2_x,p2_y;
	float t1,t2;
	float h;
	if(vertex[0].y==0)
	{
		p1_x = 0;
		p1_y = 0;
		p2_x = p1_x;
		p2_y = p1_y;		
		
		h = (float)(vertex[3].y-vertex[0].y);
		if(h == 0.0)
			h = 1.0;
		k1_x[0] = 0.0;
		k1_y[0] = (float)src_height/h;
		k1[0] = (float)(vertex[3].x-vertex[0].x)/h;
		
		h = (float)(vertex[2].y-vertex[3].y);
		if(h == 0.0)
			h = 1.0;			
		k1_x[1] = (float)src_width/h;
		k1_y[1] = 0.0;
		k1[1] = (float)(vertex[2].x-vertex[3].x)/h;	

		h = (float)(vertex[1].y-vertex[2].y);
		if(h == 0.0)
			h = -1.0;
		k1_x[2] = 0.0;
		k1_y[2] = 0.0-(float)src_height/h;	
		k1[2] = (float)(vertex[1].x-vertex[2].x)/h;		
		
		h = (float)(vertex[1].y-vertex[0].y);
		if(h == 0.0)
			h = 1.0;
		k2_x[0] = (float)src_width/h;
		k2_y[0] = 0.0;
		k2[0] = (float)(vertex[1].x-vertex[0].x)/h;
		
		h = (float)(vertex[2].y-vertex[1].y);
		if(h == 0.0)
			h = 1.0;
		k2_x[1] = 0.0;
		k2_y[1] = (float)src_height/h;
		k2[1] = (float)(vertex[2].x-vertex[1].x)/h;
		
		h = (float)(vertex[3].y-vertex[2].y);
		if(h == 0.0)
			h = -1.0;
		k2_x[2] = 0.0-(float)src_width/h;
		k2_y[2] = 0.0;			
		k2[2] = (float)(vertex[3].x-vertex[2].x)/h;
		
		t1 = vertex[0].x;
		t2 = t1;
	}
	else if(vertex[1].y==0)
	{		
		p1_x = src_width;
		p1_y = 0;
		p2_x = p1_x;
		p2_y = p1_y;
		
		h = (float)(vertex[0].y-vertex[1].y);
		if(h == 0.0)
			h = -1.0;
		k1_x[0] = 0.0-(float)src_width/h;
		k1_y[0] = 0.0;
		k1[0] = (float)(vertex[0].x-vertex[1].x)/h;
		
		h = (float)(vertex[3].y-vertex[0].y);
		if(h == 0.0)
			h = 1.0;
		k1_x[1] = 0.0;
		k1_y[1] = (float)src_height/h;
		k1[1] = (float)(vertex[3].x-vertex[0].x)/h;
		
		h = (float)(vertex[2].y-vertex[3].y);
		if(h == 0.0)
			h = 1.0;
		k1_x[2] = (float)src_width/h;
		k1_y[2] = 0.0;
		k1[2] = (float)(vertex[2].x-vertex[3].x)/h;
		
		h = (float)(vertex[2].y-vertex[1].y);
		if(h == 0.0)
			h = 1.0;
		k2_x[0] = 0.0;
		k2_y[0] = (float)src_height/h;
		k2[0] = (float)(vertex[2].x-vertex[1].x)/h;
		
		h = (float)(vertex[3].y-vertex[2].y);
		if(h == 0.0)
			h = -1.0;
		k2_x[1] = 0.0-(float)src_width/h;
		k2_y[1] = 0.0;			
		k2[1] = (float)(vertex[3].x-vertex[2].x)/h;
		
		h = (float)(vertex[0].y-vertex[3].y);
		if(h == 0.0)
			h = -1.0;
		k2_x[2] = 0.0;
		k2_y[2] = 0.0-(float)src_height/h;
		k2[2] = (float)(vertex[0].x-vertex[3].x)/h;
				
		t1 = vertex[1].x;
		t2 = t1;
	}
	else if(vertex[2].y==0)
	{
		p1_x = src_width;
		p1_y = src_height;
		p2_x = p1_x;
		p2_y = p1_y;
		
		h = (float)(vertex[1].y-vertex[2].y);
		if(h == 0.0)
			h = -1.0;
		k1_x[0] = 0.0;
		k1_y[0] = 0.0-(float)src_height/h;	
		k1[0] = (float)(vertex[1].x-vertex[2].x)/h;	
		
		h = (float)(vertex[0].y-vertex[1].y);
		if(h == 0.0)
			h = -1.0;
		k1_x[1] = 0.0-(float)src_width/h;
		k1_y[1] = 0.0;
		k1[1] = (float)(vertex[0].x-vertex[1].x)/h;
		
		h = (float)(vertex[3].y-vertex[0].y);
		if(h == 0.0)
			h = 1.0;
		k1_x[2] = 0.0;
		k1_y[2] = (float)src_height/h;
		k1[2] = (float)(vertex[3].x-vertex[0].x)/h;
		
		h = (float)(vertex[3].y-vertex[2].y);
		if(h == 0.0)
			h = -1.0;
		k2_x[0] = 0.0-(float)src_width/h;
		k2_y[0] = 0.0;			
		k2[0] = (float)(vertex[3].x-vertex[2].x)/h;
		
		h = (float)(vertex[0].y-vertex[3].y);
		if(h == 0.0)
			h = -1.0;
		k2_x[1] = 0.0;
		k2_y[1] = 0.0-(float)src_height/h;
		k2[1] = (float)(vertex[0].x-vertex[3].x)/h;
		
		h = (float)(vertex[1].y-vertex[0].y);
		if(h == 0.0)
			h = 1.0;
		k2_x[2] = (float)src_width/h;
		k2_y[2] = 0.0;
		k2[2] = (float)(vertex[1].x-vertex[0].x)/h;
		
		t1 = vertex[2].x;
		t2 = t1;
	}
	else if(vertex[3].y==0)
	{
		p1_x = 0;
		p1_y = src_height;
		p2_x = p1_x;
		p2_y = p1_y;
		
		h = (float)(vertex[2].y-vertex[3].y);
		if(h == 0.0)
			h = 1.0;
		k1_x[0] = (float)src_width/h;
		k1_y[0] = 0.0;
		k1[0] = (float)(vertex[2].x-vertex[3].x)/h;
		
		h = (float)(vertex[1].y-vertex[2].y);
		if(h == 0.0)
			h = -1.0;
		k1_x[1] = 0.0;
		k1_y[1] = 0.0-(float)src_height/h;	
		k1[1] = (float)(vertex[1].x-vertex[2].x)/h;	
		
		h = (float)(vertex[0].y-vertex[1].y);
		if(h == 0.0)
			h = -1.0;
		k1_x[2] = 0.0-(float)src_width/h;
		k1_y[2] = 0.0;
		k1[2] = (float)(vertex[0].x-vertex[1].x)/h;
		
		h = (float)(vertex[0].y-vertex[3].y);
		if(h == 0.0)
			h = -1.0;
		k2_x[0] = 0.0;
		k2_y[0] = 0.0-(float)src_height/h;
		k2[0] = (float)(vertex[0].x-vertex[3].x)/h;
		
		h = (float)(vertex[1].y-vertex[0].y);
		if(h == 0.0)
			h = 1.0;
		k2_x[1] = (float)src_width/h;
		k2_y[1] = 0.0;
		k2[1] = (float)(vertex[1].x-vertex[0].x)/h;
		
		h = (float)(vertex[2].y-vertex[1].y);
		if(h == 0.0)
			h = 1.0;
		k2_x[2] = 0.0;
		k2_y[2] = (float)src_height/h;
		k2[2] = (float)(vertex[2].x-vertex[1].x)/h;
		
		t1 = vertex[3].x;
		t2 = t1;
	}
	
	// printf("k1_x is %f,%f,%f\n",k1_x[0],k1_x[1],k1_x[2]);
	// printf("k1_y is %f,%f,%f\n",k1_y[0],k1_y[1],k1_y[2]);
	// printf("k2_x is %f,%f,%f\n",k2_x[0],k2_x[1],k2_x[2]);
	// printf("k2_y is %f,%f,%f\n",k2_y[0],k2_y[1],k2_y[2]);
	
	
	float x,y;
	int x1,y1,x2,y2;	
	float wx,wy;
	float w1,w2,w3,w4;
	
	float w;
	float k_wx,k_wy;
	
	int i,j;
	for(j=0;j<dst_height;j++)
	{	
		// if(j<420)
		// {
			// printf("%d:  t1 is %f  t2 is %f\t",j,t1,t2);
			// printf("p1 is (%f,%f),  p2 is (%f,%f)\n",p1_x,p1_y,p2_x,p2_y);
		// }
		
		if(t2>=t1)
		{
			w = (t2-t1);
			k_wx = (p2_x-p1_x)/w;
			k_wy = (p2_y-p1_y)/w;
			
			x = p1_x;
			y = p1_y;
		}
		else
		{
			w = (t1-t2);
			k_wx = (p1_x-p2_x)/w;
			k_wy = (p1_y-p2_y)/w;
			
			x = p2_x;
			y = p2_y;
		}
			
		
		for(i=0;i<dst_width;i++)
		{
			if((t1<=t2)&&((i<t1)||(i>t2)))
				DST(i,j) = 0;
			else if((t2<=t1)&&((i<t2)||(i>t1)))
				DST(i,j) = 0;
			else
			{				
				// if(j==400)
					// printf("(%f,%f),",x,y);
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
				
				x = x+k_wx;
				y = y+k_wy;
			}
		}	
		
		p1_x = p1_x+k1_x[0];
		p1_y = p1_y+k1_y[0];
		p2_x = p2_x+k2_x[0];
		p2_y = p2_y+k2_y[0];	

		
		if((p1_x>src_width)||(p1_x<0)||(p1_y>src_height)||(p1_y<0))
		{
			p1_x = p1_x-k1_x[0];
			p1_y = p1_y-k1_y[0];
			
			k1_x[0] = k1_x[1];
			k1_x[1] = k1_x[2];
			k1_y[0] = k1_y[1];
			k1_y[1] = k1_y[2];
			
			k1[0] = k1[1];
			k1[1] = k1[2];
			
			p1_x = p1_x+k1_x[0];
			p1_y = p1_y+k1_y[0];
			
			// printf("k1_x is %f\n",k1_x[0]);
			// printf("k1_y is %f\n",k1_y[0]);
		}
		
		if((p2_x>src_width)||(p2_x<0)||(p2_y>src_height)||(p2_y<0))
		{
			p2_x = p2_x-k2_x[0];
			p2_y = p2_y-k2_y[0];
		
			k2_x[0] = k2_x[1];
			k2_x[1] = k2_x[2];
			k2_y[0] = k2_y[1];
			k2_y[1] = k2_y[2];
			
			k2[0] = k2[1];
			k2[1] = k2[2];
			
			p2_x = p2_x+k2_x[0];
			p2_y = p2_y+k2_y[0];
			
			// printf("k2_x is %f\n",k2_x[0]);
			// printf("k2_y is %f\n",k2_y[0]);
		}
		
		t1 = t1+k1[0];
		t2 = t2+k2[0];
	}

	return dst;
}

void imgRectMap(ImgMat *src,ImgPoint *vertex,ImgMat *dst)
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
	
	float k1_x,k1_y,k2_x,k2_y;
	k1_x = (float)(vertex[3].x - vertex[0].x)/(float)dst_height;
	k1_y = (float)(vertex[3].y - vertex[0].y)/(float)dst_height;
	k2_x = (float)(vertex[2].x - vertex[1].x)/(float)dst_height;
	k2_y = (float)(vertex[2].y - vertex[1].y)/(float)dst_height;
	
	float p1_x,p1_y,p2_x,p2_y;
	p1_x = (float)vertex[0].x;
	p1_y = (float)vertex[0].y;
	p2_x = (float)vertex[1].x;
	p2_y = (float)vertex[1].y;
	
	float k_x,k_y;
	float x,y;
	int x1,y1,x2,y2;
	float wx,wy;
	float w1,w2,w3,w4;
	
	int i,j;
	for(j=0;j<dst_height;j++)
	{
		k_x = (p2_x-p1_x)/(float)dst_width;
		k_y = (p2_y-p1_y)/(float)dst_width;
		
		x = p1_x;
		y = p1_y;
		
		for(i=0;i<dst_width;i++)
		{
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
			
			x = x+k_x;
			y = y+k_y;
		}
		
		p1_x = p1_x+k1_x;
		p1_y = p1_y+k1_y;
		p2_x = p2_x+k2_x;
		p2_y = p2_y+k2_y;
	}
}
*/
	

	
				
				
/*
ImgMat *imgReShape(ImgMat *src,ImgPoint *vertex)
{
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int x_max;
	x_max = vertex[0].x;
	x_max = (vertex[1].x>x_max)?vertex[1].x:x_max;
	x_max = (vertex[2].x>x_max)?vertex[2].x:x_max;
	x_max = (vertex[3].x>x_max)?vertex[3].x:x_max;
	
	int x_min;
	x_min = vertex[0].x;
	x_min = (vertex[1].x<x_min)?vertex[1].x:x_min;
	x_min = (vertex[2].x<x_min)?vertex[2].x:x_min;
	x_min = (vertex[3].x<x_min)?vertex[3].x:x_min;
	
	int y_max;
	y_max = vertex[0].y;
	y_max = (vertex[1].y>y_max)?vertex[1].y:y_max;
	y_max = (vertex[2].y>y_max)?vertex[2].y:y_max;
	y_max = (vertex[3].y>y_max)?vertex[3].y:y_max;
	
	int y_min;
	y_min = vertex[0].y;
	y_min = (vertex[1].y<y_min)?vertex[1].y:y_min;
	y_min = (vertex[2].y<y_min)?vertex[2].y:y_min;
	y_min = (vertex[3].y<y_min)?vertex[3].y:y_min;
	
	vertex[0].x = vertex[0].x-x_min;
	vertex[0].y = vertex[0].y-y_min;
	vertex[1].x = vertex[1].x-x_min;
	vertex[1].y = vertex[1].y-y_min;
	vertex[2].x = vertex[2].x-x_min;
	vertex[2].y = vertex[2].y-y_min;
	vertex[3].x = vertex[3].x-x_min;
	vertex[3].y = vertex[3].y-y_min;	
	
	int dst_width;
	dst_width = x_max-x_min;
	
	int dst_height;
	dst_height = y_max-y_min;
	

	
	ImgMat *dst;
	dst = imgCreateMat(dst_height,dst_width,TYPE_8UC1);
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	

	float h0;
	// h0 = ABS(vertex[0].x-vertex[3].x)+ABS(vertex[0].y-vertex[3].y);
	
	h0 = (float)(sqrt((double)((vertex[0].x-vertex[3].x)*(vertex[0].x-vertex[3].x)+(vertex[0].y-vertex[3].y)*(vertex[0].y-vertex[3].y))));
	
	float h1;
	// h0 = ABS(vertex[1].x-vertex[2].x)+ABS(vertex[1].y-vertex[2].y);
	
	h1 = (float)(sqrt((double)((vertex[1].x-vertex[2].x)*(vertex[1].x-vertex[2].x)+(vertex[1].y-vertex[2].y)*(vertex[1].y-vertex[2].y))));
	
	float w,h;
	h = (h0>h1)?h0:h1;
	h=h*1.3;
	
	printf("h0 is %f\n",h0);
	printf("h1 is %f\n",h1);
	printf("h is %f\n",h);
	
	float s_h0x,s_h1x,s_h0y,s_h1y;
	s_h0x = (float)(vertex[3].x-vertex[0].x)/h;
	s_h0y = (float)(vertex[3].y-vertex[0].y)/h;
	s_h1x = (float)(vertex[2].x-vertex[1].x)/h;	
	s_h1y = (float)(vertex[2].y-vertex[1].y)/h;
	
	float s_wx,s_wy;
	
	int lx,ly;
	float wx,wy;
	float w1,w2,w3,w4;
	
	float x,y;
	
	float p0_x,p0_y,p1_x,p1_y;
	p0_x = (float)vertex[0].x-s_h0x;
	p0_y = (float)vertex[0].y-s_h0y;
	p1_x = (float)vertex[1].x-s_h1x;
	p1_y = (float)vertex[1].y-s_h1y;
	
	float kx,ky;
	ky = (float)src_height/h;
	
	int i,j;
	i=0;
	j=0;	
	for(j=0;j<h;j++)
	// while((p0_x<vertex[3].x)&&(p0_y<vertex[3].y)&&(p1_x<vertex[2].x)&&(p1_y<vertex[2].y))
	{		
		p0_x = p0_x+s_h0x;
		p0_y = p0_y+s_h0y;
		p1_x = p1_x+s_h1x;
		p1_y = p1_y+s_h1y;
		
		
		
		w = sqrt((float)((p0_x-p1_x)*(p0_x-p1_x)+(p0_y-p1_y)*(p0_y-p1_y)));
		w = w*1.3;
		// w = (ABS(p1_x-p0_x)+ABS(p1_y-p0_y));
		s_wx = (p1_x-p0_x)/w;
		s_wy = (p1_y-p0_y)/w;
		
		
		 
		x = p0_x-s_wx;
		y = p0_y-s_wy;	

		ly = (int)(j*ky);
		wy = j*ky-(float)ly;
		
		kx = (float)src_width/w;
		
		if(j==0)
		{
			printf("p0 is (%f,%f)\n",p0_x,p0_y);
			printf("p1 is (%f,%f)\n",p1_x,p1_y);
			printf("w is %f\n",w);
			printf("ly is %d\n",ly);
		}		
		
		for(i=0;i<w;i++)
		// while((x<p1_x)&&(y<p1_y))
		{
			x = x+s_wx;
			y = y+s_wy;
			
			lx = (int)(i*kx);
			wx = (float)i*kx-(float)lx;
			
			
			
			w1 = (1-wx)*(1-wy);
			w2 = wx*(1-wy);
			w3 = wx*wy;
			w4 = (1-wx)*wy;
			
			// printf("SRC(419,12) is %d\n",SRC(419,12));	
			
			DST((int)(x+0.5),(int)(y+0.5)) = SRC(lx,ly)*w1+SRC(lx+1,ly)*w2+SRC(lx+1,ly+1)*w3+SRC(lx,ly+1)*w4;
			
			int xx,yy;
			xx = (int)(x+0.5);
			yy = (int)(y+0.5);
			if(xx == 174)
			{
				printf("yy is %d\t",yy);
				printf("x is %f\ty is %f\n",x,y);
			}
			
			
			// if(j==10)
			// {
				// printf("(%d,%d)\t",lx,ly);
				// printf("(%d,%d)\t",(int)(x+0.5),(int)(y+0.5));
				// printf("w is %f,%f,%f,%f\t",w1,w2,w3,w4);
				// printf("src is %d,%d,%d,%d\t",SRC(lx,ly),SRC(lx+1,ly),SRC(lx+1,ly+1),SRC(lx,ly+1));
				// printf("dst is %d\n",DST((int)(x+0.5),(int)(y+0.5)));
			// }
		}
		
	}
	
	return dst;
}
*/	
