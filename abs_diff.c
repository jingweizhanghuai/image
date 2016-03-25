#include <stdio.h>
#include "type.h"
#include "err.h"

#define ABS(x) (((x)>0)?(x):(0-(x)))

void imgAbsDiff(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *average_diff)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgSub,src1);
	SOURCE_ERROR_CHECK(imgSub,src2);
	DESTINATION_ERROR_CHECK(imgSub,dst);
	#endif
	
	int img_width;
	img_width = src1->width;
	
	int img_height;
	img_height = src1->height;
	
	#ifdef DEBUG
	if((src2->width != img_width)||(src2->height != img_height)||(src1->type != src2->type))
	{
		printf("IMG Error:\n\tin imgAbsDiff.\n");
		exit(0);
	}
	#endif
	
	if((dst->width != img_width)||(dst->height != img_height)||(dst->type != src2->type))
		imgMatRedefine(dst,img_height,img_width,src1->type);
	
	int img_size;
	img_size = src1->size;
	
	int cn;
	cn = ((src1->type&0xF8)>>3)+1;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	int i,k;
	
	int sum;
	
	if(src1->type == TYPE_8U)
	{
		unsigned char *p_src1_1;
		p_src1_1 = src1->data.ptr;
		unsigned char *p_src2_1;
		p_src2_1 = src2->data.ptr;
		
		if(average_diff==NULL)
		{
			for(k=0;k<cn;k++)
			{
				for(i=0;i<img_size;i++)
				{
					*p_dst = ABS((*p_src1_1)-(*p_src2_1));
					
					p_src1_1 = p_src1_1 +1;
					p_src2_1 = p_src2_1 +1;
					p_dst = p_dst +1;
				}
		
				if(k<(cn-1))
				{
					p_src1_1 = p_src1_1+img_size;
					p_src2_1 = p_src2_1+img_size;
				}
			}
		}
		else
		{
			sum = 0;
			for(k=0;k<cn;k++)
			{
				for(i=0;i<img_size;i++)
				{
					*p_dst = ABS((*p_src1_1)-(*p_src2_1));
					
					sum = sum+(*p_dst);
					
					p_src1_1 = p_src1_1 +1;
					p_src2_1 = p_src2_1 +1;
					p_dst = p_dst +1;
				}
		
				if(k<(cn-1))
				{
					p_src1_1 = p_src1_1+img_size;
					p_src2_1 = p_src2_1+img_size;
				}
			}
			*average_diff = (float)sum/(float)(img_size*cn);			
		}
	}
	else if(src1->type == TYPE_8S)
	{
		char *p_src1_2;
		p_src1_2 = (char *)src1->data.ptr;
		char *p_src2_2;
		p_src2_2 = (char *)src2->data.ptr;		
		
		if(average_diff==NULL)
		{
			for(k=0;k<cn;k++)
			{
				for(i=0;i<img_size;i++)
				{
					*p_dst = ABS((*p_src1_2)-(*p_src2_2));
					
					p_src1_2 = p_src1_2 +1;
					p_src2_2 = p_src2_2 +1;
					p_dst = p_dst +1;
				}
		
				if(k<(cn-1))
				{
					p_src1_2 = p_src1_2+img_size;
					p_src2_2 = p_src2_2+img_size;
				}
			}
		}
		else
		{
			sum = 0;
			for(k=0;k<cn;k++)
			{
				for(i=0;i<img_size;i++)
				{
					*p_dst = ABS((*p_src1_2)-(*p_src2_2));
					
					sum = sum+(*p_dst);
					
					p_src1_2 = p_src1_2 +1;
					p_src2_2 = p_src2_2 +1;
					p_dst = p_dst +1;
				}
		
				if(k<(cn-1))
				{
					p_src1_2 = p_src1_2+img_size;
					p_src2_2 = p_src2_2+img_size;
				}
			}
			*average_diff = (float)sum/(float)(img_size*cn);			
		}
	}
}

ImgMat *imgCreateMat(int height,int width,char type);
	
void AbsDiff(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *average_diff)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAbsDiff,src1);
	#endif
	
	if(dst == NULL)
	{
		imgAbsDiff(src1,src2,src1,average_diff);
		int type = src1->type;
		if((type&0x07)!= TYPE_8U)
		{
			type = (src1->type)&0xf8;
			src1->type = type;
			*(src1->hidinfo + 4) = type;
		}
	}
	else
		imgAbsDiff(src1,src2,dst,average_diff);
}
