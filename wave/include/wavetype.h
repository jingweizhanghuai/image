#ifndef _WAVETYPE_H_
#define _WAVETYPE_H_

#define MA_DEFAULT -1

#define MA_WAVE_TD     0
#define MA_WAVE_FD     1
#define MA_WAVE_PS     2

#define MA_MAX_WAVE_CN 32

typedef struct MAWave {
    int size;                       // 波形数据大小（每个通道的数据点的个数）
    
    float *memory;
    float *data[MA_MAX_WAVE_CN];
    
    int cn;                         // 波形通道数

    struct
    {
        int wave_type;               // 波形类型
        int frequency;              // 波形的采样率
    }info;
}MAWave;

typedef struct MAWaveIndex {
    int frame_size;
    int frame_num;
    float frame_overlap;
    
    int *index;
}MAWaveIndex;

#endif
