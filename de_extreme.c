#include <stdio.h>
#include "type.h"
#include "err.h"

void maxdata(int a,int b,int c,int d,int *max)
{
	int data1_1,data1_2;
	if(a>b)
	{
		data1_1 = a;
		data1_2 = b;
	}
	else
	{
		data1_1 = b;
		data1_2 = a;
	}
	
	int data2_1,data2_2;
	if(c>d)
	{
		data2_1 = c;
		data2_2 = d;
	}
	else
	{
		data2_1 = d;
		data2_2 = c;
	}
	
	if(data1_1>data2_1)
	{
		*max = data1_1;
		*(max+1) = (data1_2>data2_1)?data1_2:data2_1;
	}
	else
	{
		*max = data2_1;
		*(max+1) = (data2_2>data1_1)?data2_2:data1_1;
	}
}

void mindata(int a,int b,int c,int d,int *min)
{
	int data1_1,data1_2;
	if(a<b)
	{
		data1_1 = a;
		data1_2 = b;
	}
	else
	{
		data1_1 = b;
		data1_2 = a;
	}
	
	int data2_1,data2_2;
	if(c<d)
	{
		data2_1 = c;
		data2_2 = d;
	}
	else
	{
		data2_1 = d;
		data2_2 = c;
	}
	
	if(data1_1<data2_1)
	{
		*min = data1_1;
		*(min+1) = (data1_2<data2_1)?data1_2:data2_1;
	}
	else
	{
		*min = data2_1;
		*(min+1) = (data2_2<data1_1)?data2_2:data1_1;
	}
}

#define PTR(mat) {\
	p##mat[0] = p_##mat;\
	for(i=1;i<mat->height;i++)\
		p##mat[i] = p##mat[i-1]+mat->width;\
}\

#define SRC(x,y) *(psrc[y]+(x))
#define DST(x,y) *(pdst[y]+(x))

#define ABS(x) (((x)>0)?(x):(0-(x)))

void imgDeExtreme(ImgMat *src,ImgMat *dst)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgDeExtreme,src);
	DESTINATION_ERROR_CHECK(imgDeExtreme,dst);
	#endif
	
	int img_height;
	img_height = src->height;

	int img_width;
	img_width = src->width;
	
	if((dst->height != img_height)||(dst->width != img_width)||(dst->type != src->type))
		imgMatRedefine(dst,img_height,img_width,src->type);
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char **psrc;
	unsigned char **pdst;
	
	int data0,data1,data2,data3,data4,data5,data6,data7,data8;
	int sum_x,sum_y;
	
	int cn;
	cn = ((src->type)>>3)+1;
	
	int i,j,k;
	
	for(k=0;k<cn;k++)
	{
		PTR(src);
		PTR(dst);
		for(j=1;j<img_height-1;j++)
		{
			data2 = SRC(0,j-1);
			data3 = SRC(1,j-1);
			data0 = SRC(0,j);
			data5 = SRC(1,j);
			data7 = SRC(0,j+1);
			data8 = SRC(1,j+1);
			
			for(i=1;i<img_width;i++)
			{
				data1 = data2;
				data2 = data3;
				data3 = SRC(i+1,j-1);
				
				data4 = data0;
				data0 = data5;
				data5 = SRC(i+1,j);
				
				data6 = data7;
				data7 = data8;
				data8 = SRC(i+1,j+1);
				
				sum_x = ABS(data1-data2)+ABS(data2-data3)+ABS(data4-data0)+ABS(data0-data5)+ABS(data6-data7)+ABS(data7-data8);
				sum_y = ABS(data1-data4)+ABS(data4-data6)+ABS(data2-data0)+ABS(data0-data7)+ABS(data3-data5)+ABS(data5-data8);
				
				
				// if((i==130)&&(j==90))
				// {
					// printf("sum_x is %d\n",sum_x);
					// printf("sum_y is %d\n",sum_y);
					// printf("data0 is %d\n",data0);
				// }
				
				if(sum_y-sum_x>20)
				{
					if(((data0>data4)&&(data0>data5))||((data0<data4)&&(data0<data5)))
						SRC(i,j) = (data4+data5)>>1;				
				}
				else if(sum_x-sum_y>20)
				{
					if(((data0>data2)&&(data0>data7))||((data0<data2)&&(data0<data7)))
						SRC(i,j) = (data2+data7)>>1;
				}
				else
				{
					if((data0>data2)&&(data0>data7)&&(data0>data4)&&(data0>data5))
					{
						int max[2];
						maxdata(data2,data7,data4,data5,max);
						SRC(i,j) = (max[0]+max[1])>>1;
					}
					else if((data0<data2)&&(data0<data7)&&(data0<data4)&&(data0<data5))
					{
						int min[2];
						mindata(data2,data7,data4,data5,min);
						SRC(i,j) = (min[0]+min[1])>>1;
						// if((i==130)&&(j==90))
						// {
							// printf("data is %d,%d,%d,%d\n",data2,data7,data4,data5);
							// printf("min is %d,%d\n",min[0],min[1]);
							// printf("SRC(i,j) is %d\n",SRC(i,j));
						// }
						
					}
				}				
			}
		}
		
		p_src= p_src+src->size;
		p_dst= p_dst+dst->size;
	}
}

ImgMat *imgCreateMat(int height,int width,char type);

void DeExtreme(ImgMat *src,ImgMat *dst)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgDeExtreme,src);
	#endif
	
	if(dst==NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgDeExtreme(src,dst);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgDeExtreme(src,dst);
}

