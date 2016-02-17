#include <stdio.h>
#include "type.h"

#define SRC1(a,b) p_src1[(b)*img_width+(a)]
#define SRC2(a,b) p_src2[(b)*ovl_width+(a)]

#define SRC1_0(a,b) p_src1[((b)*img_width+(a))*cn1]
#define SRC1_1(a,b) p_src1[((b)*img_width+(a))*cn1+1]
#define SRC1_2(a,b) p_src1[((b)*img_width+(a))*cn1+2]

#define SRC2_0(a,b) p_src2[((b)*ovl_width+(a))*cn2]
#define SRC2_1(a,b) p_src2[((b)*ovl_width+(a))*cn2+1]
#define SRC2_2(a,b) p_src2[((b)*ovl_width+(a))*cn2+2]
#define SRC2_3(a,b) p_src2[((b)*ovl_width+(a))*cn2+3]

#define DST_0(a,b) p_dst[((b)*img_width+(a))*3]
#define DST_1(a,b) p_dst[((b)*img_width+(a))*3+1]
#define DST_2(a,b) p_dst[((b)*img_width+(a))*3+2]

ImgMat *imgCreateMat(int height,int width,char type);

void imgOverlay(ImgMat *src1,ImgMat *src2,ImgPoint locate)
{
	int img_width;
	img_width = src1->width;
	
	int img_height;
	img_height = src1->height;
	
	unsigned char *p_src1;
	p_src1 = src1->data.ptr;
	
	int ovl_width;
	ovl_width = src2->width;
	
	int ovl_height;
	ovl_height = src2->height;
	
	unsigned char *p_src2;
	p_src2 = src2->data.ptr;
	
	if((locate.x>img_width)||(locate.y>img_height))
		return;		
	
	int p1x,p1y,p2x,p2y;
	p1x = (locate.x<0)?0:locate.x;
	p1y = (locate.y<0)?0:locate.y;
	
	p2x = locate.x+ovl_width;
	p2y = locate.y+ovl_height;
	
	if((p2x<0)||(p2y<0))
		return;
	
	p2x = (p2x>img_width)?img_width:p2x;
	p2y = (p2y>img_height)?img_height:p2y;
	
	int cn1;
	cn1 = ((src1->type&0xF8)>>3)+1;
	
	int cn2;
	cn2 = ((src2->type&0xF8)>>3)+1;
	
	int k;
	
	int i,j;
	if((cn1 == 1)&&(cn2 == 1))
	{
		for(j=p1y;j<p2y;j++)
			for(i=p1x;i<p2x;i++)				
				SRC1(i,j) = SRC2(i-locate.x,j-locate.y);
	}
	else if((cn1 >= 3)&&(cn2 == 1))
	{
		for(j=p1y;j<p2y;j++)
			for(i=p1x;i<p2x;i++)
			{
				SRC1_0(i,j) = SRC2(i-locate.x,j-locate.y);
				SRC1_1(i,j) = SRC2(i-locate.x,j-locate.y);
				SRC1_2(i,j) = SRC2(i-locate.x,j-locate.y);
			}
	}	
	else if((cn1 >= 3)&&(cn2 == 3))
	{
		for(j=p1y;j<p2y;j++)
			for(i=p1x;i<p2x;i++)
			{
				SRC1_0(i,j) = SRC2_0(i-locate.x,j-locate.y);
				SRC1_1(i,j) = SRC2_1(i-locate.x,j-locate.y);
				SRC1_2(i,j) = SRC2_2(i-locate.x,j-locate.y);
			}
	}
	else if((cn1 >= 3)&&(cn2 == 4))
	{
		for(j=p1y;j<p2y;j++)
			for(i=p1x;i<p2x;i++)
			{
				k = SRC2_3(i-locate.x,j-locate.y);
				SRC1_0(i,j) = (SRC1_0(i,j)*(256-k)+SRC2_0(i-locate.x,j-locate.y)*k)>>8;
				SRC1_1(i,j) = (SRC1_1(i,j)*(256-k)+SRC2_1(i-locate.x,j-locate.y)*k)>>8;
				SRC1_2(i,j) = (SRC1_2(i,j)*(256-k)+SRC2_2(i-locate.x,j-locate.y)*k)>>8;
			}
	}
	else 
	{
		ImgMat *dst;
		dst = imgCreateMat(img_height,img_width,TYPE_8UC3);
				
		unsigned char *p_dst;
		p_dst = dst->data.ptr;
		
		if((cn1 == 1)&&(cn2 == 3))
		{			
			for(j=p1y;j<p2y;j++)
				for(i=p1x;i<p2x;i++)
				{
					DST_0(i,j) = SRC2_0(i-locate.x,j-locate.y);
					DST_1(i,j) = SRC2_1(i-locate.x,j-locate.y);
					DST_2(i,j) = SRC2_2(i-locate.x,j-locate.y);
				}
		}
		else if((cn1 == 1)&&(cn2 == 4))
		{
			for(j=p1y;j<p2y;j++)
				for(i=p1x;i<p2x;i++)
				{
					k = SRC2_3(i-locate.x,j-locate.y);
					DST_0(i,j) = (SRC1(i,j)*(256-k)+SRC2_0(i-locate.x,j-locate.y)*k)>>8;
					DST_1(i,j) = (SRC1(i,j)*(256-k)+SRC2_1(i-locate.x,j-locate.y)*k)>>8;
					DST_2(i,j) = (SRC1(i,j)*(256-k)+SRC2_2(i-locate.x,j-locate.y)*k)>>8;
				}
		}
		
		free(src1->data.ptr);
		free(src1->hidinfo);
		
		*src1 = *dst;
		
		free(dst);
	}
}
