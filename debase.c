#include <stdio.h>
#include "type.h"
#include "err.h"

#define PTR(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	if(cn>1)\
	{\
		p1##mat = malloc((mat->height)<<2);\
		for(i=0;i<mat->height;i++)\
			p1##mat[i] = p0##mat[i]+mat->size;\
	}\
	if(cn>2)\
	{\
		p2##mat = malloc((mat->height)<<2);\
		for(i=0;i<mat->height;i++)\
			p2##mat[i] = p1##mat[i]+mat->size;\
	}\
	if(cn>3)\
	{\
		p3##mat = malloc((mat->height)<<2);\
		for(i=0;i<mat->height;i++)\
			p3##mat[i] = p2##mat[i]+mat->size;\
	}\
}\

#define SRC_0(x,y) *(p0src[y]+(x))
#define DST_0(x,y) *(p0dst[y]+(x))
#define SRC_1(x,y) *(p1src[y]+(x))
#define DST_1(x,y) *(p1dst[y]+(x))
#define SRC_2(x,y) *(p2src[y]+(x))
#define DST_2(x,y) *(p2dst[y]+(x))
#define SRC_3(x,y) *(p3src[y]+(x))
#define DST_3(x,y) *(p3dst[y]+(x))

#define PTR_FREE(mat) {\
	free(p0##mat);\
	if(cn>1)\
		free(p1##mat);\
	if(cn>2)\
		free(p2##mat);\
	if(cn>3)\
		free(p3##mat);\
}\
	
void imgDebase(ImgMat *src,ImgMat *dst,int height,int width,int r)
{
#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgDebase,src);
	DESTINATION_ERROR_CHECK(imgDebase,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgDebase:\n");
		exit(0);
	}	
#endif

	if(r==0)
	{
		imgResize(src,dst,height,width);
		return;
	}
	
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int dst_width;
	if(width!=0)	
		dst_width = width;
	else
		dst_width = dst->width;
	
	int dst_height;
	if(height!=0)
		dst_height = height;
	else
		dst_height = dst->height;
	
	if((dst_width != dst->width)||(dst_height != dst->height)||(dst->type != src->type))
		imgMatRedefine(dst,dst_height,dst_width,src->type);
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	int i,j;	
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	unsigned char **p3src;
	PTR(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	unsigned char **p3dst;
	PTR(dst);
	
	float kx,ky;
	kx = (float)src_width/(float)dst_width;
	ky = (float)src_height/(float)dst_height;	
	
	int x0,y0,x1,y1,x2,y2;
	float w1,w2,w3,w4;
	int lx,ly;
	
	float x,y;
	float wx,wy;
	float w;
	
	float sum_0,sum_1,sum_2,sum_3;
	
	float area;
	area = (float)((r<<1)+1);
	area = area*area;
	
	if(cn == 1)
	{
		y=0.0;
		for(j=0;j<r;j++)
		{
			y0 = (int)(y+0.5);			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<dst_width;i++)
			{
				x0 = (int)(x+0.5);				
				x = x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
			}
		}
		for(j=r;j<dst_height-r;j++)
		{
			y0 = (int)y;	
			
			w4 = y-(float)y0;
			w3 = (1.0-w4);
			
			y1 = (int)y-r;
			y2 = (int)y+r+1;
			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<r;i++)
			{
				x0 = (int)(x+0.5);
				x = x+kx;
			
				DST_0(i,j) = SRC_0(x0,y0);
			}			
			
			for(i=r;i<dst_width-r;i++)
			{
				x0 = (int)x;				
				
				w2 = x-(float)x0;								
				w1 = (1.0-w2);				
				
				x1 = (int)x-r;
				x2 = (int)x+r+1;	
				
				x = x+kx;			
				
				sum_0 = 0.0;
				sum_1 = 0.0;
				sum_2 = 0.0;
				for(ly=y1;ly<=y2;ly++)
				{					
					if(ly == y1)
						w = w3;
					else if(ly == y2)
						w = w4;
					else
						w = 1.0;					
					
					for(lx=x1;lx<=x2;lx++)
					{						
						if(lx == x1)
							w = w*w1;
						else if(lx == x2)
							w = w*w2;						
				
						if(w == 1.0)
						{
							sum_0 = sum_0+(float)SRC_0(lx,ly);
						}
						else
						{
							sum_0 = sum_0+(float)SRC_0(lx,ly)*w;
						}
					}
				}
									
				DST_0(i,j) = (int)sum_0/area;
				
			}
			for(;i<dst_width;i++)
			{
				x0 = (int)x;
				x= x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
			}				
		}
		for(;j<dst_height;j++)
		{
			y0 = (int)(y+0.5);			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<dst_width;i++)
			{
				x0 = (int)(x+0.5);				
				x = x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
			}
		}
	}
	else if(cn==3)
	{
		y=0.0;
		for(j=0;j<r;j++)
		{
			y0 = (int)(y+0.5);			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<dst_width;i++)
			{
				x0 = (int)(x+0.5);				
				x = x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
			}
		}
		for(j=r;j<dst_height-r;j++)
		{
			y0 = (int)y;	
			
			w4 = y-(float)y0;
			w3 = (1.0-w4);
			
			y1 = (int)y-r;
			y2 = (int)y+r+1;
			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<r;i++)
			{
				x0 = (int)(x+0.5);
				x = x+kx;
			
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
			}			
			
			for(i=r;i<dst_width-r;i++)
			{
				x0 = (int)x;				
				
				w2 = x-(float)x0;								
				w1 = (1.0-w2);				
				
				x1 = (int)x-r;
				x2 = (int)x+r+1;	
				
				x = x+kx;			
				
				sum_0 = 0.0;
				sum_1 = 0.0;
				sum_2 = 0.0;
				for(ly=y1;ly<=y2;ly++)
				{					
					if(ly == y1)
						w = w3;
					else if(ly == y2)
						w = w4;
					else
						w = 1.0;					
					
					for(lx=x1;lx<=x2;lx++)
					{						
						if(lx == x1)
							w = w*w1;
						else if(lx == x2)
							w = w*w2;						
				
						if(w == 1.0)
						{
							sum_0 = sum_0+(float)SRC_0(lx,ly);
							sum_1 = sum_1+(float)SRC_1(lx,ly);
							sum_2 = sum_2+(float)SRC_2(lx,ly);
						}
						else
						{
							sum_0 = sum_0+(float)SRC_0(lx,ly)*w;
							sum_1 = sum_1+(float)SRC_1(lx,ly)*w;
							sum_2 = sum_2+(float)SRC_2(lx,ly)*w;
						}
					}
				}
									
				DST_0(i,j) = (int)sum_0/area;
				DST_1(i,j) = (int)sum_1/area;
				DST_2(i,j) = (int)sum_2/area;				
			}
			for(;i<dst_width;i++)
			{
				x0 = (int)x;
				x= x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
			}				
		}
		for(;j<dst_height;j++)
		{
			y0 = (int)(y+0.5);			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<dst_width;i++)
			{
				x0 = (int)(x+0.5);				
				x = x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
			}
		}
	}
	else if(cn==4)
	{
		y=0.0;
		for(j=0;j<r;j++)
		{
			y0 = (int)(y+0.5);			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<dst_width;i++)
			{
				x0 = (int)(x+0.5);				
				x = x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
				DST_3(i,j) = SRC_3(x0,y0);
			}
		}
		for(j=r;j<dst_height-r;j++)
		{
			y0 = (int)y;	
			
			w4 = y-(float)y0;
			w3 = (1.0-w4);
			
			y1 = (int)y-r;
			y2 = (int)y+r+1;
			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<r;i++)
			{
				x0 = (int)(x+0.5);
				x = x+kx;
			
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
				DST_3(i,j) = SRC_3(x0,y0);
			}			
			
			for(i=r;i<dst_width-r;i++)
			{
				x0 = (int)x;				
				
				w2 = x-(float)x0;								
				w1 = (1.0-w2);				
				
				x1 = (int)x-r;
				x2 = (int)x+r+1;	
				
				x = x+kx;			
				
				sum_0 = 0.0;
				sum_1 = 0.0;
				sum_2 = 0.0;
				for(ly=y1;ly<=y2;ly++)
				{					
					if(ly == y1)
						w = w3;
					else if(ly == y2)
						w = w4;
					else
						w = 1.0;					
					
					for(lx=x1;lx<=x2;lx++)
					{						
						if(lx == x1)
							w = w*w1;
						else if(lx == x2)
							w = w*w2;						
				
						if(w == 1.0)
						{
							sum_0 = sum_0+(float)SRC_0(lx,ly);
							sum_1 = sum_1+(float)SRC_1(lx,ly);
							sum_2 = sum_2+(float)SRC_2(lx,ly);
							sum_3 = sum_3+(float)SRC_3(lx,ly);
						}
						else
						{
							sum_0 = sum_0+(float)SRC_0(lx,ly)*w;
							sum_1 = sum_1+(float)SRC_1(lx,ly)*w;
							sum_2 = sum_2+(float)SRC_2(lx,ly)*w;
							sum_3 = sum_3+(float)SRC_3(lx,ly)*w;
						}
					}
				}
									
				DST_0(i,j) = (int)sum_0/area;
				DST_1(i,j) = (int)sum_1/area;
				DST_2(i,j) = (int)sum_2/area;
				DST_3(i,j) = (int)sum_3/area;				
			}
			for(;i<dst_width;i++)
			{
				x0 = (int)x;
				x= x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
				DST_3(i,j) = SRC_3(x0,y0);
			}				
		}
		for(;j<dst_height;j++)
		{
			y0 = (int)(y+0.5);			
			y = y+ky;
			
			x=0.0;
			for(i=0;i<dst_width;i++)
			{
				x0 = (int)(x+0.5);				
				x = x+kx;
				
				DST_0(i,j) = SRC_0(x0,y0);
				DST_1(i,j) = SRC_1(x0,y0);
				DST_2(i,j) = SRC_2(x0,y0);
				DST_3(i,j) = SRC_3(x0,y0);
			}
		}
	}
	
	PTR_FREE(src);
	PTR_FREE(dst);
}

ImgMat *imgCreateMat(int height,int width,char type);

void Debase(ImgMat *src,ImgMat *dst,int height,int width,int r)
{
#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgDebase,src);
	if(((height == 0)||(width ==0))&&(dst==NULL))
	{
		printf("IMG Error:\n\tin imgDebase.\n");
		exit(0);
	}
#endif
	
	if(dst==NULL)
	{
		dst = imgCreateMat(height,width,src->type);
		imgDebase(src,dst,height,width,r);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgDebase(src,dst,height,width,r);
}
