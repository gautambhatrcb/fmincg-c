#include<stdio.h>
#include "fmincg.h"

void costFunc(float* xVec,float* cost,float* gradVec)
{
	// f(x,y) = (x-2)^2 + (y-4)^2
	
	*cost = (xVec[0]-2)*(xVec[0]-2) + (xVec[1]-4)*(xVec[1]-4);
	
	gradVec[0] = 2*(xVec[0]-2);
	gradVec[1] = 2*(xVec[1]-4);
}

int main()
{
	float x[] = { 2,3 };
	
	fmincg(&costFunc, x, 2, 100); // nDim = 2 , maxCostFunctionCalls = 100
	
	printf("X-optim = (%.3f,%.3f)\n",x[0],x[1]);
	
	return 0;
}