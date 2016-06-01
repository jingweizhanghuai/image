#include <stdio.h>
#include "type.h"
#include "img.h"



int main()
{
	ImgMat *src;
	src = imgNewMatHeader();
	
	imgReadJPG("./test_image/4Z0.jpg",src);

	imgColorToGray(src,);
	imgSaveBMP(src,"./test_image/test00.bmp");
	
	ImgMat *dst;
	dst = imgCreateMat(src->height,src->width,TYPE_8UC1);
	
	imgMeanFilter(src,dst,7);
	imgSaveBMP(dst,"./test_image/test01.bmp");
	
	imgAdaptContrast(src,dst,85,170,7);
	
	imgSaveBMP(dst,"./test_image/test02.bmp");
	
	// float mean[192];
	// float variance[192];
	// int max[192];
	// int min[192];
	
	// imgBlockMean(src,16,12,mean,min,max);
	
	// int i,j,n;
	// n=0;
	// for(j=0;j<12;j++)
	// {
		// for(i=0;i<16;i++)
		// {
			// printf("%d\t",(int)mean[n]);
			// n++;
		// }
		// printf("\n");
	// }
	// printf("\n");
	// printf("\n");
	// n=0;
	// for(j=0;j<12;j++)
	// {
		// for(i=0;i<16;i++)
		// {
			// printf("%d\t",min[n]);
			// n++;
		// }
		// printf("\n");
	// }
		
}/*	
	
	ImgPoint pd[3];
	pd[0].x = 100;
	pd[0].y = 200;
	pd[1].x = 500;
	pd[1].y = 50;
	pd[2].x = 50;
	pd[2].y = 730;
	pd[3].x = 530;
	pd[3].y = 740;
	
	// imgPerspective(src,,,pd);
	
	// imgColorToGray(src,);
	
	// imgSaveJPG(src,"./test_image/tests2.jpg");

	ImgThreshold threshold;
	threshold.thresh_num = 1;
	threshold.thresh[0] = 100;
	// threshold.thresh[1] = 150;
	threshold.value[0] = 255;
	threshold.value[1] = 150;//NOT_CHANGE_WITH_THRESHOLD;
	// threshold.value[2] = 255;

	// imgThreshold(src,,1,&threshold);

	
	
	ImgPoint ps[3];
	ps[0].x = 0;
	ps[0].y = 0;
	ps[1].x = 600;
	ps[1].y = 0;
	ps[2].x = 0;
	ps[2].y = 600;
	// pd[3].x = 550;
	// pd[3].y = 500;
	
	// ImgRect rect;
	// rect.x = -100;
	// rect.y = -150;
	// rect.width = 900;
	// rect.height = 800;
	
	// ImgLocate locate;
	// locate.x = 30;
	// locate.y = 30;
	
	// ImgColor color;
	// color.r = 150;
	// color.g = 50;
	// color.b = 50;

	// imgFill(src,,&locate,&color,8);
	
	ImgMat *dst;
	dst = imgCreateMat(src->height,src->width,src->type);
	
	ImgPoint center;
	center.x = 236;
	center.y = 210;
	
	float m0 = 2.0;
	imgLens(src,dst,&center,m0,50,100);
	
	// int i;
	
	// imgCrop(src,,&rect);

	// TIME_START;
	// for(i=0;i<5000;i++)
	// {
		// imgMeanFilter(src,dst,5);
	// }

	// TIME_START;
	// for(i=0;i<500;i++)
	// {
		// imgAffine(src,ps,dst,pd);
	// }
		
	imgSaveBMP(dst,"./test_image/test20.bmp");
	imgReleaseMat(dst);
	imgReleaseMat(src);
	
	imgOCLFinish();
	// TIME_STOP;
}*/
