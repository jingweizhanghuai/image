#include <stdio.h>

#define maMathABS(x) (((x)>0)?(x):(-(x)))
#define maMathABSDiff(x,y) (((x)>(y))?((x)-(y)):((y)-(x)))

#define maMathFLoatABS(pin,pout) {*((int *)(pout)) =(*((int *)(pin)))&0x7FFFFFFF;}
#define maMathIntABS(pin,pout) {*pout = maMathABS(*pin);}

float maMathSqrt(float x)
{
	float y;
	int *py; 
	float diff;
	float error;
	
	y = 1.0/x;
	py = (int *)&y;	
	*py = 1597463007-((*py)>>1);
	
	diff = y*y-x;
	maMathFLoatABS(&diff,&error);
	
	while(error>0.01)
	{
		y = (y+x/y)/2.0;
		y = (y+x/y)/2.0;
		
		diff = y*y-x;
		maMathFLoatABS(&diff,&error);
	}
	
	return y;
}

float SinArray[91]={0.00000000000000,
						0.01745240643728,
						0.03489949670250, 
						0.05233595624294, 
						0.06975647374413, 
						0.08715574274766, 
						0.10452846326765, 
						0.12186934340515, 
						0.13917310096007, 
						0.15643446504023, 
						0.17364817766693, 
						0.19080899537655, 
						0.20791169081776, 
						0.22495105434387, 
						0.24192189559967, 
						0.25881904510252, 
						0.27563735581700, 
						0.29237170472274, 
						0.30901699437495, 
						0.32556815445716, 
						0.34202014332567, 
						0.35836794954530, 
						0.37460659341591, 
						0.39073112848927, 
						0.40673664307580, 
						0.42261826174070, 
						0.43837114678908, 
						0.45399049973955, 
						0.46947156278589, 
						0.48480962024634, 
						0.50000000000000, 
						0.51503807491005, 
						0.52991926423321, 
						0.54463903501503, 
						0.55919290347075, 
						0.57357643635105, 
						0.58778525229247, 
						0.60181502315205, 
						0.61566147532566, 
						0.62932039104984, 
						0.64278760968654, 
						0.65605902899051, 
						0.66913060635886, 
						0.68199836006250, 
						0.69465837045900, 
						0.70710678118655, 
						0.71933980033865, 
						0.73135370161917,
						0.74314482547739, 
						0.75470958022277, 
						0.76604444311898, 
						0.77714596145697, 
						0.78801075360672, 
						0.79863551004729, 
						0.80901699437495, 
						0.81915204428899, 
						0.82903757255504, 
						0.83867056794542, 
						0.84804809615643, 
						0.85716730070211, 
						0.86602540378444, 
						0.87461970713940, 
						0.88294759285893, 
						0.89100652418837, 
						0.89879404629917, 
						0.90630778703665, 
						0.91354545764260, 
						0.92050485345244, 
						0.92718385456679, 
						0.93358042649720, 
						0.93969262078591, 
						0.94551857559932, 
						0.95105651629515, 
						0.95630475596304, 
						0.96126169593832, 
						0.96592582628907, 
						0.97029572627600, 
						0.97437006478524, 
						0.97814760073381, 
						0.98162718344766, 
						0.98480775301221, 
						0.98768834059514, 
						0.99026806874157,
						0.99254615164132, 
						0.99452189536827, 
						0.99619469809175,
						0.99756405025982, 
						0.99862953475457, 
						0.99939082701910, 
						0.99984769515639,
						1.0};	

float maMathSin(int x)
{
	if(x>=360)
		x = x%360;
	
	if((x>=0)&&(x<90))
		return SinArray[x];
	else if(x<180)
		return SinArray[180-x];
	else if(x<270)
		return (0-SinArray[x-180]);
	else
		return (0-SinArray[360-x]);
}

float maMathFloatSin(float x)
{
	int x0,x1;
	float w0,w1;
	
	x0 = (int)x;
	x1 = x0+1;
	w1 = x-(float)x0;
	w0 = 1-w1;
	
	return (w0*maMathSin(x0)+w1*maMathSin(x1));
}

float maMathCos(int x)
{
	if(x>=360)
		x = x%360;
	
	if((x>=0)&&(x<90))
		return SinArray[90-x];
	else if(x<180)
		return (0-SinArray[x-90]);
	else if(x<270)
		return (0-SinArray[270-x]);
	else
		return SinArray[x-270];
}

float maMathFloatCos(float x)
{
	int x0,x1;
	float w0,w1;
	
	x0 = (int)x; 
	x1 = x0+1;
	w1 = x-(float)x0;
	w0 = 1-w1;
	
	return (w0*maMathCos(x0)+w1*maMathCos(x1));
}

int maMathLinearEquation(float *data,float *answer,int num)
{
	int i,j,k;
	
	float buff;
	
	#define DATA(y,x) (*(data+(y)*(num+1)+(x)))	
	
	// 高斯消元
	for(k=0;k<num;k++)
	{
		if(DATA(k,k)==0)
		{
			for(j=k+1;j<num;j++)
				if(DATA(j,k)!=0)
				{
					for(i=k;i<num+1;i++)
					{
						buff = DATA(k,i);
						DATA(k,i) = DATA(j,i);
						DATA(j,i) = buff;
					}
						
					break;
				}
				
			// 如果无解则返回0
			if(j==num)
				return 0;
		}
		
		for(i=k+1;i<num+1;i++)		
			DATA(k,i) = DATA(k,i)/DATA(k,k);
			
		DATA(k,k) = 1;
				
		for(j=k+1;j<num;j++)
			if(DATA(j,k)!=0)
			{
				for(i=k+1;i<num+1;i++)
					DATA(j,i) = DATA(j,i)/DATA(j,k)-DATA(k,i);
				
				DATA(j,k)=0;
			}
	}
	
	// 答案求解
	for(j=num-1;j>=0;j--)
	{
		answer[j] = 0-DATA(j,num);
		for(i=num-1;i>j;i--)
			answer[j] = answer[j] - DATA(j,i)*answer[i];
	}
	
	return 1;		
}

