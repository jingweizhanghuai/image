const int aa = 0xFFFFFFFF;

#define MAT_ADDRESS(mat) (ImgMat *)((int)mat&aa)
#define INT_VALUE(x) ((int)x&aa)
#define INT_ADDRESS(p) (int *)((int)p&aa)
#define FLOAT_ADDRESS(p) (float *)((int)p&aa)
#define POINT_ADDRESS(p) (ImgPoint *)((int)p&aa)
#define LOCATE_ADDRESS(p) (ImgLocate *)((int)p&aa)
#define POINTER_ADDRESS(p) (void *)((int)p&aa)
	
#define DEF_MATP(mat) ((#mat[0])?MAT_ADDRESS(mat):NULL)
#define DEF_INTP(p) ((#p[0])?INT_ADDRESS(p):NULL)
#define DEF_FLOATP(p) ((#p[0])?FLOAT_ADDRESS(p):NULL)
#define DEF_POINTP(p) ((#p[0])?POINT_ADDRESS(p):NULL)
#define DEF_LOCATEP(p) ((#p[0])?LOCATE_ADDRESS(p):NULL)
#define DEF_POINTER(p) ((#p[0])?POINTER_ADDRESS(p):NULL)

ImgMat *imgNewMatHeader();
ImgMat *imgCreateMat(int height,int width,char type);
void imgReleaseMatData(ImgMat *src);
int imgReadBMP(const char *filename,ImgMat *dst);
int imgSaveBMP(ImgMat *src,char *filename);

void imgColorToGray(ImgMat *src,ImgMat *dst);
#define imgColorToGray(src,dst) ColorToGray(src,DEF_MATP(dst))

void imgFilter(ImgMat *src,ImgMat *dst,int r,int thresh);
#define DEF_FILTER_R(r) ((#r[0])?INT_VALUE(r):5)
#define DEF_FILTER_THRESH(thresh) ((#thresh[0])?INT_VALUE(thresh):15)
#define imgFilter(src,dst,r,thresh) Filter(src,DEF_MATP(dst),DEF_FILTER_R(r),DEF_FILTER_THRESH(thresh))

void imgMeanFilter(ImgMat *src,ImgMat *dst,int r);
#define DEF_MEANFILTER_R(r) ((#r[0])?INT_VALUE(r):1)
#define imgMeanFilter(src,dst,r) MeanFilter(src,DEF_MATP(dst),DEF_MEANFILTER_R(r))

void imgResize(ImgMat *src,ImgMat *dst,int height,int width);
#define DEF_RESIZE_SIZE(x) ((#x[0])?INT_VALUE(x):0)
#define imgResize(src,dst,height,width) Resize(src,DEF_MATP(dst),DEF_RESIZE_SIZE(height),DEF_RESIZE_SIZE(width))

void imgDebase(ImgMat *src,ImgMat *dst,int height,int width,int r);
#define DEF_DEBASE_SIZE(x) ((#x[0])?INT_VALUE(x):0)
#define DEF_DEBASE_R(x) ((#x[0])?INT_VALUE(x):3)
#define imgDebase(src,dst,height,width,r) Debase(src,DEF_MATP(dst),DEF_DEBASE_SIZE(height),DEF_DEBASE_SIZE(width),DEF_DEBASE_R(r))

void imgReadColorLUT(const char *file_name,ImgColorLUT *lut,int cn);
void imgSaveColorLUT(ImgColorLUT *lut,const char *file_name);
void imgReadColorBar(const char *file_name,ImgColorLUT *lut);
void imgSaveColorBar(ImgColorLUT *lut,const char *file_name);
void imgColorLUT(ImgMat *src,ImgMat *dst,ImgColorLUT *lut);
#define imgColorLUT(src,dst,lut) ColorLUT(src,DEF_MATP(dst),lut)

void imgGrayToPesu(ImgMat *src,ImgMat *dst);
#define imgGrayToPesu(src,dst) GrayToPesu(src,DEF_MATP(dst))
void imgGrayToRainbow(ImgMat *src,ImgMat *dst);
#define imgGrayToRainbow(src,dst) GrayToRainbow(src,DEF_MATP(dst))

void imgColorInvert(ImgMat *src,ImgMat *dst);
#define imgColorInvert(src,dst) ColorInvert(src,DEF_MATP(dst))

void imgCrop(ImgMat *src,ImgMat *dst,ImgRect *rect);
#define imgCrop(src,dst,rect) Crop(src,DEF_MATP(dst),rect)

void imgAdd(ImgMat *src1,ImgMat *src2,ImgMat *dst);
#define imgAdd(src1,src2,dst) Add(src1,src2,DEF_MATP(dst))

void imgSub(ImgMat *src1,ImgMat *src2,ImgMat *dst);
#define imgSub(src1,src2,dst) Sub(src1,src2,DEF_MATP(dst))

void imgAbsDiff(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *average_diff);
#define imgAbsDiff(src1,src2,dst,average_diff) AbsDiff(src1,src2,DEF_MATP(dst),DEF_FLOATP(average_diff))

void imgIncrease(ImgMat *src,ImgMat *dst,int *value);
#define imgIncrease(src,dst,value) Increase(src,DEF_MATP(dst),DEF_INTP(value))

void imgAffine(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd);
#define imgAffine(src,ps,dst,pd) Affine(src,DEF_POINTP(ps),DEF_MATP(dst),DEF_POINTP(pd))

void imgPerspective(ImgMat *src,ImgPoint *ps,ImgMat *dst,ImgPoint *pd);
#define imgPerspective(src,ps,dst,pd) Perspective(src,DEF_POINTP(ps),DEF_MATP(dst),DEF_POINTP(pd))

void imgOverlap(ImgMat *src1,ImgMat *src2,ImgMat *dst,float *strength);
#define imgOverlap(src1,src2,dst,strength) Overlap(src1,src2,DEF_MATP(dst),DEF_FLOATP(strength))

void imgMirror(ImgMat *src,ImgMat *dst,int type);
#define DEF_MIRROR_TYPE(x) ((#x[0])?INT_VALUE(x):1)
#define imgMirror(src,dst,type)	Mirror(src,DEF_MATP(dst),DEF_MIRROR_TYPE(type))

void imgRotate(ImgMat *src,ImgMat *dst,int angle);
#define DEF_ROTATE_ANGLE(x) ((#x[0])?INT_VALUE(x):90)
#define imgRotate(src,dst,angle) Rotate(src,DEF_MATP(dst),DEF_ROTATE_ANGLE(angle))

void imgThreshold(ImgMat *src,ImgMat *dst,int cn,ImgThreshold *threshold);
#define DEF_THRESHOLD_CN(x) ((#x[0])?INT_VALUE(x):1)
#define imgThreshold(src,dst,cn,threshold) Threshold(src,DEF_MATP(dst),DEF_THRESHOLD_CN(cn),threshold)

void imgFill(ImgMat *src,ImgMat *dst,ImgLocate *start_locate,ImgColor *replace_color,int thresh);
#define DEF_FILL_THRESH(x) ((#x[0])?INT_VALUE(x):20)
#define imgFill(src,dst,start_locate,replace_color,thresh) Fill(src,DEF_POINTER(dst),DEF_POINTER(start_locate),DEF_POINTER(replace_color),DEF_FILL_THRESH(thresh))

void imgBlur(ImgMat *src,ImgMat *dst,int r);
void imgMist(ImgMat *src,ImgMat *dst,int r);

int imgLinearEquation(float *data,float *answer,int num);
float imgSqrt(float x);
float imgSin(int x);
float imgCos(int x);
