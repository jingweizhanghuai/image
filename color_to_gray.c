#include <stdio.h>
#include "type.h"

void imgCopyMat(ImgMat *src,ImgMat *dst);
void imgMatRedefine(ImgMat *src,int height,int width,int type);
ImgMat *imgCreateMat(int height,int width,char type);

void imgColorToGray(ImgMat *src,ImgMat *dst)
{	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);

	if(dst->data.ptr == NULL)
		imgCreateMatData(dst);
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	if(cn == 1)
	{
		imgCopyMat(src,dst);
		return;
	}
	
	int img_size;
	img_size = src->width*src->height;	
	
	unsigned char *p_src0;
	p_src0 = src->data.ptr;
	unsigned char *p_src1;
	p_src1 = p_src0+img_size;
	unsigned char *p_src2;
	p_src2 = p_src1+img_size;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int i;
	
	for(i=0;i<img_size;i++)
	{
		*p_dst = ((*p_src0)*29+(*p_src1)*150+(*p_src2)*77+128)>>8;
		p_dst = p_dst+1;
		p_src0 = p_src0+1;
		p_src1 = p_src1+1;
		p_src2 = p_src2+1;
	}
}

void ColorToGray(ImgMat *src,ImgMat *dst) 
{
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,TYPE_8UC1);
		imgColorToGray(src,dst);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgColorToGray(src,dst);
}

		
	
	
