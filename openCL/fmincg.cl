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

[ Sagar GV, 2013, sagar.writeme@gmail.com ] Changes Made:
- Ported to C
- Changed structure to make it usable as an OpenCL kernel
*/

#define COST_FUNC_DATATYPE float
#define COST_FUNC_DATATYPE_MIN (FLT_MIN*100)

#define RHO 0.01f
#define SIG 0.5f
#define INT 0.1f
#define EXT 3.0f
#define MAX 20
#define RATIO 100.0f

#define nDim 2

// iState should be able to hold 3 integers
// state should be able to hold (15+6*nDim) COST_FUNC_DATATYPEs
__kernel void fmincg(__global  COST_FUNC_DATATYPE* xVector,__global  COST_FUNC_DATATYPE* cost,__global  COST_FUNC_DATATYPE* gradVector,__global int* iState,__global COST_FUNC_DATATYPE* state)
{
	int i;
	__global int *st=&iState[0],*success=&iState[1],*lineSearchFuncCount=&iState[2];
	__global COST_FUNC_DATATYPE *ls_failed=&state[0],*f1=&state[1],*d1=&state[2],*z1=&state[3],*f0=&state[3],*f2=&state[4],*d2=&state[5],*f3=&state[6],*d3=&state[7],*z3=&state[8],*limit=&state[9],*z2=&state[10];
	COST_FUNC_DATATYPE A,B,C;
	__global COST_FUNC_DATATYPE *df1=&state[15],*s=&state[nDim+15],*x0=&state[2*nDim+15],*df0=&state[3*nDim+15],*df2=&state[4*nDim+15],*tmp=&state[5*nDim+15];
	__global COST_FUNC_DATATYPE *x = xVector;
	
	switch(*st)
	{
		case 0:	goto INIT;
		case 1:	goto EV1;
		case 2: goto EV2;
		case 3: goto EV3;
		case 4: goto EV4;
		default:return;
	}
INIT:
	*ls_failed = 0;

	//(*costFunc)(xVector,&f1,df1);
	*st = 1;return;
EV1	:
	*f1 = *cost;
	for(i=0;i<nDim;i++)
		df1[i] = gradVector[i];
	
	for(i=0;i<nDim;i++)
	{
		s[i] = -df1[i];
	}
	
	*d1 = 0;
	for(i=0;i<nDim;i++)
	{
		*d1 += -s[i]*s[i];
	}
	*z1 = 1.0f / (1 - *d1);
	
	while(1)
	{
		for(i=0;i<nDim;i++)
		{
			x0[i] = x[i];
			df0[i] = df1[i];
		}
		*f0 = *f1;
		
		for(i=0;i<nDim;i++)
		{
			x[i] = x[i] + (*z1)*s[i];
		}
		
		//(*costFunc)(x,&f2,df2);
		*st = 2; return;
EV2:	
		*f2 = *cost;
		for(i=0;i<nDim;i++)
			df2[i] = gradVector[i];

		*d2 = 0;
		for(i=0;i<nDim;i++)
		{
			*d2 += df2[i]*s[i];
		}
		
		*f3 = *f1;
		*d3 = *d1;
		*z3 = -(*z1);
		
		*success = 0; 
		*limit = -1;
		*lineSearchFuncCount = 0;
		// begin line search
		while(1)
		{
			while((( (*f2) > ((*f1) + RHO*(*z1)*(*d1))) || ( (*d2) > -SIG*(*d1) )) && *lineSearchFuncCount < MAX)
			{
				*limit = *z1;
				if( (*f2) > (*f1) )
				{
					*z2 = *z3 - (0.5f*(*d3)*(*z3)*(*z3))/((*d3)*(*z3)+(*f2)-(*f3));
				}
				else
				{
					A = 6*((*f2)-(*f3))/(*z3)+3*((*d2)+(*d3));
					B = 3*((*f3)-(*f2))-(*z3)*((*d3)+2*(*d2));
					*z2 = (sqrt(B*B-A*(*d2)*(*z3)*(*z3))-B)/A;
				}
				if(isnan(*z2) || isinf(*z2))
				{
					*z2 = (*z3) * 0.5f;
				}
				A = ((*z2 < INT*(*z3)) ? *z2 : INT*(*z3));
				B = (1-INT)*(*z3);
				*z2 = A > B ? A : B;
				*z1 = *z1 + *z2;
				
				for(i=0;i<nDim;i++)
				{
					x[i] = x[i] + (*z2)*s[i];
				}

				*lineSearchFuncCount += 1;
				
				//(*costFunc)(x,&f2,df2);
				*st = 3; return;
EV3:			
				*f2 = *cost;
				for(i=0;i<nDim;i++)
					df2[i] = gradVector[i];

				*d2 = 0;
				for(i=0;i<nDim;i++)
				{
					*d2 += df2[i] * s[i];
				}
				*z3 = *z3 - *z2;
			}
			if( ((*f2) > (*f1) + (*z1)*RHO*(*d1)) || ((*d2) > -SIG*(*d1)) || *lineSearchFuncCount >= MAX)
			{
				break; //failure
			}
			if( (*d2) > SIG*(*d1) )
			{
				*success = 1; break; 
			}
			A = 6*((*f2)-(*f3))/(*z3)+3*((*d2)+(*d3));
			B = 3*((*f3)-(*f2))-(*z3)*((*d3)+2*(*d2));
			*z2 = -(*d2)*(*z3)*(*z3)/(B+sqrt(B*B-A*(*d2)*(*z3)*(*z3)));
			if(!(B*B-A*(*d2)*(*z3)*(*z3) >= 0) || isnan(*z2) || isinf(*z2) || *z2 < 0)
			{
				if(*limit < -0.5f)
				{
					*z2 = *z1 * (EXT-1);
				}
				else
				{
					*z2 = (*limit-*z1)/2;
				}
			}
			else if((*limit > -0.5) && ((*z2)+(*z1) > *limit))
			{
				*z2 = (*limit-*z1)/2; 
			}	
			else if((*limit < -0.5) && ((*z2)+(*z1) > (*z1)*EXT))
			{
				*z2 = *z1*(EXT-1.0);
			}
			else if(*z2 < -(*z3)*INT)
			{
				*z2 = -(*z3)*INT;
			}
			else if((*limit > -0.5) && (*z2 < (*limit-*z1)*(1.0-INT)))
			{
				*z2 = (*limit-*z1)*(1.0-INT);
			}
			*f3 = *f2; *d3 = *d2; *z3 = -(*z2);
			*z1 = *z1 + *z2;
			for(i=0;i<nDim;i++)
			{
				x[i] = x[i] + (*z2)*s[i];
			}
			//(*costFunc)(x,&f2,df2);
			*st = 4;return;
EV4:		
			*f2 = *cost;
			for(i=0;i<nDim;i++)
				df2[i] = gradVector[i];
			
			*d2 = 0;
			for(i=0;i<nDim;i++)
			{
				*d2 += df2[i]*s[i];
			}
		}
		// line search ended
		if(*success)
		{
			*f1 = *f2;
			//printf("Cost: %e\n", *f1);
			
			A = 0;
			B = 0;
			C = 0;
			for(i=0;i<nDim;i++)
			{
				A += df1[i]*df1[i];
				B += df2[i]*df2[i];
				C += df1[i]*df2[i];
			}
			for(i=0;i<nDim;i++)
			{
				s[i] = ((B-C)/A)*s[i] - df2[i];
			}
			for(i=0;i<nDim;i++)
			{
				tmp[i] = df1[i]; df1[i] = df2[i]; df2[i] = tmp[i];
			}
			*d2 = 0;
			for(i=0;i<nDim;i++)
			{
				*d2 += df1[i] * s[i];
			}
			if(*d2 > 0)
			{
				for(i=0;i<nDim;i++)
				{
					s[i] = -df1[i];
				}
				*d2 = 0;
				for(i=0;i<nDim;i++)
				{
					*d2 += -s[i]*s[i];
				}
			}
			A = (*d1)/((*d2)-COST_FUNC_DATATYPE_MIN);
			*z1 = (*z1) * ((RATIO < A) ? RATIO : A);
			*d1 = *d2;
			*ls_failed = 0;
		}
		else
		{
			*f1 = *f0;
			for(i=0;i<nDim;i++)
			{
				x[i] = x0[i];
				df1[i] = df0[i];
			}
			if(*ls_failed)
			{
				break;
			}
			for(i=0;i<nDim;i++)
			{
				tmp[i] = df1[i]; df1[i] = df2[i]; df2[i] = tmp[i];
			}
			for(i=0;i<nDim;i++)
			{
				s[i] = -df1[i]; 
			}
			*d1 = 0;
			for(i=0;i<nDim;i++)
			{
				*d1 += -s[i]*s[i];
			}
			*z1 = 1/(1-(*d1));
			*ls_failed = 1;
		}
	}
}