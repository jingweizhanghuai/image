#include <stdio.h>
#include "type.h"
#include "err.h"

void imgThreshold(ImgMat *src,ImgMat *dst,int cn,ImgThreshold *threshold)//int *thresh,int *value)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgThreshold,src);
	DESTINATION_ERROR_CHECK(imgThreshold,dst);
	if(threshold==NULL)
	{
		printf("IMG Error:\n\tin imgThreshold:\n");
		exit(0);
	}
	#endif
	
	int thresh_num = threshold->thresh_num;
	
	int i,j,k;
	
	int img_size;
	img_size = src->width*src->height;
	
	int type_cn;
	type_cn = ((src->type)>>3)+1;
	
	unsigned char *p_src;
	p_src = src->data.ptr+cn;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr+cn;
	
	int thresh[11];
	int value[10];
	thresh[0] = 0;
	for(k=0;k<thresh_num;k++)
	{
		value[k] = threshold->value[k];
		thresh[k+1] = threshold->thresh[k];
	}
	value[k] = threshold->value[k];
	thresh[k+1] = 256;
	
	int data;
	
	printf("thresh[0] is %d\n",thresh[0]);
	printf("value is %d,%d\n",value[0],value[1]);
	
	for(j=0;j<type_cn;j++)
	{
		if((j+1)!=cn)
			copy_mat_data(p_src,p_dst,img_size);
		else
		{
			for(i=0;i<img_size;i++)
			{
				data = p_src[i];
		
				for(k=0;k<thresh_num+1;k++)
					if((data>=thresh[k])&&(data<thresh[k+1]))
					{
						if(value[k] == NOT_CHANGE_WITH_THRESHOLD)
							p_dst[i] = data;
						else
							p_dst[i] = value[k];
												
						break;
					}
			}
		}
			
		p_src = p_src+img_size;
		p_dst = p_dst+img_size;
	}
}
	
ImgMat *imgCreateMat(int height,int width,char type);
	
void Threshold(ImgMat *src,ImgMat *dst,int cn,ImgThreshold *threshold)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgThreshold,src);
	#endif
	
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgThreshold(src,dst,cn,threshold);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgThreshold(src,dst,cn,threshold);
}	
	
	
	
