#define ID_CIRCLES         40001
#define ID_CLEAR           40002
#define ID_EXIT            40003
#define ID_FILL            40004
#define ID_OK              40005
#define ID_CANCEL          40006
#define ID_EB1             40007
#define ID_EB2             40008
#define ID_EB3             40009
#define ID_LABEL1          40010
#define ID_LABEL2          40011
#define ID_LABEL3          40012

const COLORREF CLR_CYAN = RGB(0, 255, 255);
const COLORREF CLR_BLUE = RGB(0, 0, 255);
const COLORREF CLR_RED = RGB(255, 0, 0);
const COLORREF CLR_GREEN = RGB(0, 255, 0);
const COLORREF CLR_YELLOW = RGB(255, 255, 0);
const COLORREF CLR_BOUNDARY = RGB(0, 0, 0);
const COLORREF CLR_BG = RGB(255, 255, 255);

typedef enum
{
  READY_MODE,
  DRAW_MODE,
  DRAWN_MODE,
  FILL_MODE,
  FILLED_MODE
} MODE;

const int nCentres = 3;

typedef struct
{
  HDC hdcMem; 
  HBITMAP hbmp;
  HPEN hDrawPen, hFillPen;
  MODE drawMode;
  POINT centres[nCentres];/* corner points of polygon */
  SIZE maxBoundary;
  int cmade;
  // int nCornerPts; /* number of corner points of the polygon */ 
  int radius[nCentres];
} DRAWING_DATA;

// typedef struct
// {
//   int yMin, yMax;
//   double x;
//   double mInv;
// } EDGE_ENTRY;

extern DRAWING_DATA gDrawData; // global data being shared across files

void setupMenus(HWND hwnd);
void processRightButtonDown(HWND hwnd);
void processCommand(int cmd, HWND hwnd);
void setDrawMode(MODE mode, HWND hwnd);
void reDraw(HWND hwnd);
void processCommonCommand(int cmd, HWND hwnd);
