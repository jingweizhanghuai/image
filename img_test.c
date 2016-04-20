#include <stdio.h>
#include "type.h"
#include "img.h"

int main()
{
	ImgMat *src;
	src = imgNewMatHeader();
	
	imgReadBMP("./test_image/test.bmp",src);

	// imgColorToGray(src,);
	
	ImgThreshold threshold;
	threshold.thresh_num = 1;
	threshold.thresh[0] = 100;
	// threshold.thresh[1] = 150;
	threshold.value[0] = 255;
	threshold.value[1] = 150;//NOT_CHANGE_WITH_THRESHOLD;
	// threshold.value[2] = 255;
	
	// imgThreshold(src,,1,&threshold);
	
	// ImgPoint pd[3];
	// pd[0].x = 100;
	// pd[0].y = 200;
	// pd[1].x = 500;
	// pd[1].y = 50;
	// pd[2].x = 50;
	// pd[2].y = 550;
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
	
	int i;
	
	// imgCrop(src,,&rect);

	// TIME_START;
	// for(i=0;i<5000;i++)
	// {
		// imgMeanFilter(src,dst,5);
	// }

	TIME_START;
	for(i=0;i<5000;i++)
	{
		imgMeanFilter_cl(src,dst,5);
	}
		
	imgSaveBMP(dst,"./test_image/test5.bmp");
	imgReleaseMat(dst);
	imgReleaseMat(src);
	
	imgOCLFinish();
	TIME_STOP;
}
