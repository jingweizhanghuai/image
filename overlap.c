#include <stdio.h>
#include "type.h"
#include "err.h"

// #define SRC_PTR {\
	// p_src = malloc((src->height)<<2);\
	// p_src[0] = src->data.ptr;\
	// for(i=1;i<src->height;i++)\
		// p_src[i] = p_src[i-1]+src->step;\
// }\

// #define DST_PTR {\
	// p_dst = malloc((dst->height)<<2);\
	// p_dst[0] = dst->data.ptr;\
	// for(i=1;i<dst->height;i++)\
		// p_dst[i] = p_dst[i-1]+dst->step;\
// }\

// #define SRC(x,y) *(p_src[y]+(x))
// #define DST(x,y) *(p_dst[y]+(x))

// #define SRC3_0(x,y) *(p_src[y]+(x)+(x)+(x))
// #define DST3_0(x,y) *(p_dst[y]+(x)+(x)+(x))
// #define SRC3_1(x,y) *(p_src[y]+(x)+(x)+(x)+1)
// #define DST3_1(x,y) *(p_dst[y]+(x)+(x)+(x)+1)
// #define SRC3_2(x,y) *(p_src[y]+(x)+(x)+(x)+2)
// #define DST3_2(x,y) *(p_dst[y]+(x)+(x)+(x)+2)

// #define SRC4_0(x,y) *(p_src[y]+(x)+(x)+(x)+(x))
// #define DST4_0(x,y) *(p_dst[y]+(x)+(x)+(x)+(x))
// #define SRC4_1(x,y) *(p_src[y]+(x)+(x)+(x)+(x)+1)
// #define DST4_1(x,y) *(p_dst[y]+(x)+(x)+(x)+(x)+1)
// #define SRC4_2(x,y) *(p_src[y]+(x)+(x)+(x)+(x)+2)
// #define DST4_2(x,y) *(p_dst[y]+(x)+(x)+(x)+(x)+2)
// #define SRC4_3(x,y) *(p_src[y]+(x)+(x)+(x)+(x)+3)
// #define DST4_3(x,y) *(p_dst[y]+(x)+(x)+(x)+(x)+3)


// #define SRC1(x,y,c) p_src1[(y)*img_width*cn1 + (x)*cn1+c]
// #define SRC2(x,y,c) p_src2[(y)*img_width*cn2 + (x)*cn2+c]

// #define SRC(x,y,c) p_src[(y)*img_width*cn + (x)*cn+c]
// #define DST(x,y,c) p_dst[(y)*dst_width*cn + (x)*cn+c]

ImgMat *imgCreateMat(int height,int width,char type);
void imgReleaseMat(ImgMat *src);
void imgReleaseMatData(ImgMat *src);

void imgOverlap(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *strength)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgOverlap,src1);
	SOURCE_ERROR_CHECK(imgOverlap,src2);
	DESTINATION_ERROR_CHECK(imgOverlap,dst);
	#endif
	
	imgMate(src1,dst);
	imgMate(src2,dst);
	
	int img_size;
	img_size = dst->width*dst->height;
	
	int i,j;
	
	unsigned char *p_src1;
	p_src1 = src1->data.ptr;
	
	unsigned char *p_src2;
	p_src2 = src2->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int k1,k2;
	if((*strength <= 0.0)||(*strength >= 1.0))
	{
		k1 = 512;
		k2 = 512;
	}
	else
	{
		k2 = (int)(1024.0*(*strength));
		k1 = 1024-k2; 
	}
	// int k2_r,k2_g,k2_b;
	// k2_r = (int)(1024.0*strength*0299);
	// k2_g = (int)(1024.0*strength*0299);
	// k2_b = (int)(1024.0*strength*0299);
	
	int cn1;
	cn1 = ((src1->type&0xF8)>>3)+1;
	
	int cn2;
	cn2 = ((src2->type&0xF8)>>3)+1;
	
	int cn_dst;
	cn_dst = (cn1>cn2)?cn1:cn2;
	cn_dst = (cn_dst>3)?3:cn_dst;
	
	int dst_type;
	dst_type = (cn_dst<<3)+(dst_type&0x07);
	
	if(dst_type != dst->type)
		imgMatRedefine(dst,dst->height,dst->width,dst_type);	

	if((cn1 == 1)&&(cn2 == 1))
	{
		for(i=0;i<img_size;i++)
		{
			*p_dst = ((*p_src1)*k1+(*p_src2)*k2+512)>>10;
			p_src1 = p_src1+1;
			p_src2 = p_src2+1;
			p_dst = p_dst+1;
		}
		
		// for(j=0;j<img_height;j++)
			// for(i=0;i<img_width;i++)				
				// DST(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
	}
	else if((cn1 >= 3)&&(cn2 == 1))
	{
		for(i=0;i<img_size;i++)
		{
			p_dst[0] = (p_src1[0]*k1+p_src2[0]*k2+512)>>10;
			p_dst[1] = (p_src1[1]*k1+p_src2[0]*k2+512)>>10;
			p_dst[2] = (p_src1[2]*k1+p_src2[0]*k2+512)>>10;
			p_src1 = p_src1+cn1;
			p_src2 = p_src2+1;
			p_dst = p_dst+3;
		}
			
		// for(j=0;j<img_height;j++)
			// for(i=0;i<img_width;i++)
			// {
				// SRC1(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
				// SRC1(i,j,1) = (SRC1(i,j,1)*k1+SRC2(i,j,0)*k2+512)>>10;
				// SRC1(i,j,2) = (SRC1(i,j,2)*k1+SRC2(i,j,0)*k2+512)>>10;
			// }
	}
	else if((cn1 >= 3)&&(cn2 >= 3))
	{
		for(i=0;i<img_size;i++)
		{
			p_dst[0] = (p_src1[0]*k1+p_src2[0]*k2+512)>>10;
			p_dst[1] = (p_src1[1]*k1+p_src2[1]*k2+512)>>10;
			p_dst[2] = (p_src1[2]*k1+p_src2[2]*k2+512)>>10;
			p_src1 = p_src1+cn1;
			p_src2 = p_src2+cn2;
			p_dst = p_dst+3;
		}
		// for(j=0;j<img_height;j++)
			// for(i=0;i<img_width;i++)
			// {
				// SRC1(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
				// SRC1(i,j,1) = (SRC1(i,j,1)*k1+SRC2(i,j,1)*k2+512)>>10;
				// SRC1(i,j,2) = (SRC1(i,j,2)*k1+SRC2(i,j,2)*k2+512)>>10;
			// }
	}
	else if((cn1 == 1)&&(cn2 >= 3))
	{
		for(i=0;i<img_size;i++)
		{
			p_dst[0] = (p_src1[0]*k1+p_src2[0]*k2+512)>>10;
			p_dst[1] = (p_src1[0]*k1+p_src2[1]*k2+512)>>10;
			p_dst[2] = (p_src1[0]*k1+p_src2[2]*k2+512)>>10;
			p_src1 = p_src1+1;
			p_src2 = p_src2+cn2;
			p_dst = p_dst+3;
		}
		
		// ImgMat *dst;
		// dst = imgCreateMat(img_height,img_width,src2->type);
				
		// unsigned char *p_dst;
		// p_dst = dst->data.ptr;
		
		// int dst_width = img_width;
		// int dst_height = img_height;
		// int cn = 3;
				
		// for(j=0;j<img_height;j++)
			// for(i=0;i<img_width;i++)
			// {
				// DST(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
				// DST(i,j,1) = (SRC1(i,j,0)*k1+SRC2(i,j,1)*k2+512)>>10;
				// DST(i,j,2) = (SRC1(i,j,0)*k1+SRC2(i,j,2)*k2+512)>>10;
			// }
			
		// imgReleaseMatData(src1);
		// free(src1->hidinfo);
		
		// src1->hidinfo = dst->hidinfo;
		// src1->step = dst->step;
		// src1->size = dst->size;
		// src1->type = dst->type;
		// src1->width = dst_width;
		// src1->height = dst_height;
		// src1->data.ptr = p_dst;

		// free(dst);
	}	
}

ImgMat *imgCreateMat(int height,int width,char type);

void Overlap(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *strength)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgOverlap,src1);
	#endif
	
	if(dst == NULL)
	{
		dst = imgCreateMat(src1->height,src1->width,src1->type);
		imgOverlap(src1,src2,dst,strength);
		free(src1->data.ptr);
		free(src1->hidinfo);
		*src1 = *dst;
		free(dst);
	}
	else
		imgAdd(src1,src2,dst,strength);
}
