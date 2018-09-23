#include <windows.h>
#include "Fill.h"

DRAWING_DATA gDrawData; // global data
void drawCircle(int r, int circle_no);
LRESULT CALLBACK DlgAxis(HWND hdlg,UINT mess,WPARAM more,LPARAM pos);


void reDraw(HWND hwnd)
{
  InvalidateRect(hwnd, NULL, 1);
}

void drawPoint(int x, int y)
{
  SetPixel(gDrawData.hdcMem, x, y, CLR_BOUNDARY);
}

void setDrawMode(MODE mode, HWND hwnd)
{
  gDrawData.drawMode = mode;
  setupMenus(hwnd);
}

void createMemoryBitmap(HDC hdc)
{
  gDrawData.hdcMem = CreateCompatibleDC(hdc);
  gDrawData.hbmp = CreateCompatibleBitmap(hdc, 
       gDrawData.maxBoundary.cx, gDrawData.maxBoundary.cy);
  SelectObject(gDrawData.hdcMem, gDrawData.hbmp);
  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx, 
       gDrawData.maxBoundary.cy, PATCOPY);
}

void initialize(HWND hwnd, HDC hdc)
{
  gDrawData.hDrawPen=CreatePen(PS_SOLID, 1, CLR_BOUNDARY);

  gDrawData.maxBoundary.cx = GetSystemMetrics(SM_CXSCREEN);
  gDrawData.maxBoundary.cy = GetSystemMetrics(SM_CYSCREEN);
  createMemoryBitmap(hdc);
  setDrawMode(READY_MODE, hwnd);
}

void cleanup()
{
  DeleteDC(gDrawData.hdcMem);
}

void reset(HWND hwnd)
{

  gDrawData.cmade = 0;
  gDrawData.centres[0].x = gDrawData.centres[0].y = 0;
  gDrawData.centres[1].x = gDrawData.centres[1].y = 0;
  gDrawData.centres[2].x = gDrawData.centres[2].y = 0;
  gDrawData.drawMode = READY_MODE;

  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx, 
         gDrawData.maxBoundary.cy, PATCOPY);

  reDraw(hwnd);
  setupMenus(hwnd);
}

void plot_sympoint(int ex, int ey,int i, COLORREF clr)
{
  int cx = gDrawData.centres[i].x;
  int cy = gDrawData.centres[i].y;

  SetPixel(gDrawData.hdcMem, ex+cx,cy-ey, clr);
  SetPixel(gDrawData.hdcMem, -ex+cx,cy-ey, clr);
  SetPixel(gDrawData.hdcMem, -ex+cx,cy+ey, clr);
  SetPixel(gDrawData.hdcMem, ex+cx,cy+ey, clr);
}

void addPoint(HWND hwnd, int x, int y)
{
  int i = gDrawData.cmade;
  /* the coordinates of the centre is stored 
     and the ellipse is drawn */
  SelectObject(gDrawData.hdcMem, gDrawData.hDrawPen);
  drawPoint(x,y);
  gDrawData.centres[i].x = x;
  gDrawData.centres[i].y = y;
  drawCircle(gDrawData.radius[i],i);
  if(i==2){  
    setDrawMode(DRAWN_MODE, hwnd);
    reDraw(hwnd);
	}
  gDrawData.cmade++;  
}


void drawImage(HDC hdc)
{
  BitBlt(hdc, 0, 0, gDrawData.maxBoundary.cx, 
    gDrawData.maxBoundary.cy, gDrawData.hdcMem, 0, 0, SRCCOPY);
}

void processLeftButtonDown(HWND hwnd, int x, int y)
{
  switch (gDrawData.drawMode)
  {
    case DRAW_MODE:
      addPoint(hwnd,x,y);
      reDraw(hwnd);
      break;

    default:
      MessageBox(hwnd,"Circles already drawn, now you can clear the area", "Warning",MB_OK);
      return;
  }
}


void processCommonCommand(int cmd, HWND hwnd)
{
  int response;
  switch(cmd)
  {
    case ID_CLEAR:
      reset(hwnd);
      setDrawMode(READY_MODE, hwnd);
      break;
    case ID_CIRCLES:
//      MessageBox(hwnd,"Click the right button after clicking the last corner point","Remember",MB_OK);
      setDrawMode(DRAW_MODE, hwnd);
      DialogBox(NULL,"MyDB",hwnd,(DLGPROC)DlgAxis);
      break;
    case ID_EXIT:
      response=MessageBox(hwnd,"Quit the program?", "EXIT",
                          MB_YESNO);
      if(response==IDYES)
        PostQuitMessage(0);
      break;
    default:
      break;
  }
}

void drawCircle(int r, int circle_no)
{
  int x,y;
  x=0;
  y=r;
  plot_sympoint(x,y,circle_no, RGB(0,0,0));

  double d=(r*r)-(r*r*r)+(r*r/4);
  while((r*r*(y-(1/2)))>(r*r*(x+1)))  
  {
    /* in region 1*/
    if(d<0)
    {
      /*choose E*/
      d=d+(r*r*(2*x+3));
      x=x+1;
    }
    else                              
    {
      /*choose SE*/
      d=d+(r*r*(2*x+3))+(r*r*(2-2*y));
      x=x+1;
      y=y-1;
    }
    plot_sympoint(x,y,circle_no, RGB(0,0,0));
  }
  d=(r*r*(x+(1/2))*(x+1/2))
    +r*r*(y-1)*(y-1)-(r*r*r*r);
  while(y>0)   
  {
    /*in region 2*/
    if(d<0)
    {
      /*choose SE*/
      d=d+r*r*(2*x+2)+r*r*(3-2*y);
      x=x+1;
      y=y-1;
    }
    else
    {
      /*choose S*/
      d=d+r*r*(3-2*y);
      y=y-1;
    }
    plot_sympoint(x,y,circle_no, RGB(0,0,0));
  }
}

LRESULT CALLBACK DlgAxis(HWND hdlg,UINT mess,WPARAM more,LPARAM pos)
{
  char str[20];
  switch(mess)
  {
    case WM_COMMAND:
      switch(more)
      {
        case ID_OK:
          GetDlgItemText(hdlg,ID_EB1,str,20);
          gDrawData.radius[0]=(long int)(atof(str))/2;
          GetDlgItemText(hdlg,ID_EB2,str,20);
          gDrawData.radius[1]=(long int)(atof(str))/2;
          GetDlgItemText(hdlg,ID_EB3,str,20);
          gDrawData.radius[2]=(long int)(atof(str))/2;
   
          EndDialog(hdlg,TRUE);
          return 1;
          break;

        case ID_CANCEL:
          EndDialog(hdlg,TRUE);
          break;
      }
      break;
    default:
      break;
  }
  return 0;
}


LRESULT CALLBACK WindowF(HWND hwnd,UINT message,WPARAM wParam,
                         LPARAM lParam)
{
  HDC hdc;
  PAINTSTRUCT ps;
  int x,y;

  switch(message)
  {
    case WM_CREATE:
      hdc = GetDC(hwnd);
      initialize(hwnd, hdc);
      ReleaseDC(hwnd, hdc);
      break;

    case WM_COMMAND:
      processCommand(LOWORD(wParam), hwnd);
      break;

    case WM_LBUTTONDOWN:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      processLeftButtonDown(hwnd, x,y);
      break;

    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      drawImage(hdc);
      EndPaint(hwnd, &ps);
      break;

    case WM_DESTROY:
      cleanup();
      PostQuitMessage(0);
      break;

    default:
      break;
  }
  // Call the default window handler
  return DefWindowProc(hwnd, message, wParam, lParam);
}
