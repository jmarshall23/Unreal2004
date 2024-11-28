// Doodle.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <math.h>
#include <stdio.h>
void MyPaint(HDC hdc);

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DOODLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DOODLE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DOODLE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_DOODLE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
int n1=3, n2=4, kx=0, sx=0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING] = "Press asdfzxcv";

        switch (message) 
	{
		case WM_CHAR:          // Johnh added this.
                         if (wParam=='d') n2 -= 1;
                    else if (wParam=='f') n2 += (n2<25);
                    else if (wParam=='a') n1 -= 1;
                    else if (wParam=='s') n1 += (n1<25);
                    else if (wParam=='z') kx -= 1;
                    else if (wParam=='x') kx += 1;
                    else if (wParam=='c') sx -= 1;
                    else if (wParam=='v') sx += 1;
                    else if (wParam==27)
                        PostQuitMessage(0);
                    RedrawWindow(hWnd,0,0,RDW_INVALIDATE|RDW_ERASE);
                    break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
                        MyPaint(hdc);
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


/****************************************************************************
  This is my code, everything above is generated by MSVC.
*/

typedef float MeReal;
typedef MeReal MeVector3[3];

void McdPolygonIntersection(const MeVector3 normal, MeReal dist,
                            int numpoly1, MeVector3 *poly1, 
                            int numpoly2, MeVector3 *poly2, 
                            int *numOut, MeVector3 *polyOut);


/****************************************************************************
  set the pen width and color, rgb is 3 binary bits
*/
void SetPen(HDC hdc, int width, int r, int g, int b)
{
    static int i=0;
    HPEN hPen = CreatePen(PS_SOLID, width, RGB(r*255,g*255,b*255));
    HPEN hPenOld = (HPEN) SelectObject(hdc, hPen); 
    SelectObject(hdc, GetStockObject(NULL_BRUSH)); 
    if (++i > 10) DeleteObject(hPenOld); 
}

void MyPaint(HDC hdc)
{
    int i,n;
    MeVector3 t[100];
    double pi=6.28;

    //  Construct the two polygons
    for (i=0; i<n1; ++i)
    {
        t[i][0] = (MeReal) sin(pi*i/n1)-.9f;
        t[i][1] = (MeReal) cos(pi*i/n1);
        t[i][2] = 1.f;
    }
    for (i=0; i<n2; ++i)
    {
        t[i+n1][0] = (MeReal) (sin(pi*i/n2)*pow(1.1,sx) + .1*kx); 
        t[i+n1][1] = (MeReal) (cos(pi*i/n2)*pow(1.05,sx));
        t[i+n1][2] = 1.f;
    }

    MeVector3 norm = {0,0,2};

    McdPolygonIntersection(norm, 0, n1, t, n2, t+n1, &n, t+n1+n2);

    POINT poly[30];
    int s = 200;
    int x0=400, y0=300;

    //  Draw each polygon, and then draw the intersection

    for (i=0; i<n1; ++i)
    {
        poly[i].x = x0 + (int)(s*t[i][0]);
        poly[i].y = y0 - (int)(s*t[i][1]);
    }
    SetPen(hdc,2,1,0,0);
    Polygon(hdc, poly, n1);

    for (i=0; i<n2; ++i)
    {
        poly[i].x = x0 + (int)(s*t[i+n1][0]);
        poly[i].y = y0 - (int)(s*t[i+n1][1]);
    }
    SetPen(hdc,2,0,0,1);
    Polygon(hdc, poly, n2);

    for (i=0; i<n; ++i)
    {
        poly[i].x = x0 + (int)(s*t[i+n1+n2][0]);
        poly[i].y = y0 - (int)(s*t[i+n1+n2][1]);
    }
    SetPen(hdc,2,0,1,0);
    Polygon(hdc, poly, n);

}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#define MeAtan2 (MeReal)atan2
#define MeFabs (MeReal)fabs
#define MEASSERT(x) if (!(x)) { *(int*)0 = 0; } else {}
#define MeVector3Copy(a,b) memcpy(a,b,3*sizeof*a)
#define ME_ARE_EQUAL( x, y )  (fabs(x-y) < 1e-6)
#define ME_IS_ZERO(x)           (MeFabs( (x) ) < 1e-6)
#   define ME_SMALL_EPSILON     (1.0e-6f)
#   define ME_MEDIUM_EPSILON    (1.0e-2f)


/****************************************************************************
  return true if a==(xv,yv) within tolerance
*/
int McdEqual2D(const MeVector3 a, MeReal xv, MeReal yv, int x, int y)
{
    MeReal u = a[x]-xv, v = a[y]-yv;
    return u < ME_MEDIUM_EPSILON 
        && u > -ME_MEDIUM_EPSILON 
        && v < ME_MEDIUM_EPSILON 
        && v > -ME_MEDIUM_EPSILON;
}
/****************************************************************************
  r = a - b
*/
void McdSubtract2D(MeVector3 r, const MeVector3 a, const MeVector3 b, int x, int y)
{
    r[x] = a[x] - b[x];
    r[y] = a[y] - b[y];
}
/****************************************************************************
  r = a + b
*/
void McdAdd2D(MeVector3 r, const MeVector3 a, const MeVector3 b, int x, int y)
{
    r[x] = a[x] + b[x];
    r[y] = a[y] + b[y];
}
/****************************************************************************
  return a x b
*/
MeReal McdCross2D(const MeVector3 a, const MeVector3 b, int x, int y)
{
    return a[x]*b[y] - b[x]*a[y];
}

/****************************************************************************
  One of these is called by qsort.  
  I had to make one for each axis.
*/
typedef int (*MePolyPointCompareFn)(const void*,const void*);

int MePolyPointCompare0(const void *p1, const void *p2) 
{   
    MeReal t = ((MeReal*)p1)[0] - ((MeReal*)p2)[0];
    return t < 0 ? -1 : t > 0 ? 1 : 0;
}
int MePolyPointCompare1(const void *p1, const void *p2) 
{   
    MeReal t = ((MeReal*)p1)[1] - ((MeReal*)p2)[1];
    return t < 0 ? -1 : t > 0 ? 1 : 0;
}
int MePolyPointCompare2(const void *p1, const void *p2) 
{   
    MeReal t = ((MeReal*)p1)[2] - ((MeReal*)p2)[2];
    return t < 0 ? -1 : t > 0 ? 1 : 0;
}

/****************************************************************************
  This sorts polygon vertices. 

  First compute the center of the polygon, then compute the angle of each
  vertex against the middle using cross prod.  Store the angle
  in the axis coordinate.  Then sort by angle.
*/
void MePolygonSort(int numpoly, MeVector3 *poly, int axis)
{
    int i, x, y;
    MeVector3 v, mid = {0,0,0};

    if (numpoly < 3)
        return;

    x = (axis + 1) % 3;
    y = (axis + 2) % 3;

    //  Find poly midpoint
    for (i = 0; i < numpoly; ++i)
        McdAdd2D(mid, mid, poly[i], x, y);

    mid[x] *= (MeReal) 1/numpoly;
    mid[y] *= (MeReal) 1/numpoly;

    for (i = 0; i < numpoly; ++i)
    {
        McdSubtract2D(v, poly[i], mid, x, y);
        poly[i][axis] = MeAtan2(v[y], v[x]);
    }

    MePolyPointCompareFn cmp[] = { MePolyPointCompare0, MePolyPointCompare1, MePolyPointCompare2 };

    qsort(poly, numpoly, sizeof *poly, cmp[axis]);
}

/****************************************************************************
  This appends one 2D point to an array of 3D vectors.
  It test if the new point is the same as the last point, or the first 
  point in polyOut.  Return 1 if it is the same as the first.
*/
int McdAddPoint(int *numOut, MeVector3 *polyOut, 
                MeReal xv, MeReal yv, int x, int y)
{
    int i = *numOut;

    if (i > 0 && McdEqual2D(polyOut[i-1], xv, yv, x, y))
        return 0;

    if (i > 1 && McdEqual2D(polyOut[0], xv, yv, x, y))
        return 1;

    polyOut[i][x] = xv;
    polyOut[i][y] = yv;
    ++*numOut;
    return 0;
}


/****************************************************************************
  This computes the intersection of two polygons.
  It uses the sickle algorithm, see O'Rourke 82 at

      http://www.cs.smith.edu/~orourke/books/compgeom.html
      http://citeseer.nj.nec.com/context/201691/0

  The normal and distance determines the projection plane.  All the points
  in poly1 and poly2 are assumed to lie on (or close to) this plane.

  This is function messes up poly1 and poly2, they are flattened to
  a coordinate plane and sorted using qsort.

  The caller MUST allocate sufficient space for polyOut.  The maximum
  size required is 2*min(numpoly1, numpoly2).
*/
void McdPolygonIntersection(const MeVector3 normal, MeReal dist,
                            int numpoly1, MeVector3 *poly1, 
                            int numpoly2, MeVector3 *poly2, 
                            int *numOut, MeVector3 *polyOut)
{
    //  check parms

    MEASSERT(poly1 && poly2 && numOut && polyOut);
    *numOut = 0;

    //  special case for polygons with only one point

    if (numpoly1==1)
        MeVector3Copy(polyOut[*numOut++], poly1[0]);
    else if (numpoly2==1)
        MeVector3Copy(polyOut[*numOut++], poly2[0]);

    if (numpoly1 < 2 || numpoly2 < 2)
        return;

    //  determine best axis

    MeVector3 temp = { MeFabs(normal[0]), MeFabs(normal[1]), MeFabs(normal[2]) };
    int axis;
    if (temp[0] > temp[1])
        axis = temp[0] > temp[2] ? 0 : 2;
    else
        axis = temp[1] > temp[2] ? 1 : 2;

    //  sort each poly 

    MePolygonSort(numpoly1, poly1, axis);
    MePolygonSort(numpoly2, poly2, axis);

    //------------------------------------------------------------------
    //  begin intersection walk

    int s1, s2, e1, e2, neg1, neg2;
    int x, y, inside, change, counter; //, first_e1, first_e2;
    MeVector3 a1, a2, b1, b2, b3;
    MeReal cross, sign1, sign2, ps1, ps2;

    x = (axis + 1) % 3;
    y = (axis + 2) % 3;

    s1 = s2 = 0;        // start of each edge
    e1 = e2 = 1;        // end of edge
    change = 0;         // which poly should advance, 1 or 2
    inside = 0;         // which poly is inside the other, 1 or 2
    ps1 = ps2 = 0;      // the previous value of sign1,2
    neg1 = neg2 = 0;    // true if sign1,2 was ever negative

    counter = 2*(numpoly1 + numpoly2);   // max loops

    while (--counter)
    {
        //  a1, a2 are the sides of the two polygons
        //  b1, b2 are the bridge vectors between the sides

        if (change!=2) 
            McdSubtract2D(a1, poly1[e1], poly1[s1], x, y);
        if (change!=1) 
            McdSubtract2D(a2, poly2[e2], poly2[s2], x, y);
        McdSubtract2D(b1, poly2[e2], poly1[s1], x, y);
        McdSubtract2D(b2, poly1[e1], poly2[s2], x, y);

        cross = McdCross2D(a1, a2, x, y);
        sign1 = McdCross2D(a1, b1, x, y);
        sign2 = McdCross2D(a2, b2, x, y);

        //  look for a change in sign1 or sign2 which indicates a cross over

        if (change && (ps1*sign1 <= 0 || ps2*sign2 <= 0) 
            && !ME_IS_ZERO(cross))
        {
            MeReal k1, k2;
            McdSubtract2D(b3, poly1[s1], poly2[s2], x, y);

            k1 = McdCross2D(a2, b3, x, y) / cross;
            k2 = McdCross2D(a1, b3, x, y) / cross;

            //  Test if k1 and k2 are in the range [0, 1]

            if (MeFabs(k1 - 0.5f) - 0.5f < ME_SMALL_EPSILON && 
                MeFabs(k2 - 0.5f) - 0.5f < ME_SMALL_EPSILON )
            {
                if (McdAddPoint(numOut, polyOut, 
                        poly1[s1][x] + k1*a1[x],
                        poly1[s1][y] + k1*a1[y], x, y))
                    break;

                inside = cross < 0 ? 1 : 2;
            }
        }

        //  The following verifies that the "inside" flag is set correctly.
        //  If all goes well this shouldn't be necessary, but just in case.

        if (inside==1 && sign2<0 && sign1>0)   
            inside = 2;
        if (inside==2 && sign1<0 && sign2>0)   
            inside = 1;
        if (inside && sign1<0 && sign2<0)   
            inside = 0;

        //  These are the advance rules
        if (cross > 0)
            change = sign1 > 0 ? 1 : 2;
        else
            change = sign2 > 0 ? 2 : 1;

        if (change==1)
        {
            s1 = e1;
            e1 = (e1 + 1) % numpoly1;
            if (inside==change &&
                McdAddPoint(numOut, polyOut, poly1[s1][x], poly1[s1][y], x, y))
                break;
        }
        else
        {
            s2 = e2;
            e2 = (e2 + 1) % numpoly2;
            if (inside==change &&
                McdAddPoint(numOut, polyOut, poly2[s2][x], poly2[s2][y], x, y))
                break;
        }
        ps1 = sign1;
        ps2 = sign2;
        neg1 |= sign1 < 0;
        neg2 |= sign2 < 0;

        //  If we've gone all the way around either poly then break.
        //  Look at the neg1,2 flag to see if either is completely inside the other.

        if (!*numOut && !(change==1 ? s1 : s2))
        {
            if (!neg2)
                memcpy(polyOut, poly1, (*numOut = numpoly1)*sizeof *polyOut);
            else if (!neg1)
                memcpy(polyOut, poly2, (*numOut = numpoly2)*sizeof *polyOut);
            break;
        }
    }
/*
    //  This is a bit of a hack.  Sometimes the first and last points are
    //  identical, thus check for this and remove the last point.

    if (*numOut > 2 
        && ME_ARE_EQUAL(polyOut[0][x], polyOut[*numOut-1][x]) 
        && ME_ARE_EQUAL(polyOut[0][y], polyOut[*numOut-1][y]))
        --*numOut;
*/
    //  Compute the missing coordinate for the polyOut by projecting 
    //  back onto the contact plane.

    int i;
    for (i = 0; i < *numOut; ++i)
        polyOut[i][axis] = (dist - normal[x]*polyOut[i][x] - normal[y]*polyOut[i][y]) / normal[axis];

        //   DEBUGGING

    char ds[1000];
//        sprintf(ds,"%d %d \t %c %c %c \t %d %d\n",
//            s1,s2,cross<0?'-':'+',sign1<0?'-':'+',sign2<0?'-':'+',change, *numOut);

    for (i=0; i<*numOut; ++i)
    {
        sprintf(ds,"(%5.2f, %5.2f) ",polyOut[i][0],polyOut[i][1]);
        OutputDebugString(ds);
    }
    sprintf(ds,"\n%d\n",*numOut);
    OutputDebugString(ds);

}










