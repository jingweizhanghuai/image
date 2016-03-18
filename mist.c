#include <stdio.h>
#include "type.h"

#define SRC_PTR(p) {\
	p_src = malloc(img_height<<2);\
	p_src[0] = src->data.ptr;\
	for(i=1;i<img_height;i++)\
		p_src[i] = p_src[i-1]+src->step;\
}\

#define DST_PTR(p) {\
	p_dst = malloc(img_height<<2);\
	p_dst[0] = dst->data.ptr;\
	for(i=1;i<img_height;i++)\
		p_dst[i] = p_dst[i-1]+dst->step;\
}\

#define SRC(x,y) *(p_src[y]+(x))
#define DST(x,y) *(p_dst[y]+(x))

void imgMist(ImgMat *src,ImgMat *dst,int n)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn!=1)
		return;
	
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
	
	unsigned char **p_src;
	SRC_PTR(p_src);
	
	unsigned char **p_dst;
	DST_PTR(p_dst);

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
			DST(i,j) = SRC(i,j);
	
	for(j=n;j<img_height-n;j++)
	{
		for(i=0;i<n;i++)
			DST(i,j) = SRC(i,j);
		
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
		
			data = SRC(i+lx,j+ly);
			DST(i,j) = data;
		}
		
		for(;i<img_width;i++)
			DST(i,j) = SRC(i,j);
	}
	
	for(;j<img_height;j++)
		for(i=0;i<img_width;i++)
			DST(i,j) = SRC(i,j);
		
	free(p_src);
	free(p_dst);
}
