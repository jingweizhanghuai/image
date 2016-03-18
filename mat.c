#include <stdio.h>
#include <stdlib.h>

#include "type.h"

void imgPrintMat(void *mat)
{
	ImgMat *src;
	src = (ImgMat *)mat;

	int channel;
	channel = ((src->type&0xF8)>>3)+1;

	int n;
	n = src->height*src->width*channel;

	int type;
	type = src->type&0x07;

	int i,j;

	if(type==TYPE_8U)
	{
		unsigned char *data_point_8u;
		data_point_8u = (unsigned char *)src->data.ptr;
		for(i=0;i<n;i=i+channel)
		{
			for(j=0;j<channel;j++)
	printf("%3d ",data_point_8u[i+j]);
			printf(" | ");
			if(((i+channel)%(src->width*channel))==0)
	printf("\n");
		}
	}

	else if(type==TYPE_8S)
	{
		char *data_point_8s;
		data_point_8s = (char *)src->data.ptr;
		for(i=0;i<n;i=i+channel)
		{
			for(j=0;j<channel;j++)
				printf("%5d ",data_point_8s[i+j]);
			printf(" | ");
			if(((i+channel)%(src->width*channel))==0)
				printf("\n");
		}
	}

	else if(type==TYPE_16U)
	{
		unsigned short *data_point_16u;
		data_point_16u = (unsigned short *)src->data.s;
		for(i=0;i<n;i=i+channel)
		{
			for(j=0;j<channel;j++)
				printf("%5d ",data_point_16u[i+j]);
			printf(" | ");
			if(((i+channel)%(src->width*channel))==0)
				printf("\n");
		}
	}

	else if(type==TYPE_16S)
	{
		short *data_point_16s;
		data_point_16s = (short *)src->data.s;
		for(i=0;i<n;i=i+channel)
		{
			for(j=0;j<channel;j++)
				printf("%5d ",data_point_16s[i+j]);
			printf(" | ");
			if(((i+channel)%(src->width*channel))==0)
				printf("\n");
		}
	}

	else if(type==TYPE_32S)
	{
		int *data_point_32s;
		data_point_32s = (int *)src->data.i;
		for(i=0;i<n;i=i+channel)
		{
			for(j=0;j<channel;j++)
				printf("%5d ",data_point_32s[i+j]);
			printf(" | ");
			if(((i+channel)%(src->width*channel))==0)
				printf("\n");
		}
	}

	else if(type==TYPE_32F)
	{
		float *data_point_32f;
		data_point_32f = (float *)src->data.fl;
		for(i=0;i<n;i=i+channel)
		{
			for(j=0;j<channel;j++)
				printf("%5f ",data_point_32f[i+j]);
			printf(" | ");
				if(((i+channel)%(src->width*channel))==0)
			printf("\n");
		}
	}

	else if(type==TYPE_64F)
	{
		double *data_point_64f;
		data_point_64f = (double *)src->data.db;
		for(i=0;i<n;i=i+channel)
		{
			for(j=0;j<channel;j++)
				printf("%5f ",data_point_64f[i+j]);
			printf(" | ");
			if(((i+channel)%(src->width*channel))==0)
				printf("\n");
		}
	}
}

void imgCreateMatData(ImgMat *p)
{
	int n;
	n=p->height*p->step;
	
	n = n+256;

	int *data;
	data = (int *)malloc(n);

#ifdef DEBUG
	if(data ==NULL)
	{
		printf("MXU error:\n\tin imgCreateMatData:outof memery space!\n");
		exit(0);
	}
#endif

	*(p->hidinfo + 1) = 1;
	*(p->hidinfo + 7) =(int)data; 

	p->data.i=data;
	
	if((int)p != *(p->hidinfo))
	{
		ImgMat *mat;
		mat = (ImgMat *)*(p->hidinfo); 
		
		mat->data.i = (int *)data;
	}
	
}

void imgInitializeMatHeader(ImgMat *image,int height,int width,int type)
{
	// image->data.i=NULL;
	image->type=type;
	image->hdr_refcount=1;

	int cn;
	cn = ((type&0xF8)>>3)+1;
	
	image->cn = cn;

	int type_size;
	type_size = type & 0x07;

	int depth;
	if(type_size == 0) 		depth = 1;
	else if(type_size == 1)	depth = 1;
	else if(type_size == 2)	depth = 2;
	else if(type_size == 3)	depth = 2;
	else if(type_size == 4)	depth = 4;
	else if(type_size == 5)	depth = 4;
	else					depth = 8;	

	image->depth = depth;

	int step;
	step = cn*depth*width;

	image->step = step;
	
	image->height = height;

	image->width = width;
	
	int size;
	size = height*width*cn;
	
	image->size = size;
	
	*(image->hidinfo + 1) = 0;
	*(image->hidinfo + 2) = width;
	*(image->hidinfo + 3) = height;
	*(image->hidinfo + 4) = type;
	*(image->hidinfo + 5) = width;
	*(image->hidinfo + 6) = height;
	*(image->hidinfo + 7) = 0;
	*(image->hidinfo + 8) = cn;
	*(image->hidinfo + 9) = depth;
	*(image->hidinfo +10) = size;
	
	if(*(image->hidinfo) != (int)image)
	{ 
		ImgMat *p;
		p = (ImgMat *)*(image->hidinfo);
		
		p->data.i = NULL;
		p->type = type;
		p->hdr_refcount=1;
		p->step = step;
		p->height = height;
		p->width = width;
		p->cn = cn;
		p->depth = depth;
		p->size =size;
	}		
}

ImgMat *imgNewMatHeader()
{
	ImgMat *image;
	image = (ImgMat *)malloc(sizeof(ImgMat));
	
	image->data.ptr = NULL;
	
	image->hidinfo = (int *)malloc(64);
	
	*(image->hidinfo) = (int)image;
	
	return image;
}

ImgMat *imgCreateMatHeader(int height,int width,int type)
{
	ImgMat *image;
	image = imgNewMatHeader();
	
	imgInitializeMatHeader(image,height,width,type);
	
	return image;
}

void imgMatRedefine(ImgMat *src,int height,int width,int type)
{	
	if((src->data.ptr!=NULL)&&(*(src->hidinfo + 1) == 1))
		free(src->data.ptr);
	
	imgInitializeMatHeader(src,height,width,type);
	imgCreateMatData(src);
}

void imgchangeMatHeader(ImgMat *src,int height,int width,int type)
{
	if((src->data.ptr != NULL)&&(*(src->hidinfo + 1) == 1))
		free(src->data.ptr);
	
	imgInitializeMatHeader(src,height,width,type);
}
	

ImgMat *imgCreateMat(int height,int width,char type)
{
	ImgMat *image;
	image=imgCreateMatHeader(height,width,type);	
	imgCreateMatData(image);

	return image;
}

void imgSpecifyMatData(ImgMat *src,void *data)
{
	if((src->data.ptr != NULL)&&(*(src->hidinfo + 1) == 1))
		free(src->data.i);
		
	*(src->hidinfo +7) =(int)data;///////////////////////////////////////////////////////////////////

	src->data.i=(int *)data;/////////////////////////////////////////////////////////////////////////
	
	if((int)src != *(src->hidinfo))
	{
		ImgMat *p;
		p = (ImgMat *)*(src->hidinfo); 
		
		p->data.i = (int *)data;
	}
}

void imgReleaseMatData(ImgMat *src)
{
	if((src->data.ptr != NULL)&&(*(src->hidinfo + 1) == 1))
		free(src->data.i);
	
	src->data.i = NULL;
}

void imgReleaseMat(ImgMat *src)
{
	if((src->data.ptr != NULL)&&(*(src->hidinfo + 1) == 1))
		free(src->data.i);

	void *p;
	p =(void *)*(src->hidinfo);
	
	free(src->hidinfo);
	
	free(p);

	return;
}

int imgMatCheck(ImgMat *src)
{
	int flag;
	
	flag = ((src !=NULL) && (src->hidinfo != NULL));
	
	flag = flag && (src->data.ptr != NULL); 
	
	flag = flag && (*(src->hidinfo + 2) == src->width);
	flag = flag && (*(src->hidinfo + 3) == src->height);
	flag = flag && (*(src->hidinfo + 4) == src->type);
	flag = flag && (*(src->hidinfo + 5) == src->width);
	flag = flag && (*(src->hidinfo + 6) == src->height);
	flag = flag && (*(src->hidinfo + 7) == (int)(src->data.ptr));
	flag = flag && (*(src->hidinfo + 8) == src->cn);
	flag = flag && (*(src->hidinfo + 9) == src->depth);
	flag = flag && (*(src->hidinfo +10) == src->size);
	
	if((int)src != *(src->hidinfo))
	{
		ImgMat *p;
		p = (ImgMat *)*(src->hidinfo);
		
		flag = flag && (*(p->hidinfo + 2) == src->width);
		flag = flag && (*(p->hidinfo + 3) == src->height);
		flag = flag && (*(p->hidinfo + 4) == src->type);
		flag = flag && (*(p->hidinfo + 5) == src->width);
		flag = flag && (*(p->hidinfo + 6) == src->height);
		flag = flag && (*(p->hidinfo + 7) == (int)(src->data.ptr));	
		flag = flag && (*(p->hidinfo + 8) == src->cn);
		flag = flag && (*(p->hidinfo +10) == src->size);
	}
	
	if(flag)
		return 0;
	else
		return 1;
}

void imgCrop(ImgMat *src,ImgMat *dst,ImgRect *rect);
void imgResize(ImgMat *src,ImgMat *dst,int height,int width);

void imgMateMat(ImgMat *src,int size_height,int size_width,int type)
{	
	if(((src->width)==(size_width))&&((src->height)==(size_height)))
		return;
	
	ImgMat *dst;
	dst = imgCreateMat(size_height,size_width,src->type);
	
	if(type<5)
	{
		int k1,k2,k;
		k1 = (float)(size_width)/(float)(src->width);
		k2 = (float)(size_height)/(float)(src->height);
		
		k = (k1>k2)?k1:k2;
		
		ImgSize resize;
		resize.width = (int)((float)(src->width)*k+0.5);
		resize.height = (int)((float)(src->height)*k+0.5);
			
		ImgMat *rsz;
		rsz = imgCreateMat(resize.height,resize.width,src->type);
			
		imgResize(src,rsz,0,0);
			
		if(k1 == k2)
		{
			free(src->data.ptr);
			free(src->hidinfo);
			*src = *rsz;
			free(rsz);
			
			return;
		}
		
		ImgRect rect;
		rect.width = size_width;
		rect.height = size_height;
		
		if(type == UP_LEFT_SCROP)
		{
			rect.x = 0;
			rect.y = 0;
		}
		else if(type == UP_RIGHT_SCROP)
		{
			rect.x = rsz->width - rect.width;
			rect.y = 0;
		}
		else if(type == DOWN_LEFT_SCROP)
		{
			rect.x = 0;
			rect.y = rsz->height - rect.height;
		}
		else if(type == DOWN_RIGHT_SCROP)
		{
			rect.x = rsz->width - rect.width;
			rect.y = rsz->height - rect.height;
		}
						
		imgCrop(rsz,dst,&rect);
		imgReleaseMat(rsz);				
	}
	else if(type == RESIZE)
		imgResize(src,dst,0,0);
	else if(type>5)
	{
		ImgRect rect;
		rect.width = size_width;
		rect.height = size_height;
		
		if(type == UP_LEFT_CROP)
		{
			rect.x = 0;
			rect.y = 0;
		}
		else if(type == UP_RIGHT_CROP)
		{
			rect.x = src->width - rect.width;
			rect.y = 0;
		}
		else if(type == DOWN_LEFT_CROP)
		{
			rect.x = 0;
			rect.y = src->height - rect.height;
		}
		else if(type == DOWN_RIGHT_CROP)
		{
			rect.x = src->width - rect.width;
			rect.y = src->height - rect.height;
		}		
			
		imgCrop(src,dst,&rect);
	}
	
	free(src->data.ptr);
	free(src->hidinfo);
	*src = *dst;
	free(dst);		
}



#define imgCopyMatHeader(src) imgCreateMatHeader(src->height,src->width,src->type)

void imgCopyMat(ImgMat *src,ImgMat *dst)
{
	imgMate(src,dst);

	int n;
	n = (src->step)*(src->height)/4;

	int *p_src;
	p_src = src->data.i;

	int *p_dst;
	p_dst = dst->data.i;

	int i;
	for(i=0;i<n;i++)
	{
		*p_dst=*p_src;
		p_src = p_src+1;
		p_dst = p_dst+1;
	}
}


	




void imgCleanMat(ImgMat *src)
{
	int img_size;
	img_size = src->width*src->height;
	
	unsigned char *p_src;
	p_src = src->data.ptr;
	
	int type;
	type = src->type&0x07;

	if((type==TYPE_16U)||(type == TYPE_16S))
		img_size = img_size*2;
	else if((type==TYPE_32S)||(type == TYPE_32F))
		img_size = img_size*4;
	else if(type == TYPE_64F)
		img_size = img_size*8;
			
	int i;
	for(i=0;i<img_size;i++)
	{
		*p_src = 0;
		p_src = p_src+1;
	}
}

int imgCompareMat(void *mat1,void *mat2)
{
	ImgMat *mat_1,*mat_2;
	mat_1 = (ImgMat *)mat1;
	mat_2 = (ImgMat *)mat2;
	
	int result;
	
	result = ((mat_1->type & 0x3F) == (mat_2->type & 0x3F));
	if(result==0)
	{
		printf("compare Mat header error:type of Mat 1 is 0x%x,but type of Mat 2 is 0x%x\n",(mat_1->type & 0x1F),(mat_2->type & 0x1F));
		return 1;
	}

	result = (mat_1->height == mat_2->height);
	if(result==0)
	{
		printf("compare Mat header error:height of Mat 1 is %d,but height of Mat 2 is %d\n",mat_1->height,mat_2->height);
		return 1;
	}

	result = (mat_1->width == mat_2->width);
	if(result==0)
	{
		printf("compare Mat header error:width of Mat 1 is %d,but width of Mat 2 is %d\n",mat_1->width,mat_2->width);
		return 1;
	}
	
	int mat_1_n;
	mat_1_n = (mat_1->height*mat_1->step);
	
	int mat_2_n;
	mat_2_n = (mat_2->height*mat_2->step);
	
//	result = (mat_1_n == mat_2_n);
	
	result = (mat_1->size == mat_2->size);
	if(result==0)
	{
		printf("compare Mat header error:data number of Mat 1 is %d,but datanum of Mat 2 is %d\n",mat_1_n,mat_2_n);
		return 1;
	}
	
	char *p1,*p2;
	p1 = (char *)mat_1->data.ptr;
	p2 = (char *)mat_2->data.ptr;
	
	int i;
	for(i=0;i<mat_1_n;i++)
	{
		result = (*p1 == *p2);
			
		if(result==0)
		{
			printf("compare ImgMat data error:located at %d data between ImgMat 1 and ImgMat 2 is different\n",i);
			return 1;
		}
			
		p1++;
		p2++;
	}
	
	return 0;
}
	

struct bmpheader
{
	int bmpsize;	
	int bmpreserved;	
	int bmpoffbits;	
	
	int imginfosize;	
	int imgwidth;		
	int imgheight;	
	short imgplanes;	
	short imgbitcount;	
	int imgcompression;	
	int imgsize;		
	int imgxpelspermeter;	
	int imgypelspermeter;
	int imgclrused;	
	int imgclrimportant;
};


struct colorlist
{
	unsigned char color_blue;
	unsigned char color_green;
	unsigned char color_red;
	char reserved;
};

int imgSaveGrayBMP(ImgMat *src,char *filename)
{
#ifdef DEBUG
	if(src->type != TYPE_8UC1)
	{
		printf("IMG Error:\n\tin imgSaveGrayBMP: Wrong src format\n");
		return 1;
	}
#endif

	FILE *f;
	f = fopen(filename,"wb");


	if(f == NULL)
	{
		printf("IMG Error:\n\tin imgSaveGrayBMP: save image error\n");
		return 1;
	}
		
	int img_width;
	img_width = *(src->hidinfo + 5);
	
	int img_height;
	img_height = *(src->hidinfo + 6);

	int data_width;
	data_width = ((img_width-1)&0xFFFFFFFC)+4;
	
	int img_size = img_height*data_width;
	
	int i;
 
	short bmptype = 0x4d42;
		
	fseek(f,0,SEEK_SET);
	fwrite(&bmptype,1,2,f);
// printf("%s\n",filename);
	struct bmpheader my_bmp;
	
	my_bmp.bmpsize = 1078 + img_size;
	my_bmp.bmpreserved = 0;
	my_bmp.bmpoffbits = 1078;
	my_bmp.imginfosize = 40;
	my_bmp.imgwidth = img_width;
	my_bmp.imgheight = img_height;
	my_bmp.imgplanes = 1;
	my_bmp.imgbitcount = 8;
	my_bmp.imgcompression =0;
	my_bmp.imgsize = img_size;
	my_bmp.imgxpelspermeter = 11811;
	my_bmp.imgypelspermeter = 11811;
	my_bmp.imgclrused = 0;
	my_bmp.imgclrimportant = 0;
	
	
	
	// fseek(f,2,SEEK_SET);
	fwrite(&my_bmp,1,52,f);
	
	struct colorlist color_gray[256];
	for(i=0;i<256;i++)
	{
		color_gray[i].color_blue = (unsigned char)i;
		color_gray[i].color_green = (unsigned char)i;
		color_gray[i].color_red = (unsigned char)i;
		color_gray[i].reserved = 0;
	}
	
	// fseek(f,54,SEEK_SET);
	fwrite(color_gray,1,1024,f);

	unsigned char *data;
	
	int data0 = 0;
	
	// fseek(f,1078,SEEK_SET);	
	
	if(data_width==img_width)
	{
		for(i=img_height-1;i>=0;i--)
		{
			data = src->data.ptr + i*(src->step);			
			fwrite(data,1,img_width,f);				
		}
	}
	else
	{
		for(i=img_height-1;i>=0;i--)
		{
			data = src->data.ptr + i*(src->step);
			fwrite(data,1,img_width,f);				
			fwrite(&data0,1,data_width-img_width,f);
		}
	}	
	
	fclose(f);
	
	return 0;
}
	
int imgSaveBMP(ImgMat *src,char *filename)
{
	
	if((src->type == TYPE_8UC1)||(src->type == TYPE_8SC1))
	{
		imgSaveGrayBMP(src,filename);
		return 0;
	}
	
#ifdef DEBUG
	if(src->type != TYPE_8UC3)
	{
		printf("QY Error:\n\tin imgSaveBMP: Wrong src format\n");
		return 1;
	}
#endif

	FILE *f;
	f = fopen(filename,"wb");

#ifdef DEBUG	
	if(f == NULL)
	{
		printf("QY Error:\n\tin imgSaveBMP: save image error\n");
		return 1;
	}
#endif

	int img_width;
	img_width = *(src->hidinfo + 5);
	
	int img_height;
	img_height = *(src->hidinfo + 6);
	
	int data_width;
	data_width = ((img_width*3-1)&0xFFFFFFFC)+4;

	int img_size = data_width * img_height;

	int i;
	 
	short bmptype = 0x4d42;
		
	fseek(f,0,SEEK_SET);
	fwrite(&bmptype,1,2,f);

	struct bmpheader my_bmp;
	
	my_bmp.bmpsize = 54 + img_size;
	my_bmp.bmpreserved = 0;
	my_bmp.bmpoffbits = 54;
	my_bmp.imginfosize = 40;
	my_bmp.imgwidth = img_width;
	my_bmp.imgheight = img_height;
	my_bmp.imgplanes = 1;
	my_bmp.imgbitcount = 24;
	my_bmp.imgcompression =0;
	my_bmp.imgsize = img_size;
	my_bmp.imgxpelspermeter = 11811;
	my_bmp.imgypelspermeter = 11811;
	my_bmp.imgclrused = 0;
	my_bmp.imgclrimportant = 0;
	
	// fseek(f,2,SEEK_SET);
	fwrite(&my_bmp,1,52,f);

	unsigned char *data;

	int pos = 54;
	
	int data0[3] = {0,0,0};
	
	// fseek(f,54,SEEK_SET);
	
	if(data_width==img_width)
	{		
		for(i=img_height-1;i>=0;i--)
		{			
			data = src->data.ptr + i*(src->step);			
			fwrite(data,1,img_width*3,f);			
		}
	}
	else
	{		
		for(i=img_height-1;i>=0;i--)
		{			
			data = src->data.ptr + i*(src->step);			
			fwrite(data,1,img_width*3,f);			
			fwrite(data0,1,data_width-img_width*3,f);				
		}
	}
	
	fclose(f);
	
	return 0;
}


int imgReadBMP(const char *filename,ImgMat *dst)
{
	FILE *f;
	f = fopen(filename,"rb");
	if(f == NULL)
	{
		printf("read image error\n");
		return 1;
	}
 
	short filetype;
		
	fseek(f,0,SEEK_SET);
	fread(&filetype,1,2,f);
	
	if(filetype != 0x4d42)
	{
		printf("read image error\n");
		return 1;
	}
	
	struct bmpheader my_bmp;
	
	fseek(f,2,SEEK_SET);
	fread(&my_bmp,1,52,f);
	
	int img_width;
	img_width = my_bmp.imgwidth;
	
	int img_height;
	img_height = my_bmp.imgheight;
	
	int cn;
	cn = my_bmp.imgbitcount>>3;
	
	int data_width;
	data_width = ((img_width*cn-1)&0xFFFFFFFC)+4;
	
	int type;
	type = ((cn-1)<<3);
	// type = dst->type;
	
	imgInitializeMatHeader(dst,img_height,img_width,type);
	
	if((dst->data.ptr != NULL))
		free(dst->data.ptr);
	
	imgCreateMatData(dst);
		
	int mat_width;
	mat_width = dst->width;
	
	int i;
	
	int pos;
	pos = my_bmp.bmpoffbits;
	
	unsigned char *data;
	
	int data0=0;
	
	for(i=img_height-1;i>=0;i--)
	{
		data = dst->data.ptr + i*img_width*cn;
		
		fseek(f,pos,SEEK_SET);
		fread(data,1,img_width*cn,f);
				
		pos = pos+data_width;
	}
 
	fclose(f);
	
	return 0;

} 

int imgGetMatData(ImgMat *src,int x,int y)
{
	int data;
	
	int img_width;
	img_width = src->width;
	
	int img_height;
	img_height = src->height;
	
	if(x<0)
		x=0;
	else if(x>(img_width-1))
		x=img_width-1;
		
	if(y<0)
		y=0;
	else if(y>(img_height-1))
		y=img_height-1;
		
	int type;
	type = src->type&0x07;
	 
	if(type == TYPE_8U)
	{
	 unsigned char *p_src;
	 p_src = src->data.ptr;
	 data = (int)*(p_src + y*img_width +x);
	}
	
	else if(type == TYPE_8S)
	{
	 char *p_src;
	 p_src = (char *)src->data.ptr;
	 data = (int)*(p_src + y*img_width +x);
	}
	
	//else if(type == TYPE_16S)
	//{
	//	short *p_src;
	//	p_src = src->data.s;
	//	data = (int)*(p_src + y*img_width +x);
	//}		 
	
	return data;
}

void imgGetROIData(ImgMat *src,void *p_data,int x1,int x2,int y1,int y2)
{
	int roi_width;
	roi_width = x2-x1+1;
	
	int roi_height;
	roi_height = y2-y1+1;

	int *p;
	p = (int *)p_data;

	int i,j;
	for(j=0;j<roi_height;j++)
		for(i=0;i<roi_width;i++)
		{
			*p = imgGetMatData(src,x1+i,y1+j);
			p = p+1;
		}
}



int imgReadBinaryStream(const char *filename,ImgMat *dst,int n)
{
	FILE *f;
	f = fopen(filename,"rb");

#ifdef DEBUG
	if(f==NULL)
	{
		printf("QY Error:\n\tin imgReadBinaryStream: file open error!\n");
		return 1;
	}
#endif
	int img_width;
	img_width = *(dst->hidinfo + 5);
	
	int img_height;
	img_height = *(dst->hidinfo + 6);
	
	int cn;
	cn = ((dst->type&0x38)>>3)+1;
	
	int img_step;
	img_step = img_width *cn;

	int img_size;
	img_size = img_step*img_height;

#ifdef DEBUG
	int frame_num = 0;
	
	fseek(f, 0, SEEK_END);
	
	frame_num = (int)((int)ftell(f)/img_size);

	if(n>frame_num)
	{
		printf("QY Error:\n\tin imgReadBinaryStream: frame number overflow!\n");
		return 1;
	}
#endif

	int mat_width;
	int mat_height;
	
	mat_width = dst->width;
	mat_height = dst->height;

	int offset;
	offset = (n-1)*img_size;
	
	if((img_width = mat_width)&&(img_height == mat_height))
	{ 
		fseek(f,offset,SEEK_SET);
		fread(dst->data.ptr,1,img_size,f);	
	}
	else
	{
		unsigned char *p;
		p = dst->data.ptr;
		
		int i,j;
		
		for(i=0;i<img_height;i++)
		{
			fseek(f,offset,SEEK_SET);
			fread(p,1,img_width,f);
			
			offset = offset + img_step;
			
			p = p+img_step;
			for(j=img_step;j<dst->step;j++)
			{
				*p = *(p-cn);
				p++;
			}
		}
	}
	
	fclose(f);
	
	return 0; 
}


int imgWriteBinaryStream(ImgMat *src,const char *filename)
{
	FILE *f;
	f = fopen(filename,"rb");

#ifdef DEBUG
	if(f==NULL)
	{
		printf("QY Error:\n\tin imgReadBinaryStream: file open error!\n");
		return 1;
	}
#endif

	fseek(f, 0, SEEK_END);
	
	int pos;
	pos = (int)ftell(f);
	
	fseek(f,pos,SEEK_SET);

	int mat_width;
	int mat_height;
	
	mat_width = src->width;
	mat_height = src->height;
	
	int img_width;
	img_width = *(src->hidinfo + 5);
	
	int img_height;
	img_height = *(src->hidinfo + 6);
	
	int i;
	
	if((img_width = mat_width)&&(img_height == mat_height))
	{
		int img_size;
		img_size = img_width *img_height;
		
		fwrite(src->data.ptr,1,img_size,f);
	}
	
	else 
	{ 
		unsigned char *p;
		p = src->data.ptr;
		
		for(i=0;i<img_height;i++)
		{
			fwrite(p,1,img_width,f);
			
			p = p+src->step;
			pos = pos+img_width;
			fseek(f,pos,SEEK_SET);
			
		}
	}
 
	fclose(f);
	return 0;	 
	
}





