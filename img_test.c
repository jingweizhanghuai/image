#include <stdio.h>
#include "type.h"
#include "img.h"

int main()
{
	ImgMat *src;
	src = imgNewMatHeader();
	
	imgReadBMP("./test_image/test.bmp",src);
	
	// imgColorToGray(src,);
	
	// ImgThreshold threshold;
	// threshold.thresh_num = 2;
	// threshold.thresh[0] = 100;
	// threshold.thresh[1] = 150;
	// threshold.value[0] = 0;
	// threshold.value[1] = NOT_CHANGE_WITH_THRESHOLD;
	// threshold.value[2] = 255;
	
	// imgThreshold(src,,3,&threshold);
	
	ImgPoint pd[3];
	pd[0].x = 100;
	pd[0].y = 200;
	pd[1].x = 500;
	pd[1].y = 50;
	pd[2].x = 50;
	pd[2].y = 550;
	
	imgRotate(src,,60);
		
	imgSaveBMP(src,"./test_image/test2.bmp");
}
