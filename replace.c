#include <stdio.h>
#include "type.h"
#include "err.h"

#define PTR_1(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
}\

#define PTR_3(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	p1##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p1##mat[i] = p0##mat[i]+mat->size;\
	p2##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p2##mat[i] = p1##mat[i]+mat->size;\
}\

#define PTR_4(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	p1##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p1##mat[i] = p0##mat[i]+mat->size;\
	p2##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p2##mat[i] = p1##mat[i]+mat->size;\
	p3##mat = malloc((mat->height)<<2);\
	for(i=0;i<mat->height;i++)\
		p3##mat[i] = p2##mat[i]+mat->size;\
}\


#define SRC_0(x,y) *(p0src[y]+(x))
#define DST_0(x,y) *(p0dst[y]+(x))
#define SRC_1(x,y) *(p1src[y]+(x))
#define DST_1(x,y) *(p1dst[y]+(x))
#define SRC_2(x,y) *(p2src[y]+(x))
#define DST_2(x,y) *(p2dst[y]+(x))
#define SRC_3(x,y) *(p3src[y]+(x))
#define DST_3(x,y) *(p3dst[y]+(x))
#define FLAG(x,y) *(p0flag[y]+(x))

#define ABS(x) (((x)>0)?(x):(0-(x)))
#define SATUR(x) {\
	if((x)>255)\
		x = 255;\
	else if((x)<0)\
		x = 0;\
}\

ImgMat *imgCreateMat(int height,int width,char type);

void imgColorReplace_1(ImgMat *src,ImgMat *dst,ImgLocate *start_locate,ImgColor *replace_color,int thresh)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgColorReplace,src);
	DESTINATION_ERROR_CHECK(imgColorReplace,dst);
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=1)
		IMG_ERR(imgColorReplace,"number of mat channel is not correct"); 
	if(thresh<0)
		IMG_ERR(imgColorReplace,"thresh is less than 0");
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	imgCopyMat(src,dst);
	
	ImgMat *flag;
	flag = imgCreateMat(img_height,img_width,TYPE_8UC1);
	imgCleanMat(flag);
	
	int i,j;
	
	unsigned char **p0src;
	PTR_1(src);
	
	unsigned char **p0dst;
	PTR_1(dst);
	
	unsigned char **p0flag;
	PTR_1(flag);	
	
	int replace_gray;
	if(replace_color == NULL)
		replace_gray = 255;
	else
		replace_gray = replace_color->gray;
	
	int start_x;
	int start_y;	
	if(start_locate == NULL)
	{
		start_x = 0;
		start_y = 0;
	}
	else
	{
		start_x = start_locate->x;
		start_y = start_locate->y;
	}
	
	int diff;
	diff = replace_gray - SRC_0(start_x,start_y);
	int data;
	
	int mark1,mark2;
	
	FLAG(start_x,start_y) = 255;
	DST_0(start_x,start_y) = replace_gray;
	
	for(i=start_x+1;i<img_width;i++)
	{
		if(FLAG(i-1,start_y)==255)
		{			
			if(ABS(SRC_0(i,start_y)-SRC_0(i-1,start_y))<thresh)
			{
				FLAG(i,start_y) = 255;
				data = SRC_0(i,start_y) + diff;
				SATUR(data);
				DST_0(i,start_y) = data;
			}
		}
		else
			break;
	}
	for(i=start_x-1;i>=0;i--)
	{
		if(FLAG(i+1,start_y) == 255)
			if(ABS(SRC_0(i,start_y)-SRC_0(i+1,start_y))<thresh)
			{
				FLAG(i,start_y) = 255;
				data = SRC_0(i,start_y) + diff;
				SATUR(data);
				DST_0(i,start_y) = data;
			}
		else
			break;
	}
	
	start_x = i;
	
replace:
	mark2 = 0;
	for(j=start_y+1;j<img_height;j++)
	{
		mark1 = 0;				
		for(i=start_x;i<img_width;i++)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}			
			
			if(FLAG(i-1,j)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i-1,j))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j-1)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i,j-1))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;				
		}
		
		for(i=start_x;i>=0;i--)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}	
			
			if(FLAG(i-1,j)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i-1,j))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
			if(FLAG(i,j-1)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i,j-1))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		if(mark1 == 0)
			break;
	}
	
	if(mark2==0)
		goto replace_end;
		
	start_y = j-2;
	
	mark2 = 0;
	for(j=start_y-1;j>=0;j--)
	{
		mark1 = 0;				
		for(i=start_x;i<img_width;i++)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}
			
			if(FLAG(i-1,j)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i-1,j))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j+1)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i,j+1))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		for(i=start_x;i>=0;i--)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}
			
			if(FLAG(i+1,j)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i+1,j))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j+1)==255)
				if(ABS(SRC_0(i,j)-SRC_0(i,j+1))<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff;
					SATUR(data);
					DST_0(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		
		if(mark1 == 0)
			break;
	}
	
	start_y = j+2;
	
	if(mark2==1)
		goto replace;

replace_end:	
	imgReleaseMat(flag);
}

#define DIFF(x1,y1,x2,y2) ABS(SRC_0(x1,y1)-SRC_0(x2,y2))+ABS(SRC_1(x1,y1)-SRC_1(x2,y2))+ABS(SRC_2(x1,y1)-SRC_2(x2,y2))

void imgColorReplace_3(ImgMat *src,ImgMat *dst,ImgLocate *start_locate,ImgColor *replace_color,int thresh)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgColorReplace,src);
	DESTINATION_ERROR_CHECK(imgColorReplace,dst);
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=3)
		IMG_ERR(imgColorReplace,"number of mat channel is not correct"); 
	if(thresh<0)
		IMG_ERR(imgColorReplace,"thresh is less than 0");
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	imgCopyMat(src,dst);
	
	ImgMat *flag;
	flag = imgCreateMat(img_height,img_width,TYPE_8UC1);
	imgCleanMat(flag);
	
	int i,j;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	PTR_3(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	PTR_3(dst);
	
	unsigned char **p0flag;
	PTR_1(flag);	
	
	int replace_0;
	int replace_1;
	int replace_2;
	if(replace_color == NULL)
	{
		replace_0 = 255;
		replace_1 = 255;
		replace_2 = 255;
	}
	else
	{
		replace_0 = replace_color->b;
		replace_1 = replace_color->g;
		replace_2 = replace_color->r;
	}	
	
	int start_x;
	int start_y;	
	if(start_locate == NULL)
	{
		start_x = 0;
		start_y = 0;
	}
	else
	{
		start_x = start_locate->x;
		start_y = start_locate->y;
	}
	
	int diff0,diff1,diff2;
	diff0 = replace_0 - SRC_0(start_x,start_y);
	diff1 = replace_1 - SRC_1(start_x,start_y);
	diff2 = replace_2 - SRC_2(start_x,start_y);
	
	int mark1,mark2;
	int data;
	
	FLAG(start_x,start_y) = 255;
	DST_0(start_x,start_y) = replace_0;
	DST_1(start_x,start_y) = replace_1;
	DST_2(start_x,start_y) = replace_2;
	
	thresh = thresh*3;
	
	for(i=start_x+1;i<img_width;i++)
	{
		if(FLAG(i-1,start_y)==255)
		{			
			if(DIFF(i,start_y,i-1,start_y)<thresh)
			{
				FLAG(i,start_y) = 255;
				data = SRC_0(i,start_y) + diff0;
				SATUR(data);
				DST_0(i,start_y) = data;
				data = SRC_1(i,start_y) + diff1;
				SATUR(data);
				DST_1(i,start_y) = data;
				data = SRC_2(i,start_y) + diff2;
				SATUR(data);
				DST_2(i,start_y) = data;
			}
		}
		else
			break;
	}
	for(i=start_x-1;i>=0;i--)
	{
		if(FLAG(i+1,start_y) == 255)
			if(DIFF(i,start_y,i+1,start_y)<thresh)
			{
				FLAG(i,start_y) = 255;
				data = SRC_0(i,start_y) + diff0;
				SATUR(data);
				DST_0(i,start_y) = data;
				data = SRC_1(i,start_y) + diff1;
				SATUR(data);
				DST_1(i,start_y) = data;
				data = SRC_2(i,start_y) + diff2;
				SATUR(data);
				DST_2(i,start_y) = data;
			}
		else
			break;
	}
	
	start_x = i;
	
replace:
	mark2 = 0;
	for(j=start_y+1;j<img_height;j++)
	{
		mark1 = 0;				
		for(i=start_x;i<img_width;i++)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}			
			
			if(FLAG(i-1,j)==255)
				if(DIFF(i,j,i-1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j-1)==255)
				if(DIFF(i,j,i,j-1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;				
		}
		
		for(i=start_x;i>=0;i--)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}	
			
			if(FLAG(i-1,j)==255)
				if(DIFF(i,j,i-1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
			if(FLAG(i,j-1)==255)
				if(DIFF(i,j,i,j-1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		if(mark1 == 0)
			break;
	}
	
	if(mark2==0)
		goto replace_end;
		
	start_y = j-2;
	
	mark2 = 0;
	for(j=start_y-1;j>=0;j--)
	{
		mark1 = 0;				
		for(i=start_x;i<img_width;i++)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}
			
			if(FLAG(i-1,j)==255)
				if(DIFF(i,j,i-1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j+1)==255)
				if(DIFF(i,j,i,j+1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		for(i=start_x;i>=0;i--)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}
			
			if(FLAG(i+1,j)==255)
				if(DIFF(i,j,i+1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j+1)==255)
				if(DIFF(i,j,i,j+1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		
		if(mark1 == 0)
			break;
	}
	
	start_y = j+2;
	
	if(mark2==1)
		goto replace;

replace_end:	
	imgReleaseMat(flag);
}

void imgColorReplace_4(ImgMat *src,ImgMat *dst,ImgLocate *start_locate,ImgColor *replace_color,int thresh)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgColorReplace,src);
	DESTINATION_ERROR_CHECK(imgColorReplace,dst);
	#endif
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	#ifdef DEBUG
	if(cn!=3)
		IMG_ERR(imgColorReplace,"number of mat channel is not correct"); 
	if(thresh<0)
		IMG_ERR(imgColorReplace,"thresh is less than 0");
	#endif
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if((src->height != dst->height)||(src->width != dst->width)||(dst->type != TYPE_8UC1))
		imgMatRedefine(dst,src->height,src->width,TYPE_8UC1);
	
	imgCopyMat(src,dst);
	
	ImgMat *flag;
	flag = imgCreateMat(img_height,img_width,TYPE_8UC1);
	imgCleanMat(flag);
	
	int i,j;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	unsigned char **p3src;
	PTR_4(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	unsigned char **p3dst;
	PTR_4(dst);
	
	unsigned char **p0flag;
	PTR_1(flag);	
	
	int replace_0;
	int replace_1;
	int replace_2;
	int replace_3;
	if(replace_color == NULL)
	{
		replace_0 = 255;
		replace_1 = 255;
		replace_2 = 255;
		replace_3 = 255;
	}
	else
	{
		replace_0 = replace_color->b;
		replace_1 = replace_color->g;
		replace_2 = replace_color->r;
		replace_3 = replace_color->alpha;
	}
	
	int start_x;
	int start_y;
	if(start_locate == NULL)
	{
		start_x = 0;
		start_y = 0;
	}
	else
	{
		start_x = start_locate->x;
		start_y = start_locate->y;
	}
	
	int diff0,diff1,diff2;
	diff0 = replace_0 - SRC_0(start_x,start_y);
	diff1 = replace_1 - SRC_1(start_x,start_y);
	diff2 = replace_2 - SRC_2(start_x,start_y);
	
	int mark1,mark2;
	int data;
	
	FLAG(start_x,start_y) = 255;
	DST_0(start_x,start_y) = replace_0;
	DST_1(start_x,start_y) = replace_1;
	DST_2(start_x,start_y) = replace_2;
	DST_3(start_x,start_y) = replace_3;
	
	for(i=start_x+1;i<img_width;i++)
	{
		if(FLAG(i-1,start_y)==255)
		{			
			if(DIFF(i,start_y,i-1,start_y)<thresh)
			{
				FLAG(i,start_y) = 255;
				data = SRC_0(i,start_y) + diff0;
				SATUR(data);
				DST_0(i,start_y) = data;
				data = SRC_1(i,start_y) + diff1;
				SATUR(data);
				DST_1(i,start_y) = data;
				data = SRC_2(i,start_y) + diff2;
				SATUR(data);
				DST_2(i,start_y) = data;
				DST_3(i,start_y) = replace_3;
			}
		}
		else
			break;
	}
	for(i=start_x-1;i>=0;i--)
	{
		if(FLAG(i+1,start_y) == 255)
			if(DIFF(i,start_y,i+1,start_y)<thresh)
			{
				FLAG(i,start_y) = 255;
				data = SRC_0(i,start_y) + diff0;
				SATUR(data);
				DST_0(i,start_y) = data;
				data = SRC_1(i,start_y) + diff1;
				SATUR(data);
				DST_1(i,start_y) = data;
				data = SRC_2(i,start_y) + diff2;
				SATUR(data);
				DST_2(i,start_y) = data;
				DST_3(i,start_y) = replace_3;
			}
		else
			break;
	}
	
	start_x = i;
	
replace:
	mark2 = 0;
	for(j=start_y+1;j<img_height;j++)
	{
		mark1 = 0;				
		for(i=start_x;i<img_width;i++)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}			
			
			if(FLAG(i-1,j)==255)
				if(DIFF(i,j,i-1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j-1)==255)
				if(DIFF(i,j,i,j-1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;				
		}
		
		for(i=start_x;i>=0;i--)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}	
			
			if(FLAG(i-1,j)==255)
				if(DIFF(i,j,i-1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
			if(FLAG(i,j-1)==255)
				if(DIFF(i,j,i,j-1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		if(mark1 == 0)
			break;
	}
	
	if(mark2==0)
		goto replace_end;
		
	start_y = j-2;
	
	mark2 = 0;
	for(j=start_y-1;j>=0;j--)
	{
		mark1 = 0;				
		for(i=start_x;i<img_width;i++)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}
			
			if(FLAG(i-1,j)==255)
				if(DIFF(i,j,i-1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j+1)==255)
				if(DIFF(i,j,i,j+1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		for(i=start_x;i>=0;i--)
		{
			if(FLAG(i,j) == 255)
			{
				mark1 = 1;
				continue;
			}
			
			if(FLAG(i+1,j)==255)
				if(DIFF(i,j,i+1,j)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}

			if(FLAG(i,j+1)==255)
				if(DIFF(i,j,i,j+1)<thresh)
				{
					FLAG(i,j) = 255;
					data = SRC_0(i,j) + diff0;
					SATUR(data);
					DST_0(i,j) = data;
					data = SRC_1(i,j) + diff1;
					SATUR(data);
					DST_1(i,j) = data;
					data = SRC_2(i,j) + diff2;
					SATUR(data);
					DST_2(i,j) = data;
					DST_3(i,j) = replace_3;
					mark1 = 1;
					mark2 = 1;
					continue;
				}
				
			start_x = i;
		}
		
		
		if(mark1 == 0)
			break;
	}
	
	start_y = j+2;
	
	if(mark2==1)
		goto replace;

replace_end:	
	imgReleaseMat(flag);
}

void imgColorReplace(ImgMat *src,ImgMat *dst,ImgLocate *start_locate,ImgColor *replace_color,int thresh)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgColorReplace,src);
	#endif
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	if(cn==1)
		imgColorReplace_1(src,dst,start_locate,replace_color,thresh);
	else if(cn==3)
		imgColorReplace_3(src,dst,start_locate,replace_color,thresh);	
	else if(cn==4)	
		imgColorReplace_4(src,dst,start_locate,replace_color,thresh);
}
			
void ColorReplace(ImgMat *src,ImgMat *dst,ImgLocate *start_locate,ImgColor *replace_color,int thresh)
{		
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgColorReplace,src);
	#endif
	
	printf("color is %d,%d,%d\n",replace_color->r,replace_color->g,replace_color->b);
	
	if(dst==NULL)
	{
		// printf("aaaaaaaaaaaaa\n");
		dst = imgCreateMat(src->height,src->width,src->type);
		imgColorReplace(src,dst,start_locate,replace_color,thresh);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgColorReplace(src,dst,start_locate,replace_color,thresh);
}
