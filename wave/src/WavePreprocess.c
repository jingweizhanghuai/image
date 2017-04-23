#include <stdio.h>
#include <stdlib.h>
#include "wavetype.h"
#include "exception.h"

float maMathFloatCos(float x);
MAWave *maCreateWave(int size,int cn);

/////////////////////////////////////////////////////////
// 接口功能:
//  生成Hanning窗
//
// 参数：
//  (O)win(NO) - 输出的窗波形
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maHanningWin(MAWave *win)
{
    int i;
    float data;
    int size;

    maException((win == NULL),"invalid input",EXIT);
    size = win->size;
    
    for(i=0;i<size/2;i++)
    {
        data = 0.5-0.5*maMathFloatCos(360.0*((float)i)/((float)size));
        win->data[0][i]= data;
        win->data[0][size-1-i] = data;
    }
    win->data[0][i] = 0.5-0.5*maMathFloatCos(360.0*((float)i)/((float)size));      

}

/////////////////////////////////////////////////////////
// 函数功能:
//  生成Hamming窗
//
// 参数：
//  (O)win(NO) - 输出的窗波形
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maHammingWin(MAWave *win)
{
    int i;
    float data;
    int size;

    maException((win == NULL),"invalid input",EXIT);
    size = win->size;
    
    for(i=0;i<size/2;i++)
    {
        data = 0.54-0.46*maMathFloatCos(360.0*((float)i)/((float)size));
        win->data[0][i]= data;
        win->data[0][size-1-i] = data;
    }
    win->data[0][i] = 0.54-0.46*maMathFloatCos(360.0*((float)i)/((float)size));
}

/////////////////////////////////////////////////////////
// 接口功能:
//  生成三角窗
//
// 参数：
//  (O)win(NO) - 输出的窗波形
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maFejerWin(MAWave *win)
{
    int i;
    float data;
    float step;
    int size;

    maException((win == NULL),"invalid input",EXIT);
    size = win->size;
    
    data = 0.0;
    step = 2.0/((float)size);
    for(i=0;i<size/2;i++)
    {
        win->data[0][i]= data;
        win->data[0][size-1-i] = data;
        data = data + step;
    }
    
    if(size%2)
        win->data[0][i] = data;
}

/////////////////////////////////////////////////////////
// 接口功能:
//  生成布莱克曼窗
//
// 参数：
//  (O)win(NO) - 输出的窗波形
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maBlackmanWin(MAWave *win)
{
    int i;
    float data;
    int size;

    maException((win == NULL),"invalid input",EXIT);
    size = win->size;
    
    for(i=0;i<size/2;i++)
    {
        data = 0.42-0.5*maMathFloatCos(360.0*((float)i)/((float)size))
               + 0.08*maMathFloatCos(720.0*((float)i)/((float)size));
        win->data[0][i]= data;
        win->data[0][size-1-i] = data;
    }
    win->data[0][i] = 0.42-0.5*maMathFloatCos(360.0*((float)i)/((float)size))
                    + 0.08*maMathFloatCos(720.0*((float)i)/((float)size));
}

/////////////////////////////////////////////////////////
// 接口功能:
//  对波形加窗
//
// 参数：
//  (I)src(NO) - 输出的待处理波形
//  (I)win(NO) - 输出的窗波形
//  (O)dst(src) - 输出的加窗后的波形
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maWaveWindow(MAWave *src,MAWave *win,MAWave *dst)
{    
    int cn;
    int i;
    
    maException((src == NULL),"invalid input",EXIT);
    maException((win == NULL),"invalid input",EXIT);
    maException((win->size != src->size)
               ||(win->cn != 1),"invalid windows",EXIT);
        
    if((dst == NULL)||((int)dst == MA_DEFAULT))
        dst = src;
    else
        maRedefineWave(dst,src->size,src->cn);

    for(cn = 0;cn<src->cn;cn++)
        for(i=0;i<src->size;i++)
            dst->data[cn][i] = src->data[cn][i] * win->data[cn][i];
}
    
    

#define MA_WIN_HANNING     1
#define MA_WIN_HAMMING     2
#define MA_WIN_FEJER       3
#define MA_WIN_BLACKMAN    4

/////////////////////////////////////////////////////////
// 接口功能:
//  对波形加窗
//
// 参数：
//  (I)src(NO) - 输出的待处理波形
//  (O)dst(src) - 输出的加窗后的波形
//  (I)winSelect(MA_WIN_HAMMING) - 窗类型的选择
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maWaveWithWindow(MAWave *src,MAWave *dst,int winSelect)
{
    MAWave *win;
       
    maException((src == NULL),"invalid input",EXIT);
    
    win = maCreateWave(src->size,1);
    
    if(winSelect == MA_DEFAULT)
        maHammingWin(win);
    else if(winSelect == MA_WIN_HANNING)
        maHanningWin(win);
    else if(winSelect == MA_WIN_HAMMING)
        maHammingWin(win);
    else if(winSelect == MA_WIN_FEJER)
        maFejerWin(win);
    else if(winSelect == MA_WIN_BLACKMAN)
        maBlackmanWin(win);
    else
        maException(1,"invalid window select",EXIT);
    
    maWaveWindow(src,win,dst);
    
    maReleaseWave(win);    
}
    
    
        