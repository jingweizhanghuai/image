#include <stdio.h>
#include "type.h"
#include "img.h"

// ImgMat *imgNewMatHeader();
// ImgMat *imgCreateMat(int height,int width,char type);
// void imgReleaseMatData(ImgMat *src);
// int imgReadBMP(const char *filename,ImgMat *dst);
// int imgSaveBMP(ImgMat *src,char *filename);
// void imgColorToGray(ImgMat *src,ImgMat *dst);
// void imgFilter(ImgMat *src,ImgMat *dst,int r);
// void imgBlur(ImgMat *src,ImgMat *dst,int r);
// void imgMist(ImgMat *src,ImgMat *dst,int r);
	
// int aa = 0xFFFFFFFF;
// #define DEFMAT(mat) (ImgMat *)((int)mat&aa)	
// #define DEFARG(mat) ((#mat[0])?DEFMAT(mat):NULL)
// #define imgColorToGray(src,dst) ColorToGray(src,DEFARG(dst));

int main()
{
	ImgMat *src;
	src = imgNewMatHeader();
	
	imgReadBMP("./test2.bmp",src);
	
	ImgMat *gray;
	gray = imgCreateMat(src->height,src->width,TYPE_8UC1);
	
	imgColorToGray(src,gray);	
	imgSaveBMP(gray,"./test2_gray.bmp");
	
	imgColorToGray(src,);
	imgSaveBMP(src,"./test2_gray2.bmp");
	
	ImgMat *dst;
	dst = imgCreateMat(src->height,src->width,TYPE_8UC1);
	
	// imgBlur5(gray,dst);
	// imgSaveBMP(dst,"./test2_blur5.bmp");
	
	imgFilter(gray,,,);	
	imgSaveBMP(gray,"./test2_filter.bmp");
	
	// ImgMat *dst2;
	// dst2 = imgCreateMat(src->height,src->width,TYPE_8UC1);
	
	
	// imgMist(dst,dst2,7);
	// imgSaveBMP(dst2,"./test2_mist.bmp");
	
	// ImgMat *dst3;
	// dst3 = imgCreateMat(src->height,src->width,TYPE_8UC3);
	
	// imgFilter(src,dst3,4);
	
	// imgSaveBMP(dst3,"./test2_filter2.bmp");
	
	imgReleaseMatData(src);
	// imgReleaseMatData(gray);
	// imgReleaseMatData(dst);
	// imgReleaseMatData(dst2);
}


// int main()
// {
	// ImgMat *src;
	// src = imgNewMatHeader();
	
	// imgReadBMP("./bar.bmp",src);
	
	// int img_width;
	// img_width = src->width;
	
	// int img_height;
	// img_height = src->height;
	
	// printf("width is %d,height is %d\n",img_width,img_height);
	
	// unsigned char *p_src;
	// p_src = src->data.ptr;
	
	// #define SRC_0(x,y) p_src[(y)*img_width*3+(x)*3]
	// #define SRC_1(x,y) p_src[(y)*img_width*3+(x)*3+1]
	// #define SRC_2(x,y) p_src[(y)*img_width*3+(x)*3+2]
	
	// int j;
	// for(j=0;j<img_height;j++)
		// printf("%d:  \t%d  \t%d  \t%d  \n",j,SRC_0(20,j),SRC_1(20,j),SRC_2(20,j));
// }
	
	
	
