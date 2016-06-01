#include <stdio.h>
#include "type.h"
#include "err.h"
#include "ocl_support.h"

#define PTR(mat) {\
	p0##mat = malloc((mat->height)<<2);\
	p0##mat[0] = mat->data.ptr;\
	for(i=1;i<mat->height;i++)\
		p0##mat[i] = p0##mat[i-1]+mat->width;\
	if(cn>1)\
	{\
		p1##mat = malloc((mat->height)<<2);\
		for(i=0;i<mat->height;i++)\
			p1##mat[i] = p0##mat[i]+mat->size;\
	}\
	if(cn>2)\
	{\
		p2##mat = malloc((mat->height)<<2);\
		for(i=0;i<mat->height;i++)\
			p2##mat[i] = p1##mat[i]+mat->size;\
	}\
	if(cn>3)\
	{\
		p3##mat = malloc((mat->height)<<2);\
		for(i=0;i<mat->height;i++)\
			p3##mat[i] = p2##mat[i]+mat->size;\
	}\
}\

#define SRC_0(x,y) *(p0src[y]+(x))
#define DST_0(x,y) *(p0dst[y]+(x))
#define SRC_1(x,y) *(p1src[y]+(x))
#define DST_1(x,y) *(p1dst[y]+(x))
#define SRC_2(x,y) *(p2src[y]+(x))
#define DST_2(x,y) *(p2dst[y]+(x))
#define SRC_3(x,y) *(p3src[y]+(x))
#define DST_3(x,y) *(p3dst[y]+(x))

#define PTR_FREE(mat) {\
	free(p0##mat);\
	if(cn>1)\
		free(p1##mat);\
	if(cn>2)\
		free(p2##mat);\
	if(cn>3)\
		free(p3##mat);\
}\

int imgLinearEquation(float *data,float *answer,int num);			
				
void imgAffine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{	
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAffine,src);
	DESTINATION_ERROR_CHECK(imgAffine,dst);
	#endif
	
	if((ps==NULL)&&(pd==NULL))
	{
		Resize(src,dst,0,0);
		return;
	}

	if(src->memory_valid[0]==0)
		imgReadMatOCLMemory(src);
		
	int i,j;
	
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int dst_width;
	dst_width = dst->width;
	
	int dst_height;
	dst_height = dst->height;
	
	if(dst->type != src->type)
		imgMatRedefine(dst,dst_height,dst_width,src->type);
	
	ImgPoint point_s[3];
	if(ps!=NULL)
	{
		point_s[0] = ps[0];
		point_s[1] = ps[1];
		point_s[2] = ps[2];
	}
	else
	{
		point_s[0].x = 0;
		point_s[0].y = 0;
		point_s[1].x = src_width;
		point_s[1].y = 0;
		point_s[2].x = 0;
		point_s[2].y = src_height;
	}
	
	ImgPoint point_d[3];
	if(pd!=NULL)
	{
		point_d[0] = pd[0];
		point_d[1] = pd[1];
		point_d[2] = pd[2];
	}
	else
	{
		point_d[0].x = 0;
		point_d[0].y = 0;
		point_d[1].x = dst_width;
		point_d[1].y = 0;
		point_d[2].x = 0;
		point_d[2].y = dst_height;
	}
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	unsigned char **p3src;
	PTR(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	unsigned char **p3dst;
	PTR(dst);
	
	float answer[3];
	
	float data1[12] =  {(float)point_d[0].x,(float)point_d[0].y,1.0,0.0-point_s[0].x,
						(float)point_d[1].x,(float)point_d[1].y,1.0,0.0-point_s[1].x,
						(float)point_d[2].x,(float)point_d[2].y,1.0,0.0-point_s[2].x};	
	
	if(imgLinearEquation(data1,answer,3)==0)
	{
		printf("IMG Error\n\tin imgAffine:points given is wrong.\n");
		exit(0);
	}
	
	float k1,k2,b1;
	k1 = answer[0];
	k2 = answer[1];
	b1 = answer[2];
	
	float data2[12] =  {(float)point_d[0].x,(float)point_d[0].y,1.0,0.0-point_s[0].y,
						(float)point_d[1].x,(float)point_d[1].y,1.0,0.0-point_s[1].y,
						(float)point_d[2].x,(float)point_d[2].y,1.0,0.0-point_s[2].y};
						
	if(imgLinearEquation(data2,answer,3)==0)
	{
		printf("IMG Error\n\tin imgAffine:points given is wrong.\n");
		exit(0);
	}
	
	float k3,k4,b2;
	k3 = answer[0];
	k4 = answer[1];
	b2 = answer[2];
	
	// printf("ps is (%d,%d),(%d,%d),(%d,%d)\n", point_s[0].x,point_s[0].y,point_s[1].x,point_s[1].y,point_s[2].x,point_s[2].y);
	// printf("pd is (%d,%d),(%d,%d),(%d,%d)\n", point_d[0].x,point_d[0].y,point_d[1].x,point_d[1].y,point_d[2].x,point_d[2].y);
	// printf("xs = %f*xd + %f*yd + %f\n",k1,k2,b1);
	// printf("ys = %f*xd + %f*yd + %f\n",k3,k4,b2);
	
	float x,y;
	int x1,y1,x2,y2;
	float wx,wy;
	float w1,w2,w3,w4;
	
	if(cn == 1)
	{
		for(j=0;j<dst_height;j++)
		{
			x = (float)j*k2+b1;
			y = (float)j*k4+b2;
			
			for(i=0;i<dst_width;i++)
			{
				if((x>=(src_width-1))||(x<0)||(y>=(src_height-1))||(y<0))
				{
					DST_0(i,j) = 0;
					
					x = x+k1;
					y = y+k3;
					
					continue;
				}
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}
	else if(cn == 2)
	{
		for(j=0;j<dst_height;j++)
		{
			x = (float)j*k2+b1;
			y = (float)j*k4+b2;
			
			// printf("j is %d\n",j);
			
			for(i=0;i<dst_width;i++)
			{
				if((x>=(src_width-1))||(x<0)||(y>=(src_height-1))||(y<0))
				{
					DST_0(i,j) = 0;
					
					x = x+k1;
					y = y+k3;
					
					continue;
				}
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}	
	else if(cn == 3)
	{
		for(j=0;j<dst_height;j++)
		{
			x = (float)j*k2+b1;
			y = (float)j*k4+b2;
			
			// printf("j is %d\n",j);
			
			for(i=0;i<dst_width;i++)
			{
				if((x>=(src_width-1))||(x<0)||(y>=(src_height-1))||(y<0))
				{
					DST_0(i,j) = 0;
					
					x = x+k1;
					y = y+k3;
					
					continue;
				}
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
				DST_2(i,j) =(int)((float)SRC_2(x1,y1)*w1+(float)SRC_2(x2,y1)*w2+(float)SRC_2(x2,y2)*w3+(float)SRC_2(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}	
	else if(cn == 4)
	{
		for(j=0;j<dst_height;j++)
		{
			x = (float)j*k2+b1;
			y = (float)j*k4+b2;
			
			for(i=0;i<dst_width;i++)
			{
				if((x>=(src_width-1))||(x<0)||(y>=(src_height-1))||(y<0))
				{
					DST_0(i,j) = 0;
					
					x = x+k1;
					y = y+k3;
					
					continue;
				}
				
				x1 = (int)x;
				x2 = x1+1;
				y1 = (int)y;
				y2 = y1+1;
				
				wx = x-(float)x1;
				wy = y-(float)y1;				
				
				w1 = (1-wx)*(1-wy);
				w2 = wx*(1-wy);
				w3 = wx*wy;
				w4 = (1-wx)*wy;
				
				DST_0(i,j) =(int)((float)SRC_0(x1,y1)*w1+(float)SRC_0(x2,y1)*w2+(float)SRC_0(x2,y2)*w3+(float)SRC_0(x1,y2)*w4);
				DST_1(i,j) =(int)((float)SRC_1(x1,y1)*w1+(float)SRC_1(x2,y1)*w2+(float)SRC_1(x2,y2)*w3+(float)SRC_1(x1,y2)*w4);
				DST_2(i,j) =(int)((float)SRC_2(x1,y1)*w1+(float)SRC_2(x2,y1)*w2+(float)SRC_2(x2,y2)*w3+(float)SRC_2(x1,y2)*w4);
				DST_3(i,j) =(int)((float)SRC_3(x1,y1)*w1+(float)SRC_3(x2,y1)*w2+(float)SRC_3(x2,y2)*w3+(float)SRC_3(x1,y2)*w4);
				
				x = x+k1;
				y = y+k3;
			}
		}
	}
	PTR_FREE(src);
	PTR_FREE(dst);
	
	dst->memory_valid[0] = 1;
	dst->memory_valid[1] = 0;
}

// extern cl_program program_affine;
// extern cl_kernel kernel_affine_cn1;
// extern cl_kernel kernel_affine_cn2;
// extern cl_kernel kernel_affine_cn3;
// extern cl_kernel kernel_affi

#include "img_opencl/affine.cl.h"

void imgAffine_cl(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgMeanFilter,src);
	DESTINATION_ERROR_CHECK(imgMeanFilter,dst);
	#endif
	
	if((ps==NULL)&&(pd==NULL))
	{
		Resize(src,dst,0,0);
		return;
	}
	
	if(src->memory_valid[1] == 0)
		imgWriteMatOCLMemory(src);
		
	int i,j;
	
	int src_width;
	src_width = src->width;
	
	int src_height;
	src_height = src->height;
	
	int dst_width;
	dst_width = dst->width;
	
	int dst_height;
	dst_height = dst->height;
	
	if(dst->type != src->type)
		imgMatRedefine(dst,dst_height,dst_width,src->type);
	
	if(dst->memory[1] == NULL)
		imgCreateMatOCLMemory(dst);
	
	ImgPoint point_s[3];
	if(ps!=NULL)
	{
		point_s[0] = ps[0];
		point_s[1] = ps[1];
		point_s[2] = ps[2];
	}
	else
	{
		point_s[0].x = 0;
		point_s[0].y = 0;
		point_s[1].x = src_width;
		point_s[1].y = 0;
		point_s[2].x = 0;
		point_s[2].y = src_height;
	}
	
	ImgPoint point_d[3];
	if(pd!=NULL)
	{
		point_d[0] = pd[0];
		point_d[1] = pd[1];
		point_d[2] = pd[2];
	}
	else
	{
		point_d[0].x = 0;
		point_d[0].y = 0;
		point_d[1].x = dst_width;
		point_d[1].y = 0;
		point_d[2].x = 0;
		point_d[2].y = dst_height;
	}
	
	int cn;
	cn = ((src->type&0xF8)>>3)+1;
	
	unsigned char **p0src;
	unsigned char **p1src;
	unsigned char **p2src;
	unsigned char **p3src;
	PTR(src);
	
	unsigned char **p0dst;
	unsigned char **p1dst;
	unsigned char **p2dst;
	unsigned char **p3dst;
	PTR(dst);

	float answer[3];
	
	float data1[12] =  {(float)point_d[0].x,(float)point_d[0].y,1.0,0.0-point_s[0].x,
						(float)point_d[1].x,(float)point_d[1].y,1.0,0.0-point_s[1].x,
						(float)point_d[2].x,(float)point_d[2].y,1.0,0.0-point_s[2].x};	

	if(imgLinearEquation(data1,answer,3)==0)
	{
		printf("IMG Error\n\tin imgAffine:points given is wrong.\n");
		exit(0);
	}
	
	float k1,k2,b1;
	k1 = answer[0];
	k2 = answer[1];
	b1 = answer[2];
	
	float data2[12] =  {(float)point_d[0].x,(float)point_d[0].y,1.0,0.0-point_s[0].y,
						(float)point_d[1].x,(float)point_d[1].y,1.0,0.0-point_s[1].y,
						(float)point_d[2].x,(float)point_d[2].y,1.0,0.0-point_s[2].y};
					
	if(imgLinearEquation(data2,answer,3)==0)
	{
		printf("IMG Error\n\tin imgAffine:points given is wrong.\n");
		exit(0);
	}
	
	cl_kernel *kernel_ptr;
	char *kernel_name;
	if(cn==1)
	{
		kernel_ptr = &kernel_affine_cn1;
		kernel_name = "affine_cn1";
	}
	else if(cn==2)
	{
		kernel_ptr = &kernel_affine_cn2;
		kernel_name = "affine_cn2";
	}
	else if(cn==3)
	{
		kernel_ptr = &kernel_affine_cn3;
		kernel_name = "affine_cn3";
	}
	else if(cn==4)
	{
		kernel_ptr = &kernel_affine_cn4;
		kernel_name = "affine_cn4";
	}
	
	float k3,k4,b2;
	k3 = answer[0];
	k4 = answer[1];
	b2 = answer[2];
	
	// printf("ps is (%d,%d),(%d,%d),(%d,%d)\n", point_s[0].x,point_s[0].y,point_s[1].x,point_s[1].y,point_s[2].x,point_s[2].y);
	// printf("pd is (%d,%d),(%d,%d),(%d,%d)\n", point_d[0].x,point_d[0].y,point_d[1].x,point_d[1].y,point_d[2].x,point_d[2].y);
	// printf("xs = %f*xd + %f*yd + %f\n",k1,k2,b1);
	// printf("ys = %f*xd + %f*yd + %f\n",k3,k4,b2);

	char *kernel_source;
	INIT_KERNEL_SOURCE;
	
	if((*kernel_ptr == NULL)||(program_affine == NULL))
		imgOCLBuildKernel(kernel_source,kernel_name,&program_affine,kernel_ptr);
	
	cl_int ret;
	ret = clSetKernelArg(*kernel_ptr,0,sizeof(cl_mem),src->memory[1]);
	ret = ret | clSetKernelArg(*kernel_ptr,1,sizeof(cl_mem),dst->memory[1]);
	ret = ret | clSetKernelArg(*kernel_ptr,2,4,&src_width);
	ret = ret | clSetKernelArg(*kernel_ptr,3,4,&src_height);
	ret = ret | clSetKernelArg(*kernel_ptr,4,4,&dst_width);
	ret = ret | clSetKernelArg(*kernel_ptr,5,4,&dst_height);	
	ret = ret | clSetKernelArg(*kernel_ptr,6,4,&k1);
	ret = ret | clSetKernelArg(*kernel_ptr,7,4,&k2);
	ret = ret | clSetKernelArg(*kernel_ptr,8,4,&b1);
	ret = ret | clSetKernelArg(*kernel_ptr,9,4,&k3);
	ret = ret | clSetKernelArg(*kernel_ptr,10,4,&k4);
	ret = ret | clSetKernelArg(*kernel_ptr,11,4,&b2);	
	#ifdef DEBUG
	if(ret != CL_SUCCESS)
	{
		printf("IMG Error:\n\tin imgMeanFilter: Set arguments error with opencl error %d.\n",ret);
		exit(0);
	}
	#endif

	RUN_DEVICE(*kernel_ptr,dst_width,dst_height,1);
	
	dst->memory_valid[0] = 0;
	dst->memory_valid[1] = 1;
}	

ImgMat *imgCreateMat(int height,int width,char type);

void Affine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd)
{
	#ifdef DEBUG
	SOURCE_ERROR_CHECK(imgAffine,src);
	#endif
	
	if((ps==NULL)&&(pd==NULL)&&(dst==NULL))
		return;
	
	if(dst == NULL)
	{
		dst = imgCreateMat(src->height,src->width,src->type);
		imgAffine(src,ps,dst,pd);
		free(src->data.ptr);
		free(src->hidinfo);
		*src = *dst;
		free(dst);
	}
	else
		imgAffine(src,ps,dst,pd);
}


