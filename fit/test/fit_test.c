#include <stdio.h>
#include <stdlib.h>
#include "sample.h"

/*
int main()
{
	float x[15] = {0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5};
	float y[15] = { 7.668639053,
						30.67455621,
						73.46938776,
						122.6982249,
						195.4566877,						
						273.9595719,
						400,
						506.25,
						638.2271468,
						766.8639053,
						938.7052965,
						1121.471065,
						1296,
						1436.01108,
						1600};

	float a[6];
	
	printf("x: distance from camera\n");
	printf("y: (image area)/(ROI area)\n");
	
	maMLLinearFitLSM(x,y,15,a);
	printf("y = %f*x+%f\n",a[0],a[1]);
	
	maMLParabolaFitLSM(x,y,15,a);
	printf("y = %f*x2 + %f*x + %f\n",a[0],a[1],a[2]);
	
	maMLPolyFitLSM(x,y,15,a,3);
	printf("y = %f*x3 + %f*x2 + %f*x +%f\n",a[0],a[1],a[2],a[3]);
	
	maMLPolyFitLSM(x,y,15,a,4);
	printf("y = %f*x4 + %f*x3 + %f*x2 + %f*x +%f\n",a[0],a[1],a[2],a[3],a[4]);
	
	maMLPolyFitLSM(x,y,15,a,5);
	printf("y = %f*x5 + %f*x4 + %f*x3 + %f*x2 + %f*x +%f\n",a[0],a[1],a[2],a[3],a[4],a[5]);
	
	printf("\nx: distance from camera\n");
	printf("y: (ROI height)/(image height)\n");
	
	y[0] = 0.481481481;
	y[1] = 0.240740741;
	y[2] = 0.155555556;
	y[3] = 0.12037037;
	y[4] = 0.09537037;
	y[5] = 0.080555556;
	y[6] = 0.066666667;
	y[7] = 0.059259259;
	y[8] = 0.052777778;
	y[9] = 0.048148148;
	y[10] = 0.043518519;
	y[11] = 0.039814815;
	y[12] = 0.037037037;
	y[13] = 0.035185185;
	y[14] = 0.033333333;
	
	maMLInverseFitLSM_1(x,y,15,a);
	printf("y = %f*(1/x)+%f\n",a[0],a[1]);

	maMLInverseFitLSM_2(x,y,15,a);
	printf("y = 1/(%f*x+%f)\n",a[0],a[1]);
	
	printf("\nx: distance from camera\n");
	printf("y: (ROI area)/(image area)\n");
	y[0] = 0.080671779;
	y[1] = 0.021267361;
	y[2] = 0.009587674;
	y[3] = 0.005418596;
	y[4] = 0.003566744;
	y[5] = 0.002569927;
	y[6] = 0.001794464;
	y[7] = 0.001354649;
	y[8] = 0.00115789;
	y[9] = 0.000933642;
	y[10] = 0.000771605;
	y[11] = 0.000625;
	y[12] = 0.000525174;
	y[13] = 0.000463445;
	y[14] = 0.000434028;

	maMLInverseParabolaFitLSM_2(x,y,15,a);
	printf("y = 1.0/(%f*x2 + %f*x + %f)\n",a[0],a[1],a[2]);
}

*/

int main1()
{
	float x[20];
	float y[20];
	
	float a[10];
	
	int i;
	
	FILE *f;
	f = fopen("./data1.csv","r");
	
	for(i=0;i<20;i++)
		fscanf(f,"%f ,%f\n",x+i,y+i);
	
	maMLLinearFitLSM(x,y,20,a);
	printf("y = %f*x+%f\n",a[0],a[1]);
	
	maMLLinearFitSGD(x,y,20,a);
	printf("y = %f*x+%f\n",a[0],a[1]);
	
	// maMLLinearFit(x,y,20,a);
	// printf("y = %f*x+%f\n",a[0],a[1]);
	
	fclose(f);
}

int main2()
{
	float x[20];
	float y[20];
	
	float a[10];
	
	int i;
	
	FILE *f;
	f = fopen("./data2.csv","r");
	
	for(i=0;i<20;i++)
		fscanf(f,"%f ,%f\n",x+i,y+i);
	
	maMLParabolaFitLSM(x,y,20,a);
	printf("y = %f*x2+%f*x+%f\n",a[0],a[1],a[2]);
	
	maMLParabolaFitSGD(x,y,20,a);
	printf("y = %f*x2+%f*x+%f\n",a[0],a[1],a[2]);
	
	// maMLParabolaFit(x,y,20,a);
	// printf("y = %f*x2+%f*x+%f\n",a[0],a[1],a[2]);
	
	fclose(f);
}

int main3()
{
	float x[20];
	float y[20];
	
	float a[10];
	
	int i;
	
	FILE *f;
	f = fopen("./data2.csv","r");
	
	for(i=0;i<20;i++)
		fscanf(f,"%f ,%f\n",x+i,y+i);
	
	maMLPolyFitLSM(x,y,20,a,3);
	printf("y = %f*x3+%f*x2+%f*x+%f\n",a[0],a[1],a[2],a[3]);
	
	maMLPolyFit(x,y,20,a,3);
	printf("y = %f*x3+%f*x2+%f*x+%f\n",a[0],a[1],a[2],a[3]);
	
	fclose(f);
}

MASample *maCreateSample(int sample_num,int feature_num);

int main()
{
	int i;
	FILE *f;
	MASample *data;
	
	
	data = maCreateSample(80,2);
		
	f = fopen("./data3.csv","r");
	
	for(i=0;i<80;i++)		
		fscanf(f,"%f ,%f\n",&(data->feature[i][0]),&(data->feature[i][1]));
	
	maMLKmeanCluster(data,3);
	
	for(i=0;i<80;i++)
		printf("%f ,%f ,%d\n",data->feature[i][0],data->feature[i][1],data->label[i]);

	maReleaseSample(data);
	fclose(f);
}



int main5()
{
	int i;
	FILE *f;
	MASample *data;
	
	float a[10];
	
	data = maCreateSample(30,2);
	
	f = fopen("./data6.csv","r");
	
	for(i=0;i<30;i++)
	{
		fscanf(f,"%f,%f,%f\n",&(data->feature[i][0]),&(data->feature[i][1]),&(data->value[i]));
		
		// printf("%f,%f,%f\n",(data->feature[i][0]),(data->feature[i][1]),(data->value[i]));
	}
	
	maMLLinearRegression(data,a);
	printf("y = %f*x0+%f*x1+%f\n",a[0],a[1],a[2]);
	
	maReleaseSample(data);
}
	
	

	
	
	
	
	
	
	
	
	
	
	