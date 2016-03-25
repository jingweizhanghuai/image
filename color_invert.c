#include <stdio.h>
#include "type.h"
#include "err.h"

void imgColorInvert(ImgMat *src,ImgMat *dst)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgColorInvert,src);
	DESTINATION_ERROR_CHECK(imgColorInvert,dst);
	#endif
	
	if((dst->height != src->height)||(dst->width !=src->width)||(dst->type != src->type))
		imgMatRedefine(dst,src->height,src->width,src->type);
		
	int cn;
	cn = ((src->type)>>3)+1;
	
	int img_size;
	img_size = src->size*cn;
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int i;
	for(i=0;i<img_size;i++)
	{
		*p_dst = !(*p_src);
		p_src = p_src+1;
		p_dst = p_dst+1;
	}
}

void ColorInvert(ImgMat *src,ImgMat *dst)
{
	if(dst == NULL)
		imgColorInvert(src,src);
	else
		imgColorInvert(src,dst);
}
