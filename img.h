const int aa = 0xFFFFFFFF;

#define MAT_ADDRESS(mat) (ImgMat *)((int)mat&aa)
#define INT_VALUE(x) ((int)x&aa)
#define INT_ADDRESS(p) (int *)((int)p&aa)
#define FLOAT_ADDRESS(p) (float *)((int)p&aa)
#define POINT_ADDRESS(p) (ImgPoint *)((int)p&aa)
	
#define DEF_MAT(mat) ((#mat[0])?MAT_ADDRESS(mat):NULL)
#define DEF_INTP(p) ((#p[0])?INT_ADDRESS(p):NULL)
#define DEF_FLOATP(p) ((#p[0])?FLOAT_ADDRESS(p):NULL)
#define DEF_POINT(p) ((#p[0])?POINT_ADDRESS(p):NULL)

ImgMat *imgNewMatHeader();
ImgMat *imgCreateMat(int height,int width,char type);
void imgReleaseMatData(ImgMat *src);
int imgReadBMP(const char *filename,ImgMat *dst);
int imgSaveBMP(ImgMat *src,char *filename);

void imgColorToGray(ImgMat *src,ImgMat *dst);
#define imgColorToGray(src,dst) ColorToGray(src,DEF_MAT(dst))

void imgFilter(ImgMat *src,ImgMat *dst,int r,int thresh);
#define DEF_FILTER_R(r) ((#r[0])?INT_VALUE(r):5)
#define DEF_FILTER_THRESH(thresh) ((#thresh[0])?INT_VALUE(thresh):15)
#define imgFilter(src,dst,r,thresh) Filter(src,DEF_MAT(dst),DEF_FILTER_R(r),DEF_FILTER_THRESH(thresh))

void imgMeanFilter(ImgMat *src,ImgMat *dst,int r);
#define DEF_MEANFILTER_R(r) ((#r[0])?INT_VALUE(r):1)
#define imgMeanFilter(src,dst,r) MeanFilter(src,DEF_MAT(dst),DEF_MEANFILTER_R(r))

void imgResize(ImgMat *src,ImgMat *dst,int height,int width);
#define DEF_RESIZE_SIZE(x) ((#x[0])?INT_VALUE(x):0)
#define imgResize(src,dst,height,width) Resize(src,DEF_MAT(dst),DEF_RESIZE_SIZE(height),DEF_RESIZE_SIZE(width))

void imgDebase(ImgMat *src,ImgMat *dst,int height,int width,int r);
#define DEF_DEBASE_SIZE(x) ((#x[0])?INT_VALUE(x):0)
#define DEF_DEBASE_R(x) ((#x[0])?INT_VALUE(x):3)
#define imgDebase(src,dst,height,width,r) Debase(src,DEF_MAT(dst),DEF_DEBASE_SIZE(height),DEF_DEBASE_SIZE(width),DEF_DEBASE_R(r))

void imgReadColorLUT(const char *file_name,ImgColorLUT *lut,int cn);
void imgSaveColorLUT(ImgColorLUT *lut,const char *file_name);
void imgReadColorBar(const char *file_name,ImgColorLUT *lut);
void imgSaveColorBar(ImgColorLUT *lut,const char *file_name);
void imgColorLUT(ImgMat *src,ImgMat *dst,ImgColorLUT *lut);
#define imgColorLUT(src,dst,lut) ColorLUT(src,DEF_MAT(dst),lut)

void imgGrayToPesu(ImgMat *src,ImgMat *dst);
#define imgGrayToPesu(src,dst) GrayToPesu(src,DEF_MAT(dst))
void imgGrayToRainbow(ImgMat *src,ImgMat *dst);
#define imgGrayToRainbow(src,dst) GrayToRainbow(src,DEF_MAT(dst))

void imgColorInvert(ImgMat *src,ImgMat *dst);
#define imgColorInvert(src,dst) ColorInvert(src,DEF_MAT(dst))

void imgCrop(ImgMat *src,ImgMat *dst,ImgRect *rect);
#define imgCrop(src,dst,rect) Crop(src,DEF_MAT(dst),rect)

void imgAdd(ImgMat *src1,ImgMat *src2,ImgMat *dst);
#define imgAdd(src1,src2,dst) Add(src1,src2,DEF_MAT(dst))

void imgSub(ImgMat *src1,ImgMat *src2,ImgMat *dst);
#define imgSub(src1,src2,dst) Sub(src1,src2,DEF_MAT(dst))

void imgAbsDiff(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *average_diff);
#define imgAbsDiff(src1,src2,dst,average_diff) AbsDiff(src1,src2,DEF_MAT(dst),DEF_FLOATP(average_diff))

void imgIncrease(ImgMat *src,ImgMat *dst,int *value);
#define imgIncrease(src,dst,value) Increase(src,DEF_MAT(dst),DEF_INTP(value))

void imgAffine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd);
#define imgAffine(src,ps,dst,pd) Affine(src,DEF_POINT(ps),DEF_MAT(dst),DEF_POINT(pd))

void imgPerspective(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd);
#define imgPerspective(src,ps,dst,pd) Perspective(src,DEF_POINT(ps),DEF_MAT(dst),DEF_POINT(pd))

void imgOverlap(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *strength);
#define imgOverlap(src1,src2,dst,strength) Overlap(src1,src2,DEF_MAT(dst),DEF_FLOATP(strength))

void imgMirror(ImgMat *src,ImgMat *dst,int type);
#define DEF_MIRROR_TYPE(x) ((#x[0])?INT_VALUE(x):1)
#define imgMirror(src,dst,type)	Mirror(src,DEF_MAT(dst),DEF_MIRROR_TYPE(type))

void imgRotate(ImgMat *src,ImgMat *dst,int angle);
#define DEF_ROTATE_ANGLE(x) ((#x[0])?INT_VALUE(x):90)
#define imgRotate(src,dst,angle) Rotate(src,DEF_MAT(dst),DEF_ROTATE_ANGLE(angle))

void imgThreshold(ImgMat *src,ImgMat *dst,int cn,ImgThreshold *threshold);
#define DEF_THRESHOLD_CN(x) ((#x[0])?INT_VALUE(x):1)
#define imgThreshold(src,dst,cn,threshold) Threshold(src,DEF_MAT(dst),DEF_THRESHOLD_CN(cn),threshold)

void imgBlur(ImgMat *src,ImgMat *dst,int r);
void imgMist(ImgMat *src,ImgMat *dst,int r);

int imgLinearEquation(float *data,float *answer,int num);
float imgSqrt(float x);
float imgSin(int x);
float imgCos(int x);
