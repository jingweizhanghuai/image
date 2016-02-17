#include <stdio.h>

// 函数名称：求解多元一次方程组
// data：方程组系数矩阵
// answer：求得的未知数的结果
// num：未知数的个数
int imgLinearEquation(float *data,float *answer,int num)
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
