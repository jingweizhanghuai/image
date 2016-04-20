#include <malloc.h>

#define DEBUG

#define TYPE_8U	 0
#define TYPE_8S	 1
#define TYPE_16U	2
#define TYPE_16S	3
#define TYPE_32S	4
#define TYPE_32F	5
#define TYPE_64F	6

#define TYPE_8UC1 0x00//0b000000
#define TYPE_8UC2 0x08//0b001000
#define TYPE_8UC3 0x10//0b010000
#define TYPE_8UC4 0x18//0b011000
#define TYPE_8UC5 0x20//0b100000
#define TYPE_8UC6 0x28//0b101000
#define TYPE_8UC7 0x30//0b110000

#define TYPE_8SC1 0x01//0b000001
#define TYPE_8SC2 0x09//0b001001
#define TYPE_8SC3 0x11//0b010001
#define TYPE_8SC4 0x19//0b011001
#define TYPE_8SC5 0x21//0b100001
#define TYPE_8SC6 0x29//0b101001
#define TYPE_8SC7 0x31//0b110001

#define TYPE_16UC1 0x02//0b00010
#define TYPE_16UC2 0x0A//0b01010
#define TYPE_16UC3 0x12//0b10010
#define TYPE_16UC4 0x1A//0b11010

#define TYPE_16SC1 0x03//0b00011
#define TYPE_16SC2 0x0B//0b01011
#define TYPE_16SC3 0x13//0b10011
#define TYPE_16SC4 0x1B//0b11011

#define TYPE_32SC1 0x04//0b00100
#define TYPE_32SC2 0x0C//0b01100
#define TYPE_32SC3 0x14//0b10100
#define TYPE_32SC4 0x1C//0b11100

#define TYPE_32FC1 0x05//0b00101
#define TYPE_32FC2 0x0D//0b01101
#define TYPE_32FC3 0x15//0b10101
#define TYPE_32FC4 0x1D//0b11101

#define TYPE_64FC1 0x06//0b00110
#define TYPE_64FC2 0x0E//0b01110
#define TYPE_64FC3 0x16//0b10110
#define TYPE_64FC4 0x1E//0b11110

typedef struct ImgMat
{
	int type;

	/* for internal use only */
	int* hidinfo;
	/*0:mat_address*/
	/*1:free_valid*/
	/*2:orign_width*/
	/*3:orign_height*/
	/*4:type*/
	/*5:width*/
	/*6:height*/
	/*7:data*/
	/*8:cn*/
	/*9:depth*/

	union
	{
		unsigned char* ptr;
		short* s;
		int* i;
		float* fl;
		double* db;
	}data;

	union
	{
		int rows;
		int height;
	};

	union
	{
		int cols;
		int width;
	};
	
	int cn;	
	int size;
	
	void *memory[5];	
	int memory_valid[5];
	
}ImgMat;


#define QY_THRESH_BINARY	0 
#define QY_THRESH_BINARY_INV	1	
#define QY_THRESH_TRUNC	2	
#define QY_THRESH_TOZERO	3	
#define QY_THRESH_TOZERO_INV	4	

#define THRESHOLD_BINARY 	0
#define THRESHOLD_CEILING	1
#define THRESHOLD_FLOOR		2


typedef struct ImgConvKernel
{
	int nCols;
	int nRows;
	int anchorX;
	int anchorY;
	int *values;
	int nshiftR;
}ImgConvKernel;

#define QY_SHAPE_RECT		0
#define QY_SHAPE_CROSS	 1
#define QY_SHAPE_ELLIPSE 2
#define QY_SHAPE_CUSTOM	100

typedef struct ImgRect
{
	int x;
	int y;
	union
	{
		int rows;
		int height;
	};

	union
	{
		int cols;
		int width;
	};
}ImgRect;

typedef struct ImgSize
{
	union
	{
		int rows;
		int height;
	};

	union
	{
		int cols;
		int width;
	};
}ImgSize;

int mate_type;
#define UP_LEFT_SCROP 1
#define UP_RIGHT_SCROP 2
#define DOWN_LEFT_SCROP 3
#define DOWN_RIGHT_SCROP 4
#define RESIZE 5
#define UP_LEFT_CROP 6
#define UP_RIGHT_CROP 7
#define DOWN_LEFT_CROP 8
#define DOWN_RIGHT_CROP 9

#define imgMatSize(src,size) imgMateMat(src,size->height,dst->width,mate_type);						 
#define imgMate(src,dst) imgMateMat(src,dst->height,dst->width,mate_type);

typedef struct ImgRectList
{
	ImgRect *rect;
	int num;
}ImgRectList;


typedef struct ImgROI
{
	ImgMat *mat;
	ImgRect *rect;	
}ImgROI;

typedef struct ImgVector
{
	int x;
	int y;
}ImgVector;

#define ImgPoint ImgVector
#define ImgLocate ImgVector
// typedef struct ImgPoint
// {
	// int x;
	// int y;
// }ImgPoint;

typedef struct ImgColor
{
	union
	{
		int r;
		int gray;
	};
	int g;
	int b;
	int alpha;
}ImgColor;

typedef struct ImgDPList
{
	ImgPoint pixel_list[500];
	int pixel_frame[500];
	int pixel_num;	
	
	ImgPoint dead_list[400];
	int dead_num1;
	int dead_num2;
	
	unsigned char replace_list[200];
}ImgDPList;

typedef struct ImgColorLUT
{
	int cn;
	union
	{
		int r[256];
		int p[256];
	};
	int g[256];
	int b[256];
}ImgColorLUT;


#define NOT_CHANGE_WITH_THRESHOLD 256
typedef struct ImgThreshold
{
	int thresh_num;
	
	int thresh[9];
	int value[10];
}ImgThreshold;


