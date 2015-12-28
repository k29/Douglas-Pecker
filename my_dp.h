#include <iostream>
#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>

#define MAX_POINTS 10001
#define TWICE_MAX_POINTS 20002

#define TRUE 1
#define FALSE 0

typedef double POINT[2];
typedef double HOMOG[3];

#define XX 0
#define YY 1
#define WW 2

#define CROSSPROD_2CCH(p, q, r) /* 2-d cartesian to homog cross product */\
 (r)[WW] = (p)[XX] * (q)[YY] - (p)[YY] * (q)[XX];\
 (r)[XX] = - (q)[YY] + (p)[YY];\
 (r)[YY] =   (q)[XX] - (p)[XX];

#define DOTPROD_2CH(p, q)	/* 2-d cartesian to homog dot product */\
 (q)[WW] + (p)[XX]*(q)[XX] + (p)[YY]*(q)[YY]


#define DOTPROD_2C(p, q)	/* 2-d cartesian  dot product */\
 (p)[XX]*(q)[XX] + (p)[YY]*(q)[YY]

#define LINCOMB_2C(a, p, b, q, r) /* 2-d cartesian linear combination */\
 (r)[XX] = (a) * (p)[XX] + (b) * (q)[XX];\
 (r)[YY] = (a) * (p)[YY] + (b) * (q)[YY];


// #define MIN(a,b) ( a < b ? a : b)
// #define MAX(a,b) ( a > b ? a : b)

#define OutputVertex(v) R[num_result++] = v;

POINT *V, *f_V,		/* V is the array of input points */
   **R;				/* R is the array of output pointers to V */

 int n,			/* number of elements in V */
 num_result,			/* number of elements in R */
 fv_n,  /*number of elements post outlier eliminiation*/
 outFlag;

double m_a, m_b, m_coeff;
double ry1, ry2;
double ry3, ry4;
 IplImage* image;
