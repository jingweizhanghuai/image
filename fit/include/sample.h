#ifndef _MA_SAMPLE_H_
#define _MA_SAMPLE_H_

#define LABEL_VALID 0
#define VALUE_VALID 1

typedef struct MASample
{
	float **feature;
	union 
	{
		int *label;
		float *value;
	};
	int sample_num;
	int feature_num;
}MASample;

#define INVALID_SAMPLE(sample) ((sample->feature == NULL)||(sample->label == NULL)||(sample->sample_num == 0)||(sample->feature_num == 0))

// typedef struct IMP_Feature_S
// {
	// float *feature;
	// int feature_num;
// }IMP_Feature_S;

#endif