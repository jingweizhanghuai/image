#include <stdio.h>
#include "type.h"
#include "err.h"

float *lens_list(float m0,int r1,int r2)
{
	float *list;
	list = malloc((r2+1)*sizeof(float));
	
	m0 = 1.0/m0;
	
	float m1 = 1.0;
	float m2 = 1.0;
	int r3 = (r1+r2)>>1;
	float m3 = 1.0-(m0-1.0)*(float)r1/(float)(r2-r1);
	
	if(m3<0)
	{
		printf("IMG Error:\n\tin imgLens.\n");
		exit(0);
	}
	
	int i;
	float dm;
	float m;
	
	m = m0;
	list[0] = m;
	
	float l=0;	
	
	dm = (m1-m0)/(float)(r1);
	
	for(i=1;i<=r1;i++)
	{
		
		l = l+m;
		list[i] = l/(float)i;
		m = m+dm;
	}
	
	dm = (m3-m1)/(float)(r3-r1);
	for(;i<=r3;i++)
	{
		
		l = l+m;
		list[i] = l/(float)i;
		m = m+dm;
	}
	
	dm = (m2-m3)/(float)(r2-r3);
	for(;i<=r2;i++)
	{
		
		l = l+m;
		list[i] = l/(float)i;
		m = m+dm;
	}
	
	return list;
}
	
#define DIST2(x1,y1,x2,y2) ((float)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))
#define ABS(x) (((x)>0)?(x):(0-(x)))


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

float imgSqrt(float x);

void imgLens(ImgMat *src,ImgMat *dst,ImgPoint *center,float m0,int r1,int r2)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAffine,src);
	DESTINATION_ERROR_CHECK(imgAffine,dst);
	#endif
	
	if(m0<0)
	{
		printf("IMG Error:\n\tin imgLens: m0 should larger than 0.\n");
		exit(0);
	}
	if(r1>=r2)
	{
		printf("IMG Error:\n\tin imgLens: r1 is %d, r2 is %d, but r1 should be less than r2.\n",r1,r2);
		exit(0);
	}
	if(r1==0)
	{
		printf("IMG Error:\n\tin imgLens: r1 should not be 0.\n");
		exit(0);
	}
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if(src->memory_valid[0]==0)
		imgReadMatOCLMemory(src);
	
	if((dst->width!=img_width)||(dst->height!=img_height)||(dst->type != src->type))
		imgMatRedefine(dst,img_height,img_width,src->type);
	
	int i,j;
	
	float *list;
	list = lens_list(m0,r1,r2);
	
	// for(i=0;i<r2;i++)
		// printf("list[i] is %f\n",list[i]);
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;	
	
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
	
	int x0,y0;
	if(center!=NULL)
	{
		x0 = center->x;
		y0 = center->y;
	}
	else
	{
		x0 = img_width>>1;
		y0 = img_height>>1;
	}
	
	float dist;
	int n;
	float k1,k2,k;
	float w1,w2,w3,w4;
	float wx,wy;
	float lx,ly;
	int x1,x2,y1,y2;
	
	if(cn == 1)
	{
		for(j=0;j<img_height;j++)
		{
			if(ABS(j-y0)>r2)
			{
				for(i=0;i<img_width;i++)
					DST_0(i,j) = SRC_0(i,j);
				
				continue;
			}

			for(i=0;i<img_width;i++)
			{
				if(ABS(i-x0)>r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					continue;
				}
				
				dist = DIST2(i,j,x0,y0);
				
				if(dist>r2*r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					continue;
				}
				
				dist = imgSqrt(dist);
				n = (int)dist;
				
				k1=list[n];
				k2=list[n+1];
				
				w2 = dist-(float)n;
				w1 = 1.0-w2;
				
				k = k1*w1+k2*w2;
				
				lx = x0+(i-x0)*k;
				ly = y0+(j-y0)*k;
				
				x1 = (int)lx;
				x2 = x1+1;
				y1 = (int)ly;
				y2 = y1+1;
				
				wx = lx-(float)x1;
				wy = ly-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
			}
		}
	}
	else if(cn == 2)
	{
		for(j=0;j<img_height;j++)
		{
			if(ABS(j-y0)>r2)
			{
				for(i=0;i<img_width;i++)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
				}
				
				continue;
			}

			for(i=0;i<img_width;i++)
			{
				if(ABS(i-x0)>r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					continue;
				}
				
				dist = DIST2(i,j,x0,y0);
				
				if(dist>r2*r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					continue;
				}
				
				dist = imgSqrt(dist);
				n = (int)dist;
				
				k1=list[n];
				k2=list[n+1];
				
				w2 = dist-(float)n;
				w1 = 1.0-w2;
				
				k = k1*w1+k2*w2;
				
				lx = x0+(i-x0)*k;
				ly = y0+(j-y0)*k;
				
				x1 = (int)lx;
				x2 = x1+1;
				y1 = (int)ly;
				y2 = y1+1;
				
				wx = lx-(float)x1;
				wy = ly-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
			}
		}
	}
	else if(cn == 3)
	{
		for(j=0;j<img_height;j++)
		{
			if(ABS(j-y0)>r2)
			{
				for(i=0;i<img_width;i++)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					DST_2(i,j) = SRC_2(i,j);
				}
				
				continue;
			}

			for(i=0;i<img_width;i++)
			{
				if(ABS(i-x0)>r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					DST_2(i,j) = SRC_2(i,j);
					continue;
				}
				
				dist = DIST2(i,j,x0,y0);
				
				if(dist>r2*r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					DST_2(i,j) = SRC_2(i,j);
					continue;
				}
				
				dist = imgSqrt(dist);
				n = (int)dist;
				
				k1=list[n];
				k2=list[n+1];
				
				w2 = dist-(float)n;
				w1 = 1.0-w2;
				
				k = k1*w1+k2*w2;
				
				lx = x0+(i-x0)*k;
				ly = y0+(j-y0)*k;
				
				x1 = (int)lx;
				x2 = x1+1;
				y1 = (int)ly;
				y2 = y1+1;
				
				wx = lx-(float)x1;
				wy = ly-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
				DST_2(i,j) =(int)((float)SRC_2(x1,y1)*w1+(float)SRC_2(x2,y1)*w2+(float)SRC_2(x2,y2)*w3+(float)SRC_2(x1,y2)*w4);
			}
		}
	}
	else if(cn == 4)
	{
		for(j=0;j<img_height;j++)
		{
			if(ABS(j-y0)>r2)
			{
				for(i=0;i<img_width;i++)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					DST_2(i,j) = SRC_2(i,j);
					DST_3(i,j) = SRC_3(i,j);
				}
				
				continue;
			}

			for(i=0;i<img_width;i++)
			{
				if(ABS(i-x0)>r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					DST_2(i,j) = SRC_2(i,j);
					DST_3(i,j) = SRC_3(i,j);
					continue;
				}
				
				dist = DIST2(i,j,x0,y0);
				
				if(dist>r2*r2)
				{
					DST_0(i,j) = SRC_0(i,j);
					DST_1(i,j) = SRC_1(i,j);
					DST_2(i,j) = SRC_2(i,j);
					DST_3(i,j) = SRC_3(i,j);
					continue;
				}
				
				dist = imgSqrt(dist);
				n = (int)dist;
				
				k1=list[n];
				k2=list[n+1];
				
				w2 = dist-(float)n;
				w1 = 1.0-w2;
				
				k = k1*w1+k2*w2;
				
				lx = x0+(i-x0)*k;
				ly = y0+(j-y0)*k;
				
				x1 = (int)lx;
				x2 = x1+1;
				y1 = (int)ly;
				y2 = y1+1;
				
				wx = lx-(float)x1;
				wy = ly-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
				DST_2(i,j) =(int)((float)SRC_2(x1,y1)*w1+(float)SRC_2(x2,y1)*w2+(float)SRC_2(x2,y2)*w3+(float)SRC_2(x1,y2)*w4);
				DST_3(i,j) =(int)((float)SRC_3(x1,y1)*w1+(float)SRC_3(x2,y1)*w2+(float)SRC_3(x2,y2)*w3+(float)SRC_3(x1,y2)*w4);
			}
		}
	}
	
	PTR_FREE(src);
	PTR_FREE(dst);
	
	free(list);
	
	dst->memory_valid[0] = 1;
	dst->memory_valid[1] = 0;
	
}

ImgMat *imgCreateMat(int height,int width,char type);

void Lens(ImgMat *src,ImgMat *dst,ImgPoint *center,float m0,int r1,int r2)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgLens,src);
	#endif

	if(dst == NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgLens(src,dst,center,m0,r1,r2);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgLens(src,dst,center,m0,r1,r2);
}			
			
	
// void imgLineLens(ImgMat *src,ImgMat *dst,ImgLine *center,float m0,int r1,int r2)
// {
	
	
	
	
	
	
	
	
	
	
	
	