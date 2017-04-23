#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavetype.h"
#include "exception.h"

MAWave* maCreateWave(int size,int cn);
MAWave* maCreateWaveHeader(int size,int cn);
MAWave* maNewWave();

typedef struct WAV_HEADER {
    short type;
    short channel;
    int frequency;
    int bps;
    short datanum;
    short databits;
}WAV_HEADER;

/////////////////////////////////////////////////////////
// 接口功能:
//  读取WAV文件
//
// 参数：
//  (I)file_name(NO) - WAV波形的路径
//  (O)wave_dst(NO) - 文件所读入的波形
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maReadWAV(char *file_name,MAWave *wave_dst)
{
    FILE *f;
    
    short *data;
    
    int locate;

    int block_size;
    void *block_data;
    
    int riff_size;
    
    WAV_HEADER *fmt;
    
    int i,n;
    
    char name[5];
    name[4] = 0;
    
    maException((wave_dst == NULL),"invalid input",EXIT);
    
    f = fopen(file_name,"rb");
    maException((f==NULL),"file cannot open",EXIT);
    
    fread(name,4,1,f);
    // printf("%s\n",name);
    maException((f==NULL),"RIFF error!",EXIT);
    
    fread(&riff_size,4,1,f);
    // printf("filesize is %d\n",riff_size);
    
    riff_size = riff_size + 8;
    
    fread(name,4,1,f);
    // printf("%s\n",name);
    maException((memcmp(name,"WAVE",4)!=0),"WAVE error!",{return;});
        
    locate = ftell(f);
    maException((riff_size-locate<8),"WAVE error!",{return;});
    
    while(1)
    {
        fread(name,4,1,f);
        // printf("%s\n",name);
        fread(&block_size,4,1,f);
        
        locate = locate + 8;
        maException((riff_size-locate<block_size),"??? error!",{return;});
        
        // fread(block_data,block_size,1,f);
        if(memcmp(name,"fmt ",4)==0)
        {
            fread(&fmt,block_size,1,f);
            fmt = (WAV_HEADER *)block_data;
            // printf("type is %d\n",fmt->type);
            // printf("channel is %d\n",fmt->channel);
            // printf("frequency is %d\n",fmt->frequency);
            // printf("bps is %d\n",fmt->bps);
            // printf("datanum is %d\n",fmt->datanum);
            // printf("databits is %d\n",fmt->databits);
            
            wave_dst->info.frequency = fmt->frequency;
            wave_dst->info.wave_type = MA_WAVE_TD;
        }
        else if(memcmp(name,"data",4)==0)
        {
            maRedefineWave(wave_dst,block_size/((wave_dst->cn)<<1),fmt->channel);
            data = (short *)malloc(block_size);
            fread(data,block_size,1,f);
            if(wave_dst->cn == 1)
            {
                for(i=0;i<wave_dst->size;i++)
                    wave_dst->data[0][i] = ((float)data[i])/32768.0;
            }
            else if(wave_dst->cn == 2)
            {
                wave_dst->data[1] = wave_dst->data[0];
                for(i=0,n=0;i<wave_dst->size;i++,n=n+2)
                {
                    wave_dst->data[0][i] = ((float)data[n])/32768.0;
                    wave_dst->data[1][i] = ((float)data[n+1])/32768.0;
                }
            }
            else
                maException(1,"WAV channel error!",EXIT);

            free(data);
            // printf("datasize is %d\n",wave_dst->size);
            
            goto readwav_end;
        }
        else
            fseek(f,block_size,SEEK_CUR);
        
        locate = ftell(f);
        if((riff_size-locate)<8)
            break;
    }

readwav_end:    
    fclose(f);
    f = NULL;
}

/////////////////////////////////////////////////////////
// 接口功能:
//  写入WAV文件头
//
// 参数：
//  (I)sample_rate(44100) - WAV波形文件的采样率
//  (I)cn(1) - WAV波形文件的通道数
//  (I)data_num(NO) - WAV波形文件中数据点的个数
//  (I)f(NO) - 所写入文件的文件指针
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maSaveWAVHeader(int sample_rate,int cn,int data_num,FILE *f)
{
    WAV_HEADER fmt;
    int datasize;
    int data;
    
    maException((f==NULL),"file cannot open",EXIT);
    maException((data_num<=0),"invalid input",EXIT);
    
    if(sample_rate == MA_DEFAULT)
        sample_rate = 44100;
    maException((sample_rate <= 0),"invalid input",EXIT);
    
    if(cn == MA_DEFAULT)
        cn = 1;
    maException((cn <= 0),"invalid input",EXIT);
    
    fseek(f,0,SEEK_SET);
    
    datasize = data_num*2*cn;
    
    // printf("sample_rate is %d\n",sample_rate);
    // printf("data_num is %d\n",data_num);
    
    fmt.type = 1;
    fmt.channel = cn;
    fmt.frequency = sample_rate;
    fmt.bps = sample_rate<<1;
    fmt.datanum = cn<<1;
    fmt.databits = 16*cn;
    
    fwrite("RIFF",1,4,f);
    data = datasize+36;
    fwrite(&data,4,1,f);
    fwrite("WAVE",1,4,f);
    fwrite("fmt ",1,4,f);
    data = sizeof(WAV_HEADER);
    fwrite(&data,4,1,f);
    fwrite(&fmt,sizeof(WAV_HEADER),1,f);
    fwrite("data",1,4,f);
    fwrite(&datasize,4,1,f);
}

/////////////////////////////////////////////////////////
// 接口功能:
//  写入WAV文件
//
// 参数：
//  (I)wave_src(NO) - 所写入的波形
//  (O)file_name(NO) - WAV波形的路径
//  
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maSaveWAV(MAWave *wave_src,const char *file_name)
{
    FILE *f;
    int i,n;
    short *data;
    int value;
    
    maException((wave_src == NULL),"invalid input",EXIT);
    maException((wave_src->info.wave_type != MA_WAVE_TD),"the write wave is not time domain wave",NULL);
    
    f = fopen(file_name,"wb");
    maException((f==NULL),"file cannot open",EXIT);
    
    maSaveWAVHeader(wave_src->info.frequency,wave_src->cn,wave_src->size,f);
    
    fseek(f,44,SEEK_SET);
    
    data = (short *)malloc(sizeof(short)*wave_src->cn*wave_src->size);    
    if(wave_src->cn == 1)
    {
        for(i=0;i<wave_src->size;i++)
        {
            if(wave_src->data[0][i] >= 1.0)
                data[i] = 32767;
            else if(wave_src->data[0][i] <= -1.0)
                data[i] = -32768;
            else
                data[i] = (short)(wave_src->data[0][i]*32767.0);
        }
    }
    else if(wave_src->cn == 2)
    {
        for(i=0,n=0;i<wave_src->size;i++,n=n+2)
        {
            if(wave_src->data[0][i] >= 1.0)
                data[n] = 32767;
            else if(wave_src->data[0][i] <= -1.0)
                data[n] = -32768;
            else
                data[n] = (short)(wave_src->data[0][i]*32767.0);
            
            if(wave_src->data[1][i] >= 1.0)
                data[n+1] = 32767;
            else if(wave_src->data[1][i] <= -1.0)
                data[n+1] = -32768;
            else
                data[n+1] = (short)(wave_src->data[1][i]*32767.0);            
        }
    }
    else
        maException(1,"WAV channel error!",EXIT);
    
    fwrite(data,sizeof(short),wave_src->cn*wave_src->size,f);
    free(data);
    fclose(f);
    f = NULL;
}

static MAWave *g_wav_read_wave = NULL;
static MAWave *g_wav_read_data = NULL;
static int g_wav_read_rate_set = 0;
static int g_wav_read_size = 0;

/////////////////////////////////////////////////////////
// 接口功能:
//  开始导出.wav文件到数据流
//
// 参数：
//  (I)file_name(NO) - WAV波形的路径
//  (I)sample_rate(44100) - 目标数据流的采样率
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maStartReadWAV(char *file_name,int sample_rate)
{
    g_wav_read_data = maNewWave();    
    maReadWAV(file_name,g_wav_read_data);
    maReSampleWithAverage(g_wav_read_data,NULL,NULL,sample_rate);
    g_wav_read_size = 0;
}
/*

    FILE *f;
    int cn;
    int locate;
    
    int i,j;

    int block_size;
    
    int riff_size;
    
    WAV_HEADER fmt;
    
    short *data;
    
    char name[5];
    name[4] = 0;
    
    if(sample_rate == MA_DEFAULT)
        sample_rate = 44100;
    maException((sample_rate<0),"invalid input",EXIT);
    g_wav_read_rate_set = sample_rate;
    
    g_wav_read_wave = NULL;
    
    f = fopen(file_name,"rb");
    maException((f==NULL),"file cannot open",EXIT);
    
    fread(name,4,1,f);
    // printf("%s\n",name);
    maException((f==NULL),"RIFF error!",EXIT);
    
    fread(&riff_size,4,1,f);
    // printf("filesize is %d\n",riff_size);
    
    riff_size = riff_size + 8;
    
    fread(name,4,1,f);
    // printf("%s\n",name);
    maException((memcmp(name,"WAVE",4)!=0),"WAVE error!",{return;});
        
    locate = ftell(f);
    maException((riff_size-locate<8),"WAVE error!",{return;});
    
    memset(&fmt,sizeof(WAV_HEADER),0);
    
    while(1)
    {
        fread(name,4,1,f);
        // printf("%s\n",name);
        fread(&block_size,4,1,f);
        
        locate = locate + 8;
        maException((riff_size-locate<block_size),"file error!",{return;});

        if(memcmp(name,"fmt ",4)==0)
        {
            fread(&fmt,block_size,1,f);           
            
            maException((fmt.channel>2)||(fmt.channel<1),"WAV channel error!",EXIT);
            
            cn = fmt.channel;
        }
        else if(memcmp(name,"data",4)==0)
        {
            maException((fmt.datanum == 0),"invalid format type",EXIT);
            g_wav_read_size = block_size/(fmt.datanum);
            g_wav_read_data = maCreateWave(g_wav_read_size,1);
            g_wav_read_data->info.frequency = fmt.frequency;
            
            data = (short *)malloc(cn*g_wav_read_size*sizeof(short));
            fread(data,sizeof(short),cn*g_wav_read_size,f);
            for(i=0;j=0;i<g_wav_read_size;i++,j=j+cn)
                g_wav_read_data->data[0][i] = ((float)data[j])/32768.0;
            
            free(data);
            goto start_read_wav_end;
        }
        else
            fseek(f,block_size,SEEK_CUR);
        
        locate = ftell(f);
        if((riff_size-locate)<8)
            break;
    }
start_read_wav_end:
    fclose(f);
    f = NULL;    
}
*/

/////////////////////////////////////////////////////////
// 接口功能:
//  获取.wav文件的数据
//
// 参数：
//  (I)size(2048) - 读出流数据的大小（数据点的个数）
//
// 返回值：
//  所读出波形的指针，读取失败时返回空指针，读取到的波形为 MA_TYPE_F32 类型
/////////////////////////////////////////////////////////
MAWave *maReadWAVStream(int size)
{
    if(g_wav_read_wave == NULL)
    {
        g_wav_read_wave = maCreateWaveHeader(size,1);
        g_wav_read_wave->info = g_wav_read_data->info;
    }
    else
        g_wav_read_wave->size = size;

    g_wav_read_wave->data[0] = g_wav_read_data->data[0];
    
    g_wav_read_size = g_wav_read_size - size;
    g_wav_read_data->data[0] = g_wav_read_data->data[0] + size;
    
    return g_wav_read_wave;
}
    
/*
    int i,j;
    short *data;
    
    if(size == MA_DEFAULT)
        size = 2048;
    maException((size<=0),"invalid input",EXIT);
    
    if(g_wav_read_wave == NULL)
    {
        g_wav_read_wave = maCreateWave(size,cn);
        g_wav_read_wave->info.frequency = g_wav_read_rate_set;
        g_wav_read_wave->info.wavetype = MA_WAVE_TD;
    }
    else
        maRedefineWave(g_wav_read_wave,size,cn);
    
    if(g_wav_read_data->info.frequency!=g_wav_read_rate_set)
        size = size * g_wav_read_data->info.frequency/g_wav_read_rate_set + 1;
    
    if(size > g_wav_read_size)
        return NULL;
    
    maReSampleWithAverage(g_wav_read_data,g_wav_read_wave,ACE_DEFAULT,g_wav_read_rate_set);
    
    g_wav_read_size = g_wav_read_size - size;
    g_wav_read_data->data[0] = g_wav_read_data->data[0] + size;
    // printf("g_wav_read_size is %d\n",g_wav_read_size);
    
    return g_wav_read_wave;
}
*/

/////////////////////////////////////////////////////////
// 接口功能:
//  结束读取.wav文件
//
// 参数：
//  无
//
// 返回值：
//  无
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.4.6
/////////////////////////////////////////////////////////
void maFinishReadWAV()
{
    maReleaseWave(g_wav_read_data);
    if((int)g_wav_read_wave >= 0)
        maReleaseWave(g_wav_read_wave);
    
    g_wav_read_wave = NULL;
    g_wav_read_data = NULL;
    
    g_wav_read_size = 0;
}

static MAWave *g_wav_write_wave = NULL;
static FILE *g_wav_write_f = NULL;
static int g_wav_write_rate = 0;
static int g_wav_write_size = 0;

/////////////////////////////////////////////////////////
// 接口功能:
//  开始保存数据流到.wav文件
//
// 参数：
//  (I)file_name(NO) - .wav波形文件的路径
//  (I)sample_rate(44100) - 所保存.wav文件的采样率
//
// 返回值：
//  无
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.4.5
/////////////////////////////////////////////////////////
void maStartSaveWAV(char *file_name,int sample_rate)
{
    g_wav_write_f = fopen(file_name,"wb");
    maException((g_wav_write_f==NULL),"file cannot open",EXIT);
    
    fseek(g_wav_write_f,44,SEEK_SET);
    
    g_wav_write_wave = NULL;
    g_wav_write_rate = sample_rate;
    g_wav_write_size = 0;
}


/////////////////////////////////////////////////////////
// 接口功能:
//  保存数据到.wav文件
//
// 参数：
//  (I)wav_src(NO) - 写入.wav文件的波形
//
// 返回值：
//  无
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.4.5
/////////////////////////////////////////////////////////
void maSaveWAVStream(MAWave *wav_src)
{   
    int size;
    short *data;
    int i;
    
    maException((wav_src == NULL),"invalid input",EXIT);
    maException((wav_src->cn!=1),"only 1 channel will be write",NULL);
    maException((wav_src->info.wave_type != MA_WAVE_TD),"the write wave is not time domain wave",NULL);
    maException((g_wav_write_f == NULL), "save stream has not started",EXIT);
    
    if(g_wav_write_rate == MA_DEFAULT)
    {
        if(wav_src->info.frequency != 0)
            g_wav_write_rate = wav_src->info.frequency;
        else
            g_wav_write_rate = 44100;
    }
    size = (int)(((float)(wav_src->size * g_wav_write_rate))/((float)(wav_src->info.frequency)));
    
    if(g_wav_write_wave == NULL)
    {
        g_wav_write_wave = maCreateWave(size,1);
        g_wav_write_wave->info.frequency = g_wav_write_rate; 
    }
    else
        maRedefineWave(g_wav_write_wave,size,1);
    
    maReSampleWithAverage(wav_src,g_wav_write_wave,MA_DEFAULT,g_wav_write_rate);
    data = (short *)g_wav_write_wave->data[0];
    for(i=0;i<size;i++)
        data[i] = (short)(g_wav_write_wave->data[0][i]*32768.0);
    
    fwrite(data,sizeof(short),size,g_wav_write_f);
    
    g_wav_write_size = g_wav_write_size + size;
    // printf("g_wav_write_size is %d\n",g_wav_write_size);
}

/////////////////////////////////////////////////////////
// 接口功能:
//  结束写入.wav文件
//
// 参数：
//  无
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maFinishSaveWAV()
{
    maException((g_wav_write_f == NULL), "save stream has not started",EXIT);
    maSaveWAVHeader(g_wav_write_rate,1,g_wav_write_size,g_wav_write_f);
    
    if(g_wav_write_wave != NULL)
        maReleaseWave(g_wav_write_wave);
    
    fclose(g_wav_write_f);
    
    g_wav_write_f = NULL;
    g_wav_write_wave = NULL;
    g_wav_write_rate = 0;
    g_wav_write_size = 0;
}








