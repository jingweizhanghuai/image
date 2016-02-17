#include <stdio.h>
#include "type.h"

#define SRC1(x,y,c) p_src1[(y)*img_width*cn1 + (x)*cn1+c]
#define SRC2(x,y,c) p_src2[(y)*img_width*cn2 + (x)*cn2+c]

#define SRC(x,y,c) p_src[(y)*img_width*cn + (x)*cn+c]
#define DST(x,y,c) p_dst[(y)*dst_width*cn + (x)*cn+c]

ImgMat *imgCreateMat(int height,int width,char type);
void imgReleaseMat(ImgMat *src);
void imgReleaseMatData(ImgMat *src);

void imgOverlap(ImgMat *src1,ImgMat *src2,float strength)
{
	int img_width;
	img_width = src1->width;
	
	int img_height;
	img_height = src1->height;
	
	unsigned char *p_src1;
	p_src1 = src1->data.ptr;
	
	unsigned char *p_src2;
	p_src2 = src2->data.ptr;
	
	int k2;
	k2 = (int)(1024.0*strength);
	
	// int k2_r,k2_g,k2_b;
	// k2_r = (int)(1024.0*strength*0299);
	// k2_g = (int)(1024.0*strength*0299);
	// k2_b = (int)(1024.0*strength*0299);
	
	int k1 = 1024-k2; 
	
	int cn1;
	cn1 = ((src1->type&0xF8)>>3)+1;
	
	int cn2;
	cn2 = ((src2->type&0xF8)>>3)+1;
	
	int i,j;
	if((cn1 == 1)&&(cn2 == 1))
	{
		for(j=0;j<img_height;j++)
			for(i=0;i<img_width;i++)				
				SRC1(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
	}
	else if((cn1 == 3)&&(cn2 == 1))
	{
		for(j=0;j<img_height;j++)
			for(i=0;i<img_width;i++)
			{
				SRC1(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
				SRC1(i,j,1) = (SRC1(i,j,1)*k1+SRC2(i,j,0)*k2+512)>>10;
				SRC1(i,j,2) = (SRC1(i,j,2)*k1+SRC2(i,j,0)*k2+512)>>10;
			}
	}	
	else if((cn1 == 3)&&(cn2 == 3))
	{
		for(j=0;j<img_height;j++)
			for(i=0;i<img_width;i++)
			{
				SRC1(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
				SRC1(i,j,1) = (SRC1(i,j,1)*k1+SRC2(i,j,1)*k2+512)>>10;
				SRC1(i,j,2) = (SRC1(i,j,2)*k1+SRC2(i,j,2)*k2+512)>>10;
			}
	}
	else if((cn1 == 1)&&(cn2 == 3))
	{
		ImgMat *dst;
		dst = imgCreateMat(img_height,img_width,src2->type);
				
		unsigned char *p_dst;
		p_dst = dst->data.ptr;
		
		int dst_width = img_width;
		int dst_height = img_height;
		int cn = 3;
				
		for(j=0;j<img_height;j++)
			for(i=0;i<img_width;i++)
			{
				DST(i,j,0) = (SRC1(i,j,0)*k1+SRC2(i,j,0)*k2+512)>>10;
				DST(i,j,1) = (SRC1(i,j,0)*k1+SRC2(i,j,1)*k2+512)>>10;
				DST(i,j,2) = (SRC1(i,j,0)*k1+SRC2(i,j,2)*k2+512)>>10;
			}
			
		imgReleaseMatData(src1);
		free(src1->hidinfo);
		
		src1->hidinfo = dst->hidinfo;
		src1->step = dst->step;
		src1->size = dst->size;
		src1->type = dst->type;
		src1->width = dst_width;
		src1->height = dst_height;
		src1->data.ptr = p_dst;

		free(dst);
	}	
}
