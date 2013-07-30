#include<stdio.h>
#include "fmincg.h"

void costFunc(float* xVec,float* cost,float* gradVec)
{
	// f(x,y) = (x-10)^4 + (y-4)^2
	
	*cost = (xVec[0]-10)*(xVec[0]-10)*(xVec[0]-10)*(xVec[0]-10) + (xVec[1]-4)*(xVec[1]-4);

	gradVec[0] = 4*(xVec[0]-10)*(xVec[0]-10)*(xVec[0]-10);
	gradVec[1] = 2*(xVec[1]-4);
}

int main()
{
	float x[] = { 0,0 }; // initial guess
	
	int ret = fmincg(&costFunc, x, 2, 1000); // nDim = 2 , maxCostFunctionCalls = 1000
	
	printf("X-optim = (%f,%f). Return value = %d \n",x[0],x[1],ret);
	
	return 0;
}