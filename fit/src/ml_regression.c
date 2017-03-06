#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"
#include "sample.h"

void maMLLinearRegression(MASample *DataIn,float *A)
{
	int sample_num,feature_num;
	int i,j,k;
	int n;
	float **feature;
	float *value;
	float *pf32data;
	
	maException(INVALID_SAMPLE(DataIn),"invalid input sample.",EXIT);
	
	sample_num = DataIn->sample_num;
	feature_num = DataIn->feature_num;
	
	maException((sample_num<=feature_num),"input sample data number is not enough.",EXIT);
	
	feature = DataIn->feature;
	value = DataIn->value;

	pf32data = (float *)malloc((feature_num+2)*(feature_num+1)*sizeof(float));
	
	memset(pf32data,0,(feature_num+2)*(feature_num+1));
	
	for(k=0;k<sample_num;k++)
	{
		n = 0;
		for(j=0;j<feature_num;j++)
		{
			for(i=0;i<feature_num;i++)
			{
				pf32data[n] = pf32data[n] + feature[k][i]*feature[k][j];
				n = n + 1;
			}
			pf32data[n] = pf32data[n] + feature[k][j];
			n = n + 1;
			pf32data[n] = pf32data[n] - value[k]*feature[k][j];
			n = n + 1;
		}
		for(i=0;i<feature_num;i++)
		{
			pf32data[n] = pf32data[n] + feature[k][i];
			n = n+1;
		}
		pf32data[n] = pf32data[n] + 1;
		n = n+1;
		pf32data[n] = pf32data[n] - value[k];
	}
	
	maMathLinearEquation(pf32data,A,feature_num+1);
	
	free(pf32data);
}

// MASample *maCreateSample(int sample_num,int feature_num);
// void maMLQuadraticRegression(MASample *DataIn,float *A)
// {
// 	int sample_num,feature_num;
// 	MASample *data;
// 	int i;
	
// 	maException(INVALID_SAMPLE(DataIn),"maMLLinearRegression","invalid input sample.",EXIT);
	
// 	sample_num = DataIn->sample_num;
// 	feature_num = DataIn->feature_num;
	
// 	data = maCreateSample(sample_num,(feature_num<<1));
	
	
	
	