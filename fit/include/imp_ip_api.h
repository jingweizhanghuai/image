// #ifdef WITH_GLOBAL_SET
// #include "imp_global_set.h"
// #endif

#include <stdio.h>
#include "imp_algo_type.h"
#include "imp_ip_preprocess.h"

int s32GlobalWidth = 0;
int s32GlobalHeight = 0;
int* ps32GlobalIndex = NULL;

#define IMP_IP_SetGlobalSize(h,w) {\
	s32GlobalHeight = h;\
	s32GlobalWidth =w;\
}

RGB_IMAGE_S *IMP_IP_CreateImageRGB(IMP_S32 s32Height,IMP_S32 s32Width);
RGB_IMAGE_S *IMP_IP_NewImageRGB();
void IMP_IP_SetImageRGB(RGB_IMAGE_S *img,IMP_S32 s32Height,IMP_S32 s32Width,IMP_U8 *pData);
RGB_IMAGE_S *IMP_IP_GenerateImageRGB(IMP_S32 s32Height,IMP_S32 s32Width,IMP_U8 *pData);
void IMP_IP_ReleaseImageRGB(RGB_IMAGE_S *img);

GRAY_IMAGE_S *IMP_IP_CreateImageGray(IMP_S32 s32Height,IMP_S32 s32Width);
GRAY_IMAGE_S *IMP_IP_NewImageGray();
void IMP_IP_SetImageGray(GRAY_IMAGE_S *img,IMP_S32 s32Height,IMP_S32 s32Width,IMP_U8 *pData);
GRAY_IMAGE_S *IMP_IP_GenerateImageGRAY(IMP_S32 s32Height,IMP_S32 s32Width,IMP_U8 *pData);
void IMP_IP_ReleaseImageGray(GRAY_IMAGE_S *img);

GRAY_IMAGE16_S *IMP_IP_CreateImage16Gray(IMP_S32 s32Height,IMP_S32 s32Width);
GRAY_IMAGE16_S *IMP_IP_NewImage16Gray();
void IMP_IP_SetImage16Gray(GRAY_IMAGE16_S *img,IMP_S32 s32Height,IMP_S32 s32Width,IMP_S16 *pData);
GRAY_IMAGE16_S *IMP_IP_GenerateImage16GRAY(IMP_S32 s32Height,IMP_S32 s32Width,IMP_S16 *pData);
void IMP_IP_ReleaseImage16Gray(GRAY_IMAGE16_S *img);

GRAY_IMAGE32_S *IMP_IP_CreateImage32Gray(IMP_S32 s32Height,IMP_S32 s32Width);
GRAY_IMAGE32_S *IMP_IP_NewImage32Gray();
void IMP_IP_SetImage32Gray(GRAY_IMAGE32_S *img,IMP_S32 s32Height,IMP_S32 s32Width,IMP_S32 *pData);
GRAY_IMAGE32_S *IMP_IP_GenerateImage32GRAY(IMP_S32 s32Height,IMP_S32 s32Width,IMP_S32 *pData);
void IMP_IP_ReleaseImage32Gray(GRAY_IMAGE32_S *img);

void IMP_IP_DrawRect(RGB_IMAGE_S *imgSrc,IMP_RECT_S *pRect,PIXEL_S color,IMP_S32 s32Widtgh);

void IMP_IP_SaveBMPGray(GRAY_IMAGE_S *src,const char *filename);
void IMP_IP_SaveBMPRGB(RGB_IMAGE_S *src,const char *filename);
void IMP_IP_ReadBMPRGB(const char *filename,RGB_IMAGE_S *dst);
void IMP_IP_ReadBMPGray(const IMP_CHAR  *filename,GRAY_IMAGE_S *dst);

void IMP_IP_RGBToGray(RGB_IMAGE_S *src,GRAY_IMAGE_S *dst);
void IMP_IP_RGBChannelSelect(RGB_IMAGE_S *imgu8Src,GRAY_IMAGE_S *imgu8Dst,IMP_S32 s32Cn);
void IMP_IP_Canny(GRAY_IMAGE_S *src,GRAY_IMAGE_S *dst,int thresh);

void IMP_IP_Contour(GRAY_IMAGE_S *src,GRAY_IMAGE_S *dst);


void IMP_IP_Thresh(GRAY_IMAGE_S *src,GRAY_IMAGE_S *dst,IMP_ThreshSet_S *thresh,int parm_num);



void IMP_IP_DrawRects(RGB_IMAGE_S *imgSrc,IMP_RectGroup_S *pRects,PIXEL_S color,IMP_S32 s32Widtgh);

void IMP_IP_FindEdge(GRAY_IMAGE_S *src,IMP_PointsGroup_S *points,IMP_RectGroup_S *rects);
void IMP_IP_GetConnectROI(GRAY_IMAGE_S *src,IMP_RectGroup_S *rects);



void IMP_IP_Integral(GRAY_IMAGE_S *imgSrc,GRAY_IMAGE32_S *imgDst);
void IMP_IP_Blur(GRAY_IMAGE_S *imgSrc,GRAY_IMAGE_S *imgDst,IMP_S32 r);
	
#define IMP_METHOD_LINEAR 0
#define IMP_METHOD_NN 1
void IMP_IP_Resize(GRAY_IMAGE_S *imgSrc,GRAY_IMAGE_S *imgDst,IMP_S32 method);



#define USE_RANDOM_NUM -1
IMP_HaarSet_S *IMP_IP_SetHaar(int s32Width,int s32Height,int haar_num,int s32RectNum);
void IMP_IP_HaarFromIntegral(GRAY_IMAGE32_S *imgItg,IMP_HaarSet_S *pHaar,IMP_BOOL *pboolHaarValid,IMP_FLOAT *f32Result,int num);

void IMP_IP_LinearMap(GRAY_IMAGE_S *imgSrc,GRAY_IMAGE_S *imgDst,IMP_FLOAT f32k,IMP_FLOAT f32b);
IMP_FLOAT IMP_IP_Mean(GRAY_IMAGE_S *imgSrc);
void IMP_IP_BrightControl(GRAY_IMAGE_S *imgSrc,GRAY_IMAGE_S *imgDst,IMP_S32 s32Bright);
#define IMP_IP_NormBrightControl(imgSrc,imgDst,s32NormBright) IMP_IP_BrightControl(imgSrc,imgDst,(s32NormBright<<8)/100)
void IMP_IP_ContrastControl(GRAY_IMAGE_S *imgSrc,GRAY_IMAGE_S *imgDst,IMP_FLOAT f32k);
