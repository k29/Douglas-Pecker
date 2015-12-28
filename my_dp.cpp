#include "my_dp.h"

using namespace cv;
using namespace std;

int stack[MAX_POINTS];		/* recursion stack */
int sp;				/* recursion stack pointer */


double EPSILON = 100;
double EPSILON_SQ = EPSILON*EPSILON;

#define Stack_Push(e)		/* push element onto stack */\
stack[++sp] = e
#define Stack_Pop()		/* pop element from stack (zero if none) */\
stack[sp--]
#define Stack_Top()		/* top element on stack  */\
stack[sp]
#define Stack_EmptyQ()		/* Is stack empty? */\
(sp < 0)
#define Stack_Init()		/* initialize stack */\
sp = -1

void Output(int i, int j)
{
  if (outFlag)
  {
    outFlag = FALSE;
    OutputVertex(V+i);
  }
  OutputVertex(V+j);
}

void Find_Split(int i, int j, int *split, double *dist)
{
  int k;
  HOMOG q;
  double tmp;

  *dist = -1;
  if (i + 1 < j)
  {
    CROSSPROD_2CCH(V[i], V[j], q);
    for (k = i + 1; k < j; k++)
    {
      tmp = DOTPROD_2CH(V[k], q);
      if (tmp < 0) tmp = - tmp;
      if (tmp > *dist)
      {
        *dist = tmp;
        *split = k;
      }
    }
    *dist *= *dist/(q[XX]*q[XX] + q[YY]*q[YY]);
  }
}


void DPbasic(int i, int j)
{
  int split;
  double dist_sq;

  Stack_Init();
  Stack_Push(j);
  do
  {
    Find_Split(i, Stack_Top(), &split, &dist_sq);
    if (dist_sq > EPSILON_SQ)
    {
      Stack_Push(split);
    }
    else
    {
      Output(i, Stack_Top());
      i = Stack_Pop();
    }
  }
  while (!Stack_EmptyQ());
}

//callback event for the mouse click
void callBack(int event, int x, int y, int flags, void* userdata)
{
  if  ( event == EVENT_LBUTTONDOWN )
  {
    V[n][XX] = x;
    V[n][YY] = y;
    cvCircle(image, cvPoint(((V[n]))[XX],((V[n]))[YY]), 2, cvScalar(255,0,0));
    cvShowImage("Test", image);
    n++;
  }
}

void Render_Points_Original()
{
  //points clicked by the mouse
  for(int i=0;i<n-1;i++)
  cvLine(image, cvPoint(((V[i]))[XX],((V[i]))[YY]), cvPoint(((V[i+1]))[XX],((V[i+1]))[YY]), cvScalar(0,0,255));

  cvShowImage("Test", image);
}

//declare the image, and the mosue callback event
void Render_Init()
{
  image = cvCreateImage(cvSize(1000,1000), 8, 3); //8 is the depth, 3 is channel
  cvNamedWindow("Test");
  cvSet(image, CV_RGB(0,0,0));
  cvSetMouseCallback("Test", callBack, (void*) image);
  cvShowImage("Test", image);
  cvWaitKey(0);
  Render_Points_Original(); //render those points
  cvWaitKey(0);
}

//draw the points and the lines between them


void Print_Points_original(POINT *P, int n)
{
  printf("\n%d Original Points\n", n);
  for(int i=0; i<n; i++)
  {
    printf("%d: %.4f %.4f\n", i, ((P[i]))[XX], ((P[i]))[YY]);
  }
}


void Print_Points_DP(POINT **P, int n)
{
  printf("\n%d Post DP Points\n", n);
  for(int i=0; i<n; i++)
  {
    printf("%d: %.4f %.4f\n", i, (*(P[i]))[XX], (*(P[i]))[YY]);
  }
}

void Print_Points_outlier(POINT *P, int n)
{
  printf("\n%d Points after eliminiation of outliers\n", n);
  for(int i=0; i<n; i++)
  {
    printf("%d: %.4f %.4f\n", i, ((P[i]))[XX], ((P[i]))[YY]);
  }
}

void Render_Points_DP()
{
  //points clicked by the mouse
  for(int i=0;i<num_result;i++)
  cvCircle(image, cvPoint((*(R[i]))[XX],(*(R[i]))[YY]), 2, cvScalar(255,0,0));

  for(int i=0;i<num_result-1;i++)
  cvLine(image, cvPoint((*(R[i]))[XX],(*(R[i]))[YY]), cvPoint((*(R[i+1]))[XX],(*(R[i+1]))[YY]), cvScalar(0,255,0));
  cvShowImage("Test", image);
}


void Init()
{
  n=0;
  outFlag = TRUE;
  num_result = 0;
  V = (POINT*) calloc(MAX_POINTS, sizeof(POINT));
  f_V = (POINT*) calloc(MAX_POINTS, sizeof(POINT));
  R = (POINT**) calloc(MAX_POINTS, sizeof(POINT *));
}


void linear_regression(POINT *P, int n)
{
  //calculates the averages of x and y
  double xa=0, ya=0;
  for(int i=0;i<n;i++) //n=parameter n
  {
    xa += (P[i])[XX];
    ya += (P[i])[YY];
  }
  xa /= n;
  ya /= n;

  //calculate the auxiliary sums
  double xx =0, yy=0, xy=0;
  for(int i=0;i<n; i++)
  {
    double tmpx = (P[i])[XX] - xa;
    double tmpy = (P[i])[YY] - ya;
    xx += tmpx * tmpx;
    yy += tmpy * tmpy;
    xy += tmpx * tmpy;
  }
  assert(fabs(xx) != 0);


  //calculate regression line parameters
  m_b = xy / xx; //slope
  m_a = ya - m_b * xa; //y intercept
  m_coeff = (fabs(yy) == 0) ? 1 : xy / sqrt(xx * yy); //regression cpefficient

}

void render_regression()
{
   //calculated for the first and last x
  ry1 = m_b*(((V[0]))[XX]) + m_a;
  ry2 = m_b*(((V[n-1]))[XX]) + m_a;

//  printf("\nThe coordinates for the regression line are: %.4f %.4f \t %.4f %.4f\n", ((V[0]))[XX], ry1, ((V[n-1]))[XX], ry2 );

  cvLine(image, cvPoint((((V[0]))[XX]),ry1), cvPoint((((V[n-1]))[XX]), ry2), cvScalar(200,0,200));
  cvShowImage("Test", image);
  // cvWaitKey(0);

}

void render_regression_postoutlier()
{
   //calculated for the first and last x
  ry3 = m_b*(((f_V[0]))[XX]) + m_a;
  ry4 = m_b*(((f_V[fv_n-1]))[XX]) + m_a;

//  printf("\nThe coordinates for the regression line are: %.4f %.4f \t %.4f %.4f\n", ((V[0]))[XX], ry1, ((V[n-1]))[XX], ry2 );

  cvLine(image, cvPoint((((f_V[0]))[XX]),ry3), cvPoint((((f_V[fv_n-1]))[XX]), ry4), cvScalar(100,100,0), 2);
  cvShowImage("Test", image);
}

void find_outlier(POINT *P, int n)
{
  double dist[n], dist_av, dist_var, std_dev;
  //Find the distance of all the points from tehe regression line
  for(int i=0;i<n;i++)
  {
    dist[i] = (((P[i])[YY]) - m_b*((P[i])[XX]) + m_a)/(sqrt(1 + m_b*m_b));                       //(y1 - m*x1 + c)/ sqrt(1 + m*m)
    printf("The distance from the line for point %d is: %.4f\n",i, dist[i] );
    dist_av += dist[i];
  }

  dist_av /= n;
  printf("\nThe average distance from the line is : %.4f\n", dist_av);

  //calculating variance
  for(int i =0;i<n; i++)
  {
    dist_var += pow((fabs(dist[i]-dist_av)),2);
  }
  dist_var/=n;
  printf("\nThe variance is: %.4f\n",dist_var );
  std_dev = sqrt(dist_var);
  printf("\nThe std. dev is: %.4f\n",std_dev );

  // if the dist is inside the standard deviation then it is normal else not!!
  int j=0;
  for(int i=0;i<n;i++)
  {
    if(fabs(dist[i] - dist_av) < std_dev)
    {
      printf("\nThe point calculated in the final calculation is: %d",i);
      (f_V[j])[XX] = (P[i])[XX];
      (f_V[j++])[YY] = (P[i])[YY];
    }
  }
  fv_n = j;
  // render_regression_postoutlier();

}

int main(int argc, char const *argv[])
{
  Init(); //allocate space, and number of points present to 0
  Render_Init(); //declare the image and mouse callback event
  Print_Points_original(V, n); //print those points

  DPbasic(0,n-1);
  Print_Points_DP(R,num_result); //print post DP points
  Render_Points_DP();
  linear_regression(V, n);
  render_regression();
  find_outlier(V, n);
  //Print_Points_outlier(f_V, fv_n);
  linear_regression(f_V, fv_n);
  render_regression_postoutlier();
  cvWaitKey(0);
  return 0;
}
