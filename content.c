#include <stdio.h>
#include "type.h"

#define PTR(mat) {\
	p##mat[0] = p_##mat;\
	for(i=1;i<mat->height;i++)\
		p##mat[i] = p##mat[i-1]+mat->width;\
}\

#define SRC(x,y) *(psrc[y]+(x))
#define DST(x,y) *(pdst[y]+(x))

#define ABS(x) (((x)>0)?(x):(0-(x)))

void imgContent(ImgMat *src,ImgMat *dst)
{
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	int img_size;
	img_size = src->size;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	unsigned char *p_src;
	p_src= src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char **psrc;
	psrc = malloc(img_height<<2);
	unsigned char **pdst;	
	pdst = malloc(img_height<<2);
	
	int i,j,k;
	
	int thresh = 5;
	
	unsigned char data1,data2,data3,data4,data5,data6,data7,data8,data0;
	
	for(k=0;k<cn;k++)
	{
		PTR(src);
		PTR(dst);
		
		for(i=0;i<img_width;i++)
			DST(i,0) = 0;
		
		for(j=1;j<img_height-1;j++)
		{
			DST(0,j) = 0;
			
			data1 = SRC(0,j-1);
			data2 = SRC(1,j-1);
			data0 = SRC(0,j);
			data5 = SRC(1,j);
			data7 = SRC(0,j+1);
			data8 = SRC(1,j+1);
			
			for(i=1;i<img_width-1;i++)
			{
				data1 = data2;
				data2 = data3;
				data3 = SRC(i+1,j-1);
				
				data4 = data0;
				data0 = data5;
				data5 = SRC(i+1,j);
				
				data6 = data7;
				data7 = data8;
				data8 = SRC(i+1,j+1);			
				
				if((ABS(data1-data2)>thresh)||(ABS(data4-data5)>thresh)||(ABS(data6-data8)>thresh))
				{
					DST(i,j) = data0;
					continue;
				}
				
				if((ABS(data1-data6)>thresh)||(ABS(data2-data7)>thresh)||(ABS(data3-data8)>thresh))
				{
					DST(i,j) = data0;
					continue;
				}
				
				DST(i,j) = 0;
			}
					
			DST(i,j) = 0;
		}
	
		for(i=0;i<img_width;i++)
			DST(i,j) = 0;
		
		p_src = p_src+img_size;
		p_dst = p_dst+img_size;
	}
	free(psrc);
	free(pdst);
}

