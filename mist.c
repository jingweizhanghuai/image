#include <stdio.h>
#include "type.h"

#define PTR_1(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
}\

#define PTR_3(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	p1##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p1##mat[i] = p0##mat[i]+mat->size;\
	p2##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p2##mat[i] = p1##mat[i]+mat->size;\
}\

#define PTR_4(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	p1##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p1##mat[i] = p0##mat[i]+mat->size;\
	p2##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p2##mat[i] = p1##mat[i]+mat->size;\
	p3##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p3##mat[i] = p2##mat[i]+mat->size;\
}\

#define SRC_0(x,y) *(p0src[y]+(x))
#define DST_0(x,y) *(p0dst[y]+(x))
#define SRC_1(x,y) *(p1src[y]+(x))
#define DST_1(x,y) *(p1dst[y]+(x))
#define SRC_2(x,y) *(p2src[y]+(x))
#define DST_2(x,y) *(p2dst[y]+(x))
#define SRC_3(x,y) *(p3src[y]+(x))
#define DST_3(x,y) *(p3dst[y]+(x))

void imgMist_1(ImgMat *src,ImgMat *dst,int n)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMist,src);
	DESTINATION_ERROR_CHECK(imgMist,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgMist.\n");
		exit(0);
	}
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=1)
	{
		printf("IMG Error:\n\tin imgMist.\n");
		exit(0);
	}
	#endif
	
	if(n==0)
	{
		imgCopyMat(src,dst);
		return;
	}
	else if(n>10)
		n = 10;
	else if(n<1)
		n = 1;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int type = 0;
	
	int i,j;
	int l,lx,ly;
	int x;
	x = 3563;
	
	unsigned char **p0src;
	PTR_1(src);
	
	unsigned char **p0dst;
	PTR_1(dst);

	int num;
	num = (2*n+1)*(2*n+1);
	
	int data;
	data = 0;
	
	int *r;
	r = malloc(num*8);
	int *p;
	p = r;
	for(j=-n;j<=n;j++)
		for(i=-n;i<=n;i++)
		{
			*p = i;
			*(p+1) = j;	
			p = p+2;
		}	
	
	for(j=0;j<n;j++)
		for(i=0;i<img_width;i++)
			DST_0(i,j) = SRC_0(i,j);
	
	for(j=n;j<img_height-n;j++)
	{
		for(i=0;i<n;i++)
			DST_0(i,j) = SRC_0(i,j);
		
		if(type == 1)
			x = (j|x)*x;
		
		for(i=n;i<img_width-n;i++)
		{
			if(type == 2)
				x= (i|x)*x;
			else if(type == 0)
				x = ((j|i|x))*(x+data+57);
			
			x = x&0x7FFFFFFF;
			l = (x%num)<<1;
			
			lx = r[l];
			ly = r[l+1];
			
			// if(j==255)
			// printf("x is %d  \tl is %d\t(%d,%d)\n",x,l,lx,ly);
		
			data = SRC_0(i+lx,j+ly);
			DST_0(i,j) = data;
		}
		
		for(;i<img_width;i++)
			DST_0(i,j) = SRC_0(i,j);
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
			DST_0(i,j) = SRC_0(i,j);
		
	free(p0src);
	free(p0dst);
}

void imgMist_3(ImgMat *src,ImgMat *dst,int n)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMist,src);
	DESTINATION_ERROR_CHECK(imgMist,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgMist.\n");
		exit(0);
	}
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=3)
	{
		printf("IMG Error:\n\tin imgMist.\n");
		exit(0);
	}
	#endif
	
	if(n==0)
	{
		imgCopyMat(src,dst);
		return;
	}
	else if(n>10)
		n = 10;
	else if(n<1)
		n = 1;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int type = 0;
	
	int i,j;
	int l,lx,ly;
	int x;
	x = 3563;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	PTR_3(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	PTR_3(dst);

	int num;
	num = (2*n+1)*(2*n+1);
	
	int data;
	data = 0;
	
	int *r;
	r = malloc(num*8);
	int *p;
	p = r;
	for(j=-n;j<=n;j++)
		for(i=-n;i<=n;i++)
		{
			*p = i;
			*(p+1) = j;	
			p = p+2;
		}
	
	
	for(j=0;j<n;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
		}
	
	for(j=n;j<img_height-n;j++)
	{
		for(i=0;i<n;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
		}
		
		if(type == 1)
			x = (j|x)*x;
		
		for(i=n;i<img_width-n;i++)
		{
			if(type == 2)
				x= (i|x)*x;
			else if(type == 0)
				x = ((j|i|x))*(x+data+57);
			
			x = x&0x7FFFFFFF;
			l = (x%num)<<1;
			
			lx = r[l];
			ly = r[l+1];
			
			// if(j==255)
			// printf("x is %d  \tl is %d\t(%d,%d)\n",x,l,lx,ly);
		
			data = SRC_0(i+lx,j+ly);
			DST_0(i,j) = data;
			DST_1(i,j) = SRC_1(i+lx,j+ly);
			DST_2(i,j) = SRC_2(i+lx,j+ly);
		}
		
		for(;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
		}
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
		}
		
	free(p0src);
	free(p0dst);
	free(p1src);
	free(p1dst);
	free(p2src);
	free(p2dst);
}

void imgMist_4(ImgMat *src,ImgMat *dst,int n)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMist,src);
	DESTINATION_ERROR_CHECK(imgMist,dst);
	if(r<0)
	{
		printf("IMG Error:\n\tin imgMist.\n");
		exit(0);
	}
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=4)
	{
		printf("IMG Error:\n\tin imgMist.\n");
		exit(0);
	}
	#endif
	
	if(n==0)
	{
		imgCopyMat(src,dst);
		return;
	}
	else if(n>10)
		n = 10;
	else if(n<1)
		n = 1;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((img_height != dst->height)||(img_width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int type = 0;
	
	int i,j;
	int l,lx,ly;
	int x;
	x = 3563;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	unsigned char **p3src;
	PTR_4(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	unsigned char **p3dst;
	PTR_4(dst);

	int num;
	num = (2*n+1)*(2*n+1);
	
	int data;
	data = 0;
	
	int *r;
	r = malloc(num*8);
	int *p;
	p = r;
	for(j=-n;j<=n;j++)
		for(i=-n;i<=n;i++)
		{
			*p = i;
			*(p+1) = j;	
			p = p+2;
		}
	
	
	for(j=0;j<n;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
			DST_3(i,j) = SRC_3(i,j);
		}
	
	for(j=n;j<img_height-n;j++)
	{
		for(i=0;i<n;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
			DST_3(i,j) = SRC_3(i,j);
		}
		
		if(type == 1)
			x = (j|x)*x;
		
		for(i=n;i<img_width-n;i++)
		{
			if(type == 2)
				x= (i|x)*x;
			else if(type == 0)
				x = ((j|i|x))*(x+data+57);
			
			x = x&0x7FFFFFFF;
			l = (x%num)<<1;
			
			lx = r[l];
			ly = r[l+1];
		
			data = SRC_0(i+lx,j+ly);
			DST_0(i,j) = data;
			DST_1(i,j) = SRC_1(i+lx,j+ly);
			DST_2(i,j) = SRC_2(i+lx,j+ly);
			DST_3(i,j) = SRC_3(i+lx,j+ly);
		}
		
		for(;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
			DST_3(i,j) = SRC_3(i,j);
		}
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
		{
			DST_0(i,j) = SRC_0(i,j);
			DST_1(i,j) = SRC_1(i,j);
			DST_2(i,j) = SRC_2(i,j);
			DST_3(i,j) = SRC_3(i,j);
		}
		
	free(p0src);
	free(p0dst);
	free(p1src);
	free(p1dst);
	free(p2src);
	free(p2dst);
	free(p3src);
	free(p3dst);
}

void imgMist(ImgMat *src,ImgMat *dst,int r)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMist,src);
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn==1)
		imgMist_1(src,dst,r);
	else if(cn == 3)
		imgMist_3(src,dst,r);
	else if(cn == 4)
		imgMist_4(src,dst,r);
	else 
	{
		printf("IMG Error:\n\tin imgMist.\n");
		exit(0);
	}
}

ImgMat *imgCreateMat(int height,int width,char type);

void Mist(ImgMat *src,ImgMat *dst,int r,int thresh) 
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMist,src);
	#endif
	
	if(dst==NULL)
	{
		if(r==0)
			return;

		dst = imgCreateMat(src->height,src->width,src->type);
		imgMist(src,dst,r);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgMist(src,dst,r);
}
