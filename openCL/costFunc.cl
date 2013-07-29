#define COST_FUNC_DATATYPE float

__kernel void costFunc(__global  COST_FUNC_DATATYPE* xVector,__global  COST_FUNC_DATATYPE* cost,__global  COST_FUNC_DATATYPE* gradVector)
{
	// f(x,y) = (x-10)^4 + (y-2)^2
	
	*cost = (xVector[0] - 10)*(xVector[0] - 10)*(xVector[0] - 10)*(xVector[0] - 10) + (xVector[1] - 2)*(xVector[1] - 2);
	
	gradVector[0] = 4*(xVector[0] - 10)*(xVector[0] - 10)*(xVector[0] - 10);
	gradVector[1] = 2*(xVector[1] - 2);
}