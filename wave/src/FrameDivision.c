#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "wavetype.h"
#include "exception.h"

#define ABS(x) (((x)>0)?(x):(0-(x)))

/////////////////////////////////////////////////////////
// 接口功能:
//  生成分帧索引
//
// 参数：
//  (I)wav_size(NO) - 待分帧的波形的长度
//  (I)div_num(自适应) - 所分得的帧数，默认为根据wav_size、div_size、div_overlap计算得到帧数
//  (I)div_size(自适应) - 所分得的帧长度，默认为根据wav_size、div_num、div_overlap计算得到帧长度
//  (I)div_overlap(自适应) - 所分得的帧覆盖率，默认为根据wav_size、div_num、div_size计算得到帧覆盖率
//  注：div_num、div_size、div_overlap三者中至少应有两个为输入的有效值。
//
// 返回值：
//  分帧索引
/////////////////////////////////////////////////////////
MAWaveIndex *maGetFrameIndex(int wav_size,int div_num,int div_size,float div_overlap)
{
    int step;
    int i;
    
    MAWaveIndex *index;
    index = (MAWaveIndex *)malloc(sizeof(MAWaveIndex));

    if(div_overlap != MA_DEFAULT)
    {
        maException(((div_overlap >=1.0)||(div_overlap <0.0)),"invilid input division overlap",EXIT);
        index->frame_overlap = div_overlap;
    }
    else
        index->frame_overlap = -1.0;

    if(div_num != MA_DEFAULT)
    {
        maException((div_num <1),"invilid input frame division number",EXIT);
        index->frame_num = div_num;
    }
    else
        index->frame_num = -1;
    
    if(div_size != MA_DEFAULT)
    {
        maException((div_size <1),"invilid input frame division size",EXIT);
        index->frame_size = div_size;
    }
    else
        index->frame_size = -1;
    
    maException((index->frame_size+index->frame_num+(int)(index->frame_overlap)<0),"input para is not enough",EXIT);

    if(index->frame_num < 0)
    {
        step = index->frame_size-(int)(((float)index->frame_size)*index->frame_overlap);
        index->frame_num = (wav_size-index->frame_size)/step +1;
    }
    else if(index->frame_overlap < 0.0)
    {
        step = (wav_size-index->frame_size)/(index->frame_num-1);
        index->frame_overlap = ((float)(index->frame_size-step))/((float)index->frame_size);
    }
    else
    {
        index->frame_size = (int)(((float)wav_size)/(((float)(index->frame_num-1))*(1.0-index->frame_overlap)+1.0)+0.5);
        step = (wav_size-index->frame_size)/(index->frame_num-1);
        
        if(div_size != MA_DEFAULT)
            maException((ABS(index->frame_size-div_size)>1),"invilid input",EXIT);
    }
    maException((wav_size<=index->frame_size)||(wav_size<=index->frame_num),"invalid input",EXIT);
        
    index->index = (int *)malloc(index->frame_num*sizeof(int));   
    printf("index is %d\n",index->index);
    index->index[0] = 0;
    for(i=1;i<index->frame_num;i++)
        index->index[i] = index->index[i-1]+step;

    return index;
}

void maReleaseWaveIndex(MAWaveIndex *index)
{
    if(index->index != NULL)
        free(index->index);
    
    free(index);
}

/////////////////////////////////////////////////////////
// 接口功能:
//  获取分帧后的第n帧波形
//
// 参数：
//  (I)src(NO) - 输入的波形（已经过分帧）
//  (I)n(NO) - 帧序号
//
// 返回值：
//  指向分帧波形的指针
/////////////////////////////////////////////////////////
void maWaveFrame(MAWave *src,MAWaveIndex *index,MAWave *dst,int n)
{
    int i;
    
    maException((src == NULL)||(dst == NULL),"invalid input",EXIT);
    maException((index == NULL),"no frame index",EXIT);
    maException((index->index == NULL),"invalid frame index",EXIT);
    maException((n>index->frame_num),"invalid operate",EXIT);
    
    if(dst->memory != NULL)
    {
        free(dst->memory);
        dst->memory = NULL;
    }
    
    dst->size = index->frame_size;
    dst->cn = src->cn;
    dst->info = src->info;
    
    for(i=0;i<src->cn;i++)
        dst->data[i] = src->data[i] + index->index[n];
    }

/////////////////////////////////////////////////////////
// 接口功能:
//  将单通道的多帧波形转换为多通道的单帧波形
//
// 参数：
//  (I)src(NO) - 输入的波形（已经过分帧）
//
// 返回值：
//  指向多通道单帧波形的指针
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.4.17
/////////////////////////////////////////////////////////
void maWaveFrameMat(MAWave *src,MAWaveIndex *index,MAWave *dst)
{
    int cn;
    int i;
    
    maException((src == NULL),"invalid input",EXIT);
    maException((index == NULL),"no frame index",EXIT);
    maException((index->index == NULL),"no frame index",EXIT);
    maException((src->cn != 1),"invalid operate",EXIT);
    
    if((int)dst<=0)
        dst = src;
    if(dst!=src)
        if(dst->memory != NULL)
        {
            free(dst->memory);
            dst->memory = NULL;
        }
        
    if(index->frame_num > MA_MAX_WAVE_CN)
    {
        maException(1,"frame num is larger than max wave channel",NULL);
        cn = MA_MAX_WAVE_CN;
    }
    else
        cn = index->frame_num;
    
    dst->size = index->frame_size;
    dst->cn = cn;
    dst->info = src->info;
    
    for(i=0;i<cn;i++)
        dst->data[i] = src->data[0] + index->index[i];
}
