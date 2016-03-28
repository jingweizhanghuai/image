#include <stdio.h>
#include "type.h"
#include "err.h"

#define PTR(mat) {\
	p##mat[0] = p_##mat;\
	for(i=1;i<mat->height;i++)\
		p##mat[i] = p##mat[i-1]+mat->width;\
}\

#define HF(x,y) *(phf[y]+(x))
#define HFABS(x,y) (((HF(x,y))>0)?(HF(x,y)):(0-(HF(x,y))))

void detail_enhance(ImgMat *hf,int thresh,float strength)
{	
	int img_height;
	img_height = hf->height;
	
	int img_width;
	img_width = hf->width;
	
	char *p_hf;
	p_hf = (char *)hf->data.ptr;
	
	char **phf;
	phf = malloc(img_height<<2);
	
	int sum;
	int data;
	
	int cn;
	cn = ((hf->type)>>3)+1;
	
	int i,j,k;
	
	for(k=0;k<cn;k++)
	{
		PTR(hf);

		for(j=1;j<img_height-1;j++)
		{	
			for(i=1;i<img_width;i++)
			{
				if(HFABS(i,j)<thresh)
				{
					HF(i,j) = 0;
					continue;
				}
				
				sum = (HFABS(i-1,j-1)>thresh)+(HFABS(i,j-1)>thresh)+(HFABS(i+1,j-1)>thresh)+
					   (HFABS(i-1,j)>thresh)+(HFABS(i,j)>thresh)+(HFABS(i+1,j)>thresh)+
					   (HFABS(i-1,j+1)>thresh)+(HFABS(i,j+1)>thresh)+(HFABS(i+1,j+1)>thresh);
				
				if(sum<3)
					HF(i,j) = 0;
				else
				{
					data = (int)((float)HF(i,j)*strength);
					
					if(data>127)
						data = 127;
					else if(data<-128)
						data = -128;	
					
					HF(i,j) = data;
				}			
			}
		}
		p_hf = p_hf + hf->size;
	}
}

ImgMat *imgCreateMat(int height,int width,char type);

void imgDetailEnhance(ImgMat *src,ImgMat *dst,int r,int thresh,float strength)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgFrequenceSeparate,src);
	DESTINATION_ERROR_CHECK(imgFrequenceSeparate,dst);
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((dst->height != img_height)||(dst->width != img_width)||(dst->type != src->type))
		imgMatRedefine(dst,img_height,img_width,src->type);
	
	int hf_type;
	hf_type = (src->type)&0x01;
	
	ImgMat *hf;
	hf = imgCreateMat(img_height,img_width,hf_type);
	
	imgFrequenceSeparate(src,dst,hf,r);
	
	detail_enhance(hf,thresh,strength);
	
	Add(dst,hf,NULL);
	
	imgReleaseMat(hf);
}
	


