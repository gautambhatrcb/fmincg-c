/*
Copyright (C) 2001 and 2002 by Carl Edward Rasmussen. Date 2002-02-13
%
%
% (C) Copyright 1999, 2000 & 2001, Carl Edward Rasmussen
% 
% Permission is granted for anyone to copy, use, or modify these
% programs and accompanying documents for purposes of research or
% education, provided this copyright notice is retained, and note is
% made of any changes that have been made.
% 
% These programs and documents are distributed without any warranty,
% express or implied.  As the programs were written for research
% purposes only, they have not been tested to the degree that would be
% advisable in any important application.  All use of these programs is
% entirely at the user's own risk.

[ s-gv, 2013, sagar.writeme@gmail.com ] Changes Made:
- Ported to C

*/
#include "fmincg.h"

void fmincg(void (*costFunc)(COST_FUNC_DATATYPE* inputVector, COST_FUNC_DATATYPE* cost, COST_FUNC_DATATYPE* gradVector), COST_FUNC_DATATYPE* xVector, int nDim, int maxCostCalls)
{
	int i;
	for(i = 0;i < nDim; i++)
		xVector[i] = 0;
}