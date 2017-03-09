#include <stdio.h>



void MinDistanceLocate(float **feature,int sample_num,int dim_num,float *w)
{
	int vector_num;
	float *k1,*k2;
	float *pk1;
	
	#define FEATURE(x,y) feature[x][y]
	
	k1 = malloc(dim_num*(dim_num+1)*sizeof(float));
	#define K1(x,y)	k1[(x)*(dim_num+1)+(y)]
	
	vector_num = dim_num - sample_num;
	if(vector_num !=0)
	{
		k2 = malloc(sample_num*(sample_num+1)*sizeof(float));
		#define K2(x,y)	k2[(x)*(dim_num+1)+(y)]
	}
	
	for(k=0;k<sample_num;k++)
		memcpy(k1+k*(feature_num+1),feature[k],(feature_num+1)*sizeof(float));
	
	for(k=0;k<vector_num;k++)
	{
		v1 = V1 + vector_num;
		
		// K2(0,0) = FEATURE[0][1];
		// K2(0,1) = FEATURE[0][2];
		// K2(0,2) = FEATURE[0][3]+FEATURE[0][k];
		// K2(1,0) = FEATURE[1][1];
		// K2(1,1) = FEATURE[1][2];
		// K2(1,2) = FEATURE[1][3];
		
		// maMathLinearEquation(k2,v1,sample_num);
		
		for(i=0;i<sample_num;i++)
		{
			for(j=vector_num;j<dim_num;j++)
			{				
				K2(i,j) = FEATURE[i][j];
			}
			K2(i,j) = FEATURE[i][j];
		}
		maMathLinearEquation(k2,v1,sample_num);
		
		v2 = V2 + vector_num;
		for(i=0;i<sample_num;i++)
		{
			for(j=vector_num;j<dim_num;j++)
			{				
				K2(i,j) = FEATURE[i][j];
			}
			K2(i,j) = FEATURE[i][j] + FEATURE[i][k];
		}
		maMathLinearEquation(k2,v2,sample_num);
		
		K1(k+sample_num,k) = 1.0;		
		for(j=vector_num;j<dim_num;j++)
			K1(k+sample_num,j) = v2[j]-v1[j];
	}
	
	maMathLinearEquation(k1,w,sample_num);
}

	
// sample *caculate(sample *data,float *w)
// {
	// int sample_num;
	// int dim_num;
	
	// sample_num = data->sample_num;
	// dim_num = data->dim_num;
	
	
	