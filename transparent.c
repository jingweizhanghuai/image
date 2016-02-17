#include <stdio.h>
#include "type.h"

ImgMat *imgCreateMat(int height,int width,char type);

void imgTransparent(ImgMat *src)
{
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn == 1)
		return;
	
	int img_size;
	img_size = src->width*src->height;	
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	ImgMat *dst;
	dst = imgCreateMat(src->height,src->width,TYPE_8UC4);
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int i;
	
	for(i=0;i<img_size;i++)
	{
		p_dst[0] = p_src[0];
		p_dst[1] = p_src[1];
		p_dst[2] = p_src[2];
		p_dst[4] = (p_src[0]*30+p_src[1]*150+p_src[2]*76)>>8;
		
		p_dst = p_dst+4;
		p_src = p_src+cn;
	}
	
	free(src->data.ptr);
	free(src->hidinfo);
	
	*src = *dst;
	
	free(dst);	
}
