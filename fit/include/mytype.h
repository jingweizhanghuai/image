#ifndef _MA_TYPE_H_
#define _MA_TYPE_H_

#include <stdio.h>

typedef struct IMP_ThreshSet_S
{
	int s32Floor;
	int s32Ceiling;
	int s32Value;
}IMP_ThreshSet_S;

#define IMP_Rect_S IMP_RECT_S
#define IMP_Point_S IMP_POINT_S

//group.__kin: 0:非连续地址，1：连续地址，2：来自数组，3：来自链表。

#define maInitGroup(group,Type) {(group).pp##Type = NULL;(group).s32N=0;(group).__kind = 0;(group).__startaddr = NULL;}

#define maCreateGroup(group,Type,n) {\
	int s32i;\
	((group).pp##Type) = (IMP_##Type##_S **)malloc(n<<2);\
	(((group).pp##Type)[0]) = (IMP_##Type##_S *)malloc(n*sizeof(IMP_##Type##_S));\
	for(s32i=1;s32i<n;s32i++)\
		(((group).pp##Type)[s32i]) = (((group).pp##Type)[s32i-1])+1;\
	(group).s32N = n;\
	(group).__kind = 1;\
	(group).__startaddr = (((group).pp##Type)[0]);\
}

#define maReleaseGroup(group,Type) {\
	int s32i;\
	if((group).__kind == 0)\
		for(s32i=0;s32i<(group).s32N;s32i++)\
			free(((group).pp##Type)[s32i]);\
	else if((group).__kind == 1)\
		free((group).__startaddr);\
	free(((group).pp##Type));\
}

#define maGenerateGroupFromAddr(group,Type,p,n) {\
	int s32i;\
	((group).pp##Type) = (IMP_##Type##_S **)malloc(n<<2);\
	for(s32i=0;s32i<n;s32i++)\
		(((group).pp##Type)[s32i]) = p+s32i;\
	(group).s32N = n;\
	(group).__kind = 1;\
	(group).__startaddr = p;\
}

#define maGenerateGroupFromArry(group,Type,p,n) {\
	int s32i;\
	((group).pp##Type) = (IMP_##Type##_S **)malloc(n<<2);\
	for(s32i=0;s32i<n;s32i++)\
		(((group).pp##Type)[s32i]) = p+s32i;\
	(group).s32N = n;\
	(group).__kind = 2;\
	(group).__startaddr = p;\
}

#define maFreeCroup(group,Type) {\
	if(((group).pp##Type)!=NULL)\
		free(((group).pp##Type));\
}

#define POINTS_SIZE 4096
typedef struct IMP_POINTS_S
{
	IMP_POINT_S point[POINTS_SIZE];
	int s32N;
	int __kind;
	void *__startaddr;
}IMP_POINTS_S;

typedef struct IMP_PointsGroup_S
{
	IMP_POINTS_S **ppPoints;
	int s32N;
	int __kind;
	void *__startaddr;
}IMP_PointsGroup_S;

typedef struct IMP_RectGroup_S
{
	IMP_RECT_S **ppRect;
	int s32N;
	int __kind;
	void *__startaddr;
}IMP_RectGroup_S;

typedef struct IMP_PointGroup_S
{
	IMP_POINT_S **ppPoint;
	int s32N;
	int __kind;
	void *__startaddr;
}IMP_PointGroup_S;

#define IMP_POLYGON_S IMP_PointGroup_S

#define IMP_METHOD_LINEAR 0
#define IMP_METHOD_NN 1

typedef struct IMP_HaarSet_S
{
	IMP_BOOL boolValid;
	
	IMP_RECT_S stRect1;
	IMP_FLOAT w1;
	
	IMP_RECT_S stRect2;
	IMP_FLOAT w2;
	
	IMP_RECT_S stRect3;
	IMP_FLOAT w3;
	
	IMP_RECT_S stRect4;
	IMP_FLOAT w4;
	
	IMP_BOOL boolRect3Valid;
	IMP_BOOL boolRect4Valid;
}IMP_HaarSet_S;

typedef struct IMP_HaarSetGroup_S 
{
	IMP_HaarSet_S **ppHaarSet;
	int s32N;
	int __kind;
	void *__startaddr;
}IMP_HaarSetGroup_S;

#define USE_RANDOM_NUM -1
#endif