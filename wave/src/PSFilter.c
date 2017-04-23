#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wavetype.h"
#include "exception.h"

typedef struct TriangleFilter
{
    int l;
    int c;
    int h;
    
    float step1;
    float step2;
}TriangleFilter;

#define TRAINGLE_PARA(low,med,high,para) {\
    para.c = med;\
    \
    if(low==med)\
        para.l = med-1;\
    else\
        para.l = low;\
    \
    if(med==high)\
        para.h = med+1;\
    else\
        para.h = high;\
    \
    para.step1 = 2.0/((float)((para.h-para.l)*(para.c-para.l)));\
    para.step2 = 2.0/((float)((para.h-para.l)*(para.h-para.c)));\
}

#define USE_FILTER(wave,filter,feature) {\
    float k;\
    \
    feature = 0.0;\
    k = 0.0;\
    for(j=filter.l+1;j<=filter.c;j++)\
    {\
        k = k + filter.step1;\
        feature = feature + (wave->data[cn][j])*k;\
    }\
    for(j=filter.c+1;j<=filter.h;j++)\
    {\
        k = k - filter.step2;\
        feature = feature + (wave->data[cn][j])*k;\
    }\
}

static TriangleFilter g_uniform_para[128];
static int g_uniform_low_frequency = 0;
static int g_uniform_high_frequency = 0;
static int g_uniform_num = 0;

void GetUniformPara()
{
    int i;
    int frq[130];
    float frequency;
    float step;
    
    step = ((float)(g_uniform_high_frequency - g_uniform_low_frequency))/((float)(g_uniform_num+1));
    frequency = (float)g_uniform_low_frequency;
    
    frq[0] = g_uniform_low_frequency;
    for(i=1;i<=g_uniform_num;i++)
    {
        frequency = frequency + step;
        frq[i] = (int)(frequency + 0.5);
    }
    frq[i] = g_uniform_high_frequency;
    
    for(i=0;i<g_uniform_num;i++)
        TRAINGLE_PARA(frq[i],frq[i+1],frq[i+2],g_uniform_para[i]);
}

/////////////////////////////////////////////////////////
// 接口功能:
//  对功率谱使用均匀分布的三角滤波器滤波
//
// 参数：
//  (I)src(NO) - 输入的功率谱
//  (I)Start(0) - 滤波的起始位置，对应滤波器的最低频率
//  (I)End(src->size) - 滤波的终止位置，对应滤波器的最高频率
//  (I)NumCutOffFreq(NO) - 使用的滤波器个数
//  (O)MelFilterFeature(NO) - 输出的各通道滤波结果
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maUniformFilterBank(MAWave *src, int Start, int End, int NumUniformFilter, float** UniformFilterFeature)
{
    int i,j;
    int cn;
    
    float k;
    
    maException((src == NULL),"invalid input",EXIT);
    maException((src->info.wave_type != MA_WAVE_PS),"please check input wave type",NULL);
    maException((UniformFilterFeature == NULL),"invalid input",EXIT);
    if(Start == MA_DEFAULT)
        Start = 0;
    maException((Start < 0),"invalid input",EXIT);
    if(End == MA_DEFAULT)
        End = src->size;
    maException((End > src->size),"invalid input",EXIT);
    maException((NumUniformFilter > 128)||(NumUniformFilter <0),"invalid operate",EXIT);    
    
    if((g_uniform_low_frequency != Start)||(g_uniform_high_frequency != End)||(g_uniform_num != NumUniformFilter))
    {
        g_uniform_low_frequency = Start;
        g_uniform_high_frequency = End;
        g_uniform_num = NumUniformFilter;
        GetUniformPara();
    }
    
    for(cn = 0;cn<src->cn;cn++)
        for(i=0;i<NumUniformFilter;i++)
            USE_FILTER(src,g_uniform_para[i],UniformFilterFeature[cn][i]);
}

static TriangleFilter g_mel_para[128];
static int g_mel_low_frequency = 0;
static int g_mel_high_frequency = 0;
static int g_mel_num = 0;

#define HZ2MEL(hz) (float)(1127.0*log(1.0 + ((double)(hz))/700.0))
#define MEL2HZ(mel) (float)(700.0*exp(((double)(mel))/1127.0) - 700.0)

void GetMelPara()
{
    int i;
    float h2w_low,h2w_high,h2w;
    float hz;
    int mel[130];
    
    h2w_low=HZ2MEL(g_mel_low_frequency);
    h2w_high=HZ2MEL(g_mel_high_frequency);
    h2w=(h2w_high-h2w_low)/(float)(g_mel_num+1);
    
    mel[0] = g_mel_low_frequency;
    hz = h2w_low;
    for(i=1;i<=g_mel_num;i++)
    {
        hz = hz + h2w;
        mel[i] = (int)(MEL2HZ(hz)+0.5);
    }
    mel[i] = g_mel_high_frequency;
    
    for(i=0;i<g_mel_num;i++)
        TRAINGLE_PARA(mel[i],mel[i+1],mel[i+2],g_mel_para[i]);
}
    
/////////////////////////////////////////////////////////
// 接口功能:
//  对功率谱使用Mel滤波器滤波
//
// 参数：
//  (I)src(NO) - 输入的功率谱
//  (I)Start(0) - 滤波的起始位置，对应滤波器的最低频率
//  (I)End(src->size) - 滤波的终止位置，对应滤波器的最高频率
//  (I)NumCutOffFreq(NO) - 使用的滤波器个数
//  (O)MelFilterFeature(NO) - 输出的各通道滤波结果
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maMelFilterBank(MAWave *src, int Start, int End, int NumMelFilter, float** MelFilterFeature)
{
    int i,j;
    int cn;
    
    float k;
    
    maException((src == NULL),"invalid input",EXIT);
    maException((src->info.wave_type != MA_WAVE_PS),"please check input wave type",NULL);
    maException((MelFilterFeature == NULL),"invalid input",EXIT);
    if(Start == MA_DEFAULT)
        Start = 0;
    maException((Start < 0),"invalid input",EXIT);
    if(End == MA_DEFAULT)
        End = src->size;
    maException((End > src->size),"invalid input",EXIT);
    maException((NumMelFilter > 128)||(NumMelFilter <0),"invalid operate",EXIT);    
    
    if((g_mel_low_frequency != Start)||(g_mel_high_frequency != End)||(g_mel_num != NumMelFilter))
    {
        g_mel_low_frequency = Start;
        g_mel_high_frequency = End;
        g_mel_num = NumMelFilter;
        GetMelPara();
    }
    
    for(cn = 0;cn<src->cn;cn++)
        for(i=0;i<NumMelFilter;i++)
            USE_FILTER(src,g_mel_para[i],MelFilterFeature[cn][i]);
}
