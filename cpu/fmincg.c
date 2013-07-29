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

int fmincg(void (*costFunc)(COST_FUNC_DATATYPE* inputVector, COST_FUNC_DATATYPE* cost, COST_FUNC_DATATYPE* gradVector), COST_FUNC_DATATYPE* xVector, int nDim, int maxCostCalls)
{
	int i,success = 0,costFuncCount=0;
	COST_FUNC_DATATYPE ls_failed,f1,d1,z1,f0,f2,d2,f3,d3,z3,limit,z2,A,B,C;
	COST_FUNC_DATATYPE df1[nDim],s[nDim],x0[nDim],df0[nDim],df2[nDim],tmp[nDim];
	COST_FUNC_DATATYPE* x = xVector;

	ls_failed = 0;
	
	if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
	(*costFunc)(xVector,&f1,df1);
	
	for(i=0;i<nDim;i++)
	{
		s[i] = -df1[i];
	}
	
	d1 = 0;
	for(i=0;i<nDim;i++)
	{
		d1 += -s[i]*s[i];
	}
	z1 = 1.0f / (1 - d1);
	
	while(1)
	{
		for(i=0;i<nDim;i++)
		{
			x0[i] = x[i];
			df0[i] = df1[i];
		}
		f0 = f1;
		
		for(i=0;i<nDim;i++)
		{
			x[i] = x[i] + (z1)*s[i];
		}
		
		if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
		(*costFunc)(x,&f2,df2);

		d2 = 0;
		for(i=0;i<nDim;i++)
		{
			d2 += df2[i]*s[i];
		}
		
		f3 = f1;
		d3 = d1;
		z3 = -z1;
		
		success = 0; 
		limit = -1;
		while(1)
		{
			while(( (f2) > ((f1) + RHO*(z1)*(d1))) || ( (d2) > -SIG*(d1) ))
			{
				limit = z1;
				if( (f2) > (f1) )
				{
					z2 = z3 - (0.5f*(d3)*(z3)*(z3))/((d3)*(z3)+(f2)-(f3));
				}
				else
				{
					A = 6*((f2)-(f3))/(z3)+3*((d2)+(d3));
					B = 3*((f3)-(f2))-(z3)*((d3)+2*(d2));
					z2 = (sqrt(B*B-A*(d2)*(z3)*(z3))-B)/A;
				}
				if(isnan(z2) || isinf(z2))
				{
					z2 = (z3) * 0.5f;
				}
				A = ((z2 < INT*(z3)) ? z2 : INT*(z3));
				B = (1-INT)*(z3);
				z2 = A > B ? A : B;
				z1 = z1 + z2;
				
				for(i=0;i<nDim;i++)
				{
					x[i] = x[i] + (z2)*s[i];
				}
				if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
				(*costFunc)(x,&f2,df2);

				d2 = 0;
				for(i=0;i<nDim;i++)
				{
					d2 += df2[i] * s[i];
				}
				z3 = z3 - z2;
			}
			if( (f2 > f1 + (z1)*RHO*(d1)) || ((d2) > -SIG*(d1)) )
			{
				break; //failure
			}
			if( d2 > SIG*(d1) )
			{
				success = 1; break; 
			}
			A = 6*(f2-f3)/z3+3*(d2+d3);
			B = 3*(f3-f2)-z3*(d3+2*d2);
			z2 = -d2*z3*z3/(B+sqrt(B*B-A*d2*z3*z3));
			if(!(B*B-A*d2*z3*z3 >= 0) || isnan(z2) || isinf(z2) || z2 < 0)
			{
				if(limit < -0.5f)
				{
					z2 = z1 * (EXT-1);
				}
				else
				{
					z2 = (limit-z1)/2;
				}
			}
			else if((limit > -0.5) && (z2+z1 > limit))
			{
				z2 = (limit-z1)/2; 
			}	
			else if((limit < -0.5) && (z2+z1 > z1*EXT))
			{
				z2 = z1*(EXT-1.0);
			}
			else if(z2 < -z3*INT)
			{
				z2 = -z3*INT;
			}
			else if((limit > -0.5) && (z2 < (limit-z1)*(1.0-INT)))
			{
				z2 = (limit-z1)*(1.0-INT);
			}
			f3 = f2; d3 = d2; z3 = -z2;
			z1 = z1 + z2;
			for(i=0;i<nDim;i++)
			{
				x[i] = x[i] + z2*s[i];
			}
			if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
			(*costFunc)(x,&f2,df2);
			d2 = 0;
			for(i=0;i<nDim;i++)
			{
				d2 += df2[i]*s[i];
			}
		}
		if(success)
		{
			f1 = f2;
			printf("Cost: %4.6f\n", f1);
			
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
			d2 = 0;
			for(i=0;i<nDim;i++)
			{
				d2 += df1[i] * s[i];
			}
			if(d2 > 0)
			{
				for(i=0;i<nDim;i++)
				{
					s[i] = -df1[i];
				}
				d2 = 0;
				for(i=0;i<nDim;i++)
				{
					d2 += -s[i]*s[i];
				}
			}
			A = d1/(d2-COST_FUNC_DATATYPE_MIN);
			z1 = z1 * ((RATIO < A) ? RATIO : A);
			d1 = d2;
			ls_failed = 0;
		}
		else
		{
			f1 = f0;
			for(i=0;i<nDim;i++)
			{
				x[i] = x0[i];
				df1[i] = df0[i];
			}
			if(ls_failed)
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
			d1 = 0;
			for(i=0;i<nDim;i++)
			{
				d1 += -s[i]*s[i];
			}
			z1 = 1/(1-d1);
			ls_failed = 1;
		}
	}
	return 2;
}