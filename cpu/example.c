#include<stdio.h>
#include "fmincg.h"

void costFunc(float* xVec,float* cost,float* gradVec)
{
	// f(x,y) = (x-10)^2 + (y-4)^2
	
	*cost = (xVec[0]-10)*(xVec[0]-10) + (xVec[1]-4)*(xVec[1]-4);
	
	//printf("x = (%f,%f) , C = %f\n",xVec[0],xVec[1],*cost);
	
	gradVec[0] = 2*(xVec[0]-10);
	gradVec[1] = 2*(xVec[1]-4);
}

int main()
{
	float x[] = { 0,0 };
	
	int ret = fmincg(&costFunc, x, 2, 100); // nDim = 2 , maxCostFunctionCalls = 100
	
	printf("X-optim = (%f,%f). Return value = %d \n",x[0],x[1],ret);
	
	return 0;
}