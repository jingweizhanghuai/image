#include <stdio.h>
#include "err.h"
#include "type.h"

#define SATUR(x)	if(x>satur1)\
						x = satur1;\
					else if(x<satur2)\
						x = satur2;

#define IMG_ADD(p_src1,p_src2,p_dst) {\
	for(k=0;k<cn;k++)\
	{\
		for(i=0;i<img_size;i++)\
		{\
			data  = (*p_src1)+(*p_src2);\
			SATUR(data);\
			\
			*p_dst = data;\
			\
			p_src1 = p_src1 +1;\
			p_src2 = p_src2 +1;\
			p_dst = p_dst +1;\
		}\
		\
		if(k<(cn1-1))\
			p_src1 = p_src1+img_size;\
		if(k<(cn2-1))\
			p_src2 = p_src2+img_size;\
	}\
}\

void imgAdd(ImgMat *src1,ImgMat *src2,ImgMat *dst)
{	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAdd,src1);
	SOURCE_ERROR_CHECK(imgAdd,src2);
	DESTINATION_ERROR_CHECK(imgAdd,dst);
	#endif
	
	imgMate(src1,dst);
	imgMate(src2,dst);
	
	int cn1;
	cn1 = ((src1->type&0xF8)>>3)+1;
	
	int cn2;
	cn2 = ((src2->type&0xF8)>>3)+1;
	
	int cn;
	cn = (cn1>cn2)?cn1:cn2;
	
	int cn_dst;
	cn_dst = ((dst->type&0xF8)>>3)+1;
	
	if((cn==3)&&(cn_dst != 3))
	{
		dst->type = 0x10+(dst->type & 0x07);
		imgMatRedefine(dst,dst->height,dst->width,dst->type);
	}
	if((cn==1)&&(cn_dst!=1))
	{
		dst->type = (dst->type & 0x07);
		imgMatRedefine(dst,dst->height,dst->width,dst->type);
	}
	
	int img_size;
	img_size = dst->size;

	int type_s1;
	type_s1 = (src1->type)&0x07;
	int type_s2;
	type_s2 = (src2->type)&0x07;
	int type_d;
	type_d = (dst->type)&0x07;
	
	int satur1,satur2;
	if(type_d == TYPE_8U)
	{
		satur1 = 255;
		satur2 = 0;
	}
	else if(type_d == TYPE_8S)
	{
		satur1 = 127;
		satur2 = -128;
	}
	
	int i,k;
	int data;
	
	#define TYPE_D unsigned char
	if((type_s1 == TYPE_8U)&&(type_s2 == TYPE_8U)&&(type_d == TYPE_8U))
	{
		unsigned char *p_src1_1;
		p_src1_1 = src1->data.ptr;
		unsigned char *p_src2_1;
		p_src2_1 = src2->data.ptr;
		unsigned char *p_dst_1;
		p_dst_1 = dst->data.ptr;
		
		IMG_ADD(p_src1_1,p_src2_1,p_dst_1);
	}
	else if((type_s1 == TYPE_8S)&&(type_s2 == TYPE_8U)&&(type_d == TYPE_8U))
	{
		char *p_src1_2;
		p_src1_2 = (char *)src1->data.ptr;
		unsigned char *p_src2_2;
		p_src2_2 = src2->data.ptr;
		unsigned char *p_dst_2;
		p_dst_2 = dst->data.ptr;
		
		IMG_ADD(p_src1_2,p_src2_2,p_dst_2);
	}
	else if((type_s1 == TYPE_8U)&&(type_s2 == TYPE_8S)&&(type_d == TYPE_8U))
	{
		unsigned char *p_src1_3;
		p_src1_3 = src1->data.ptr;
		char *p_src2_3;
		p_src2_3 = (char *)src2->data.ptr;
		unsigned char *p_dst_3;
		p_dst_3 = dst->data.ptr;
		
		IMG_ADD(p_src1_3,p_src2_3,p_dst_3);
	}
	#undef TYPE_D
	#define TYPE_D char
	else if((type_s1 == TYPE_8S)&&(type_s2 == TYPE_8U)&&(type_d == TYPE_8S))
	{
		char *p_src1_4;
		p_src1_4 = (char *)src1->data.ptr;
		unsigned char *p_src2_4;
		p_src2_4 = src2->data.ptr;
		char *p_dst_4;
		p_dst_4 = (char *)dst->data.ptr;
		
		IMG_ADD(p_src1_4,p_src2_4,p_dst_4);		
	}
	else if((type_s1 == TYPE_8U)&&(type_s2 == TYPE_8S)&&(type_d == TYPE_8S))
	{
		unsigned char *p_src1_5;
		p_src1_5 = src1->data.ptr;
		char *p_src2_5;
		p_src2_5 = (char *)src2->data.ptr;
		char *p_dst_5;
		p_dst_5 = (char *)dst->data.ptr;
		
		IMG_ADD(p_src1_5,p_src2_5,p_dst_5);
	}
	else if((type_s1 == TYPE_8S)&&(type_s2 == TYPE_8S)&&(type_d == TYPE_8S))
	{
		char *p_src1_6;
		p_src1_6 = (char *)src1->data.ptr;
		char *p_src2_6;
		p_src2_6 = (char *)src2->data.ptr;
		char *p_dst_6;
		p_dst_6 = (char *)dst->data.ptr;
		
		IMG_ADD(p_src1_6,p_src2_6,p_dst_6);
	}	
}
	
ImgMat *imgCreateMat(int height,int width,char type);

void Add(ImgMat *src1,ImgMat *src2,ImgMat *dst)
{
	if(dst == NULL)
		imgAdd(src1,src2,src1);
	else
		imgAdd(src1,src2,dst);
}

