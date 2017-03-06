#include <stdio.h>
#include "imp_algo_type.h"
#include "imp_ip_type.h"
// #include "imp_ip_preprocess.h"

#ifndef _IMP_MATH_H_
#define _IMP_MATH_H_

IMP_FLOAT IMP_MATH_Sqrt(IMP_FLOAT f32x);
IMP_FLOAT IMP_MATH_s32Sin(IMP_S32 s32x);
IMP_FLOAT IMP_MATH_f32Sin(IMP_FLOAT f32x);
IMP_FLOAT IMP_MATH_s32Cos(IMP_S32 s32x);
IMP_FLOAT IMP_MATH_f32Cos(IMP_FLOAT f32x);

int IMP_MATH_LinearEquation(float *data,float *answer,int num);
IMP_FLOAT IMP_MATH_Probit(IMP_FLOAT *f32In,IMP_FLOAT f32mean,IMP_FLOAT f32sigma,IMP_S32 s32num);

#define IMP_MATH_ABS(x) (((x)>0)?(x):(-(x)))
#define IMP_MATH_ABSDiff(x,y) (((x)>(y))?((x)-(y)):((y)-(x)))


//////////////////////////////////////////---------------in imp_math_geometry.c---------------/////////////////////////////////////////////////
IMP_S32 IMP_MATH_IsDifferentSide(IMP_POINT_S *pstP1,IMP_POINT_S *pstP2,LINE_S *pstL);
#define IMP_MATH_IsSameSide(P1,P2,L) (!IMP_MATH_IsDifferentSide(P1,P2,L))

IMP_S32 IMP_MATH_IsCross(LINE_S *pstL1,LINE_S *pstL2,IMP_POINT32F_S *pstCrossPoint);
#define IMP_MATH_IsParal(L1,L2) (((L1->stPs.s16X-L1->stPe.s16X)*(L2->stPs.s16Y-L2->stPe.s16Y)) == ((L2->stPs.s16X-L2->stPe.s16X)*(L1->stPs.s16Y-L1->stPe.s16Y)))

IMP_S32 IMP_MATH_IsInPolygon(IMP_POINT_S *PCheck,IMP_PointGroup_S *pPolygon);
#define IMP_MATH_IsOutPolygon(Pcheck,Polygon) (!IMP_MATH_IsInPolygon(Pcheck,Polygon))

#define IMP_MATH_IsInArea(Pcheck,pPoint,n) ({\
	IMP_PointGroup_S Polygon;\
	IMP_IP_GenerateGroupFromArry(Polygon,Point,pPoint,n);\
	flag = IMP_MATH_IsInPolygon(Pcheck,&Polygon);\
	IMP_IP_ReleaseGroup(Polygon,Point);\
	flag;\
})

#define IMP_MATH_IsOutArea(Pcheck,pPoint,n) (!IMP_MATH_IsInArea(Pcheck,pPoint,n))

#define IMP_MATH_IsInRect(Pcheck,Rect) ((Pcheck->s16X>=Rect->s16X1)&&(Pcheck->s16X<=Rect->s16X2)&&(Pcheck->s16Y>=Rect->s16Y1)&&(Pcheck->s16Y<=Rect->s16Y2))
#define IMP_MATH_IsOutRect(Pcheck,Rect) (!IMP_MATH_IsInRect(Pcheck,Rect))

IMP_S32 IMP_MATH_IsConcavePolygon(IMP_POLYGON_S *PPolygon);
#define IMP_MATH_IsConvexPolygon(pPolygon) (!IMP_MATH_IsConcavePolygon(pPolygon))

#define IMP_MATH_IsConcaveArea(pPoint,n) ({\
	IMP_PointGroup_S Polygon;\
	IMP_S32 flag;\
	IMP_IP_GenerateGroupFromArry(Polygon,Point,pPoint,n);\
	flag = IMP_MATH_IsConcavePolygon(&Polygon);\
	IMP_IP_ReleaseGroup(Polygon,Point);\
	flag;\
})

#define IMP_MATH_IsConvexArea(pPoint,n) (!IMP_MATH_IsConcaveArea(pPoint,n))

IMP_PointGroup_S *IMP_MATH_PolygonConcavePoints(IMP_POLYGON_S *PPolygon);
IMP_PointGroup_S *IMP_MATH_ToConvexPolygon(IMP_POLYGON_S *PPolygon);

#define IMP_MATH_ToConvexArea(pPointIn,n,pPointConvex) ({\
IMP_PointGroup_S Polygon;\
	IMP_PointGroup_S *pConvexPoints;\
	IMP_S32 flag;\
	IMP_S32 s32N;\
	IMP_S32 s32i;\
	IMP_IP_GenerateGroupFromArry(Polygon,Point,pPointIn,n);\
	pConvexPoints = IMP_MATH_ToConvexPolygon(&Polygon);\
	s32N = pConvexPoints->s32N;\
	for(s32i=0;s32i<s32N;s32i++)\
		pPointConvex[s32i] = *(pConvexPoints->ppPoint[s32i]);\
	IMP_IP_ReleaseGroup(Polygon,Point);\
	IMP_IP_ReleaseGroup(*pConvexPoints,Point);\
	s32N;\
})

#define IMP_MATH_AreaConcavePoints(pPointIn,n,pPointConcave) ({\
	IMP_PointGroup_S Polygon;\
	IMP_PointGroup_S *pConcavePoints;\
	IMP_S32 flag;\
	IMP_S32 s32N;\
	IMP_S32 s32i;\
	IMP_IP_GenerateGroupFromArry(Polygon,Point,pPointIn,n);\
	pConcavePoints = IMP_MATH_PolygonConvavePoints(&Polygon);\
	s32N = pConcavePoints->s32N;\
	for(s32i=0;s32i<s32N;s32i++)\
		pPointConcave[s32i] = *(pConcavePoints->ppPoint[s32i]);\
	IMP_IP_ReleaseGroup(Polygon,Point);\
	IMP_IP_ReleaseGroup(*pConcavePoints,Point);\
	s32N;\
})
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////---------------in imp_math_fit.c---------------/////////////////////////////////////////////////
// y = af32A[0]*x + af32A[1]
void IMP_MATH_LinearFitLSM(IMP_FLOAT *pf32XIn,IMP_FLOAT *pf32YIn,IMP_S32 s32N,IMP_FLOAT *af32A);

// y = af32A[0]*(1/x) + af32A[1]
#define IMP_MATH_InverseFitLSM_1(pf32XIn,pf32YIn,s32N,af32A) {\
	int s32i;\
	IMP_FLOAT *buff;\
	buff = (IMP_FLOAT *)malloc(s32N*sizeof(IMP_FLOAT));\
	for(s32i=0;s32i<s32N;s32i++)\
		buff[s32i] = 1.0/pf32XIn[s32i];\
	IMP_MATH_LinearFitLSM(buff,pf32YIn,s32N,af32A);\
	free(buff);\
}

//y = 1.0/(af32A[0]*x + af32A[1])
#define IMP_MATH_InverseFitLSM_2(pf32XIn,pf32YIn,s32N,af32A) {\
	int s32i;\
	IMP_FLOAT *buff;\
	buff = (IMP_FLOAT *)malloc(s32N*sizeof(IMP_FLOAT));\
	for(s32i=0;s32i<s32N;s32i++)\
		buff[s32i] = 1.0/pf32YIn[s32i];\
	IMP_MATH_LinearFitLSM(pf32XIn,buff,s32N,af32A);\
	free(buff);\
}


// y = af32A[0]*x^2 + af32A[1]*x + af32A[2]
void IMP_MATH_ParabolaFitLSM(IMP_FLOAT *pf32XIn,IMP_FLOAT *pf32YIn,IMP_S32 s32N,IMP_FLOAT *af32A);

// y = af32A[0]*(1/x^2) + af32A[1]*(1/x) + af32A[2]
#define IMP_MATH_InverseParabolaFitLSM_1(pf32XIn,pf32YIn,s32N,af32A) {\
	int s32i;\
	IMP_FLOAT *buff;\
	buff = (IMP_FLOAT *)malloc(s32N*sizeof(IMP_FLOAT));\
	for(s32i=0;s32i<s32N;s32i++)\
		buff[s32i] = 1.0/pf32XIn[s32i];\
	IMP_MATH_ParabolaFitLSM(buff,pf32YIn,s32N,af32A);\
	free(buff);\
}

// y = 1.0/(af32A[0]*x^2 + af32A[1]*x + af32A[2])
#define IMP_MATH_InverseParabolaFitLSM_2(pf32XIn,pf32YIn,s32N,af32A) {\
	int s32i;\
	IMP_FLOAT *buff;\
	buff = (IMP_FLOAT *)malloc(s32N*sizeof(IMP_FLOAT));\
	for(s32i=0;s32i<s32N;s32i++)\
		buff[s32i] = 1.0/pf32YIn[s32i];\
	IMP_MATH_ParabolaFitLSM(pf32XIn,buff,s32N,af32A);\
	free(buff);\
}

// y = af32A[0]*x^k + af32A[1]*x^(k-1) + …… +af32A[k-1]*x + af32A[k]
void IMP_MATH_PolyFitLSM(IMP_FLOAT *pf32XIn,IMP_FLOAT *pf32YIn,IMP_S32 s32N,IMP_FLOAT *af32A,IMP_S32 s32k);

// y = af32A[0]*(1/x^k) + af32A[1]*(1/x^(k-1)) + …… +af32A[k-1]*(1/x) + af32A[k]
#define IMP_MATH_InversePolyFitLSM_1(pf32XIn,pf32YIn,s32N,af32A,s32k) {\
	int s32i;\
	IMP_FLOAT *buff;\
	buff = (IMP_FLOAT *)malloc(s32N*sizeof(IMP_FLOAT));\
	for(s32i=0;s32i<s32N;s32i++)\
		buff[s32i] = 1.0/pf32XIn[s32i];\
	IMP_MATH_PolyFitLSM(buff,pf32YIn,s32N,af32A,s32k);\
	free(buff);\
}

// y = 1.0/(af32A[0]*x^k + af32A[1]*x^(k-1) + …… +af32A[k-1]*x + af32A[k])
#define IMP_MATH_InversePolyFitLSM_2(pf32XIn,pf32YIn,s32N,af32A) {\
	int s32i;\
	IMP_FLOAT *buff;\
	buff = (IMP_FLOAT *)malloc(s32N*sizeof(IMP_FLOAT));\
	for(s32i=0;s32i<s32N;s32i++)\
		buff[s32i] = 1.0/pf32YIn[s32i];\
	IMP_MATH_PolyFitLSM(pf32XIn,buff,s32N,af32A);\
	free(buff);\
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif