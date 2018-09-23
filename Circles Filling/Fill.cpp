#include <windows.h>
#include "Fill.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

void fill();

double roundVal(double x)
{
  int ix;
  if (x >= 0) 
    ix = (int)(x + 0.5);
  else
    ix = (int)(x - 0.5);
  return (double)(ix);
}

void swap(int &x, int &y)
{ 
  int tmp;
  tmp = x;
  x = y;
  y = tmp;
}

void setupMenus(HWND hwnd)
{
  HMENU hmenu, hsubmenu;
  hmenu=GetMenu(hwnd);
  hsubmenu=GetSubMenu(hmenu, 0);

  switch (gDrawData.drawMode)
  {
    case READY_MODE :
      // enable 'Draw Circle', disable 'Filling' menu
      EnableMenuItem(hsubmenu,ID_CIRCLES,
                     MF_BYCOMMAND|MF_ENABLED);
      EnableMenuItem(hsubmenu,ID_FILL,
                     MF_BYCOMMAND|MF_GRAYED);
      break;
    case DRAW_MODE :
    case FILL_MODE :
    case FILLED_MODE :
      // disable 'Draw Circles', 'Filling' menu 
      EnableMenuItem(hsubmenu,ID_CIRCLES,
                     MF_BYCOMMAND|MF_GRAYED);
      EnableMenuItem(hsubmenu,ID_FILL,
                     MF_BYCOMMAND|MF_GRAYED);
      break;
    case DRAWN_MODE :
      // enable 'Filling' menus, disable 'Draw Circles' menu
      EnableMenuItem(hsubmenu,ID_CIRCLES,
                     MF_BYCOMMAND|MF_GRAYED);
      EnableMenuItem(hsubmenu,ID_FILL,
                     MF_BYCOMMAND|MF_ENABLED);
      break;
  }
}

void performFilling(HWND hwnd)
{
  setDrawMode(FILL_MODE, hwnd);
  SelectObject(gDrawData.hdcMem, gDrawData.hFillPen);
  fill();
  reDraw(hwnd);
  setDrawMode(FILLED_MODE, hwnd);
}

void processCommand(int cmd, HWND hwnd)
{
  switch(cmd)
  {
    case ID_FILL:
      performFilling(hwnd);
      break;
    default:
      processCommonCommand(cmd, hwnd);
      break;
  }
}

/* function which checks if a point lies inside,outside or on the circle1 */
bool circle(int x,int y ,int i){
     int cx = gDrawData.centres[i].x;
     int cy = gDrawData.centres[i].y;
     int r = gDrawData.radius[i];
     if( (x-cx)*(x-cx) + (y-cy)*(y-cy) - (r)*(r) <=0 )
      return true;
     else
      return false; 
     }

void fill()
{
int x=0;
 int y,radius;
 /* cyi are the y coordinates of the center of the circles and cxi are the x coordinates.*/
 int cy1 = gDrawData.centres[0].y;
 int cy2 = gDrawData.centres[1].y;
 int cy3 = gDrawData.centres[2].y;
 int cx1 = gDrawData.centres[0].x;
 int cx2 = gDrawData.centres[1].x;
 int cx3 = gDrawData.centres[2].x;
 /* ymin and ymax are the y values for which scan lines need to be processed*/
 int ymin = min( cy1 - gDrawData.radius[0] , min(cy2 - gDrawData.radius[1],cy3 - gDrawData.radius[2])); 
 int ymax = max( cy1 + gDrawData.radius[0] , max(cy2 +gDrawData.radius[1],cy3 +gDrawData.radius[2]));
/* MoveToEx (gDrawData.hdcMem, 10, ymin, NULL);
LineTo (gDrawData.hdcMem, 1000+1, ymin);
gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_XYZ);
SelectObject(gDrawData.hdcMem, gDrawData.hFillPen);
MoveToEx (gDrawData.hdcMem, 10, ymax, NULL);
LineTo (gDrawData.hdcMem, 1000+1, ymax);
*/
int scan = ymin;
int r11 = gDrawData.radius[0] * gDrawData.radius[0];
int r22 = gDrawData.radius[1] * gDrawData.radius[1];
int r33 = gDrawData.radius[2] * gDrawData.radius[2];
int x1=-100000,x2=-100000,x3=-100000; 
int f1 =0,f2=0,f3=0;
/* scanning the lines here and checking the intersections*/
while(scan <= ymax){     
  if ((r11 - (scan - cy1)*(scan -cy1))>=0){
    x1 = sqrt(r11 - (scan - cy1)*(scan -cy1));
    f1=1;
  }
  if ((r22 - (scan - cy2)*(scan -cy2))>=0){
     x2 = sqrt(r22 - (scan - cy2)*(scan -cy2));
     f2=1;
  }
  if ((r33 - (scan - cy3)*(scan -cy3))>=0){
    x3 = sqrt(r33 - (scan - cy3)*(scan -cy3));
    f3=1;
  }
  /*case1: only circle1 has intersections with the scan line*/     
  if ( f1+f2+f3==1){
    int x_left,x_right;
    if(f1){
      x_left =  min( x1 + cx1 , -x1 + cx1);
      x_right =  max( x1 + cx1 , -x1 + cx1);
    }else if(f2){
      x_left =  min( x2 + cx2 , -x2 + cx2);
      x_right =  max( x2 + cx2 , -x2 + cx2);
    }else{
      x_left =  min( x3 + cx3 , -x3 + cx3);
      x_right =  max( x3 + cx3 , -x3 + cx3);
    } 
    gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_CYAN);
    SelectObject(gDrawData.hdcMem, gDrawData.hFillPen);
    MoveToEx (gDrawData.hdcMem, x_left, scan, NULL);
    LineTo (gDrawData.hdcMem, x_right+1, scan);
  }  
  else if(f1+f2+f3==2){
    /*case2:  two circles have intersections with the scan line*/ 
     vector<int > p;
     if(f1){
       p.push_back(min( x1 + cx1 , -x1 + cx1));
       p.push_back(max( x1 + cx1 , -x1 + cx1));
      }
      if(f2){
       p.push_back(min( x2 + cx2 , -x2 + cx2));
       p.push_back(max( x2 + cx2 , -x2 + cx2));
      }
      if(f3){
       p.push_back(min( x3 + cx3 , -x3 + cx3));
       p.push_back(max( x3 + cx3 , -x3 + cx3)); 
      }
     sort(p.begin(),p.end());        //sort the poins in increasing order
     for( int i=0;i< p.size()-1;i++)
     {
      int n=0;
       int mid = (p[i] + p[i+1])/2;      //mid point 
       if(circle(mid,scan,0))            //checking where the mid-point lies
         n++;
       if(circle(mid,scan,1))
         n++;
      if(circle(mid,scan,2))
          n++;

      if(n==2){
        if(f1 && f2)
          gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_BLUE);
        else if(f2 && f3)
          gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_GREEN);
        else if(f1 && f3)
          gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_YELLOW);     //sets the colour of the pen
      }
      else if(n==1)
        gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_CYAN);
       
      if(n){
       SelectObject(gDrawData.hdcMem, gDrawData.hFillPen);    //selects the pen
       MoveToEx (gDrawData.hdcMem, p[i], scan, NULL);         //move to the left point    
       LineTo (gDrawData.hdcMem, p[i+1]+1, scan);             //draw the line from p[i] to p[i+1]               
      }
     }    
  }
     
  else if( f1 && f2 && f3){
         /*case7:  All circles have intersections with the scan line*/ 
    int x31 =  min( x3+ cx3 , -x3 + cx3);
    int x32 =  max( x3 + cx3 , -x3 + cx3);
    int x11 =  min( x1 + cx1 , -x1 + cx1);
    int x12 =  max( x1 + cx1 , -x1 + cx1);
    int x21 =  min( x2 + cx2 , -x2 + cx2);
    int x22 =  max( x2 + cx2 , -x2 + cx2);
    vector<int > p;
    p.push_back(x31);
    p.push_back(x32);
    p.push_back(x11);
    p.push_back(x12);
    p.push_back(x21);
    p.push_back(x22);
    sort(p.begin(),p.end());
    
    for( int i=0;i< p.size()-1;i++)
    {

      vector<int> n1;
      int mid = (p[i]+p[i+1])/2;
      if(circle(mid,scan,0))
        n1.push_back(1);
      if(circle(mid,scan,1))
        n1.push_back(2);   
      if(circle(mid,scan,2))
        n1.push_back(3);

      if(n1.size()==3)
      {
        gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_RED);
        SelectObject(gDrawData.hdcMem, gDrawData.hFillPen);
        MoveToEx (gDrawData.hdcMem, p[i], scan, NULL);
        LineTo (gDrawData.hdcMem, p[i+1]+1, scan);          
      }
      else if(n1.size()==2)
      {
        if(n1[0]==1 && n1[1]==2){
          gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_BLUE);          
        }
        else if(n1[0]==1 && n1[1]==3) {
          gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_YELLOW); 
        }
        else if(n1[0]==2 &&n1[1]==3)
        {
          gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_GREEN); 
        }  
        SelectObject(gDrawData.hdcMem, gDrawData.hFillPen);
        MoveToEx (gDrawData.hdcMem, p[i], scan, NULL);
        LineTo (gDrawData.hdcMem, p[i+1]+1, scan);        

      }
      else if(n1.size()==1){
        gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_CYAN);
        SelectObject(gDrawData.hdcMem, gDrawData.hFillPen);
        MoveToEx (gDrawData.hdcMem, p[i], scan, NULL);
        LineTo (gDrawData.hdcMem, p[i+1]+1, scan);   
      }
    }  
  }                                  

  scan++;
  f1=0;
  f2=0;
  f3=0;
  }
}
