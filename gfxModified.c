// Pat Myron and Anna McMahon
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* gfx_open creates several X11 objects, and stores them in these globals
   the globals are used by the other functions in the library.  */

static Display *gfx_display=0;
static Window  gfx_window;
static GC      gfx_gc;
static Colormap gfx_colormap;
static int      gfx_fast_color_mode = 0;




/* These values are saved by gfx_wait then retrieved later by gfx_xpos and gfx_ypos. */
static int saved_xpos = 0;
static int saved_ypos = 0;

/* Open a new graphics window. */
void gfx_open( int width, int height, const char *title )
{
	gfx_display = XOpenDisplay(0);
	if(!gfx_display) {
		fprintf(stderr,"gfx_open: unable to open the graphics window.\n");
		exit(1);
	}

	Visual *visual = DefaultVisual(gfx_display,0);
	if(visual && visual->class==TrueColor) {
		gfx_fast_color_mode = 1;
	} else {
		gfx_fast_color_mode = 0;
	}

	int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
	int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));

	gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0, width, height, 0, blackColor, blackColor);

	XSetWindowAttributes attr;
	attr.backing_store = Always;

	XChangeWindowAttributes(gfx_display,gfx_window,CWBackingStore,&attr);

	XStoreName(gfx_display,gfx_window,title);

	XSelectInput(gfx_display, gfx_window, StructureNotifyMask|KeyPressMask|ButtonPressMask | KeyReleaseMask);

	XMapWindow(gfx_display,gfx_window);

	gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);
	//gfx_gc.font = 
	gfx_colormap = DefaultColormap(gfx_display,0);

	XSetForeground(gfx_display, gfx_gc, whiteColor);

	/* Wait for the MapNotify event */
	for(;;) {
		XEvent e;
		XNextEvent(gfx_display, &e);
		if (e.type == MapNotify)
			break;
	}
}

/* Flush all previous output to the window. */
void gfx_flush()
{
	XFlush(gfx_display);
}

/* Change the current drawing color. */
void gfx_color( int r, int g, int b )
{
	XColor color;

	if(gfx_fast_color_mode) {
		/* If this is a truecolor display, we can just pick the color directly. */
		color.pixel = ((b&0xff) | ((g&0xff)<<8) | ((r&0xff)<<16) );
	} else {
		/* Otherwise, we have to allocate it from the colormap of the display. */
		color.pixel = 0;
		color.red = r<<8;
		color.green = g<<8;
		color.blue = b<<8;
		XAllocColor(gfx_display,gfx_colormap,&color);
	}

	XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Clear the graphics window to the background color. */
void gfx_clear()
{
	XClearWindow(gfx_display,gfx_window);
}

/* Change the current background color. */
void gfx_clear_color( int r, int g, int b )
{
	XColor color;
	color.pixel = 0;
	color.red = r<<8;
	color.green = g<<8;
	color.blue = b<<8;
	XAllocColor(gfx_display,gfx_colormap,&color);

	XSetWindowAttributes attr;
	attr.background_pixel = color.pixel;
	XChangeWindowAttributes(gfx_display,gfx_window,CWBackPixel,&attr);
}

/* See if an event has occurred */
int gfx_event_waiting()
{
       XEvent event;

       gfx_flush();
       while (1) {
               if(XCheckMaskEvent(gfx_display,-1,&event)) {
                       if(event.type==KeyPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else if (event.type==ButtonPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else {
                               return 0;
                       }
               } else {
                       return 0;
               }
       }
}


int gfx_event_waiting2()
{
       XEvent event;

       gfx_flush();
       while (1) {
               if(XCheckMaskEvent(gfx_display,-1,&event)) {
                       if(event.type==KeyPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else if (event.type==ButtonPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else if (event.type==KeyRelease) {
			       XPutBackEvent(gfx_display,&event);
                               return 1; 
		       } 
     			 else {
                               return 0;
                       }
               } else {
                       return 0;
               }
       }
}



/* Wait for the user to press a key or mouse button. */
char gfx_wait()
{
	XEvent event;

	gfx_flush();
	while(1) {
		XNextEvent(gfx_display,&event);

		if(event.type==KeyPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return XLookupKeysym(&event.xkey,0);
		} else if(event.type==ButtonPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return event.xbutton.button;
		}
	}
}

/* Return the X coordinate of the last event. */
int gfx_xpos()
{
	return saved_xpos;
}

/* Return the  Y coordinate of the last event. */
int gfx_ypos()
{
	return saved_ypos;
}

/* get width and height of the screen */
int gfx_xsize()
{
	return XDisplayWidth(gfx_display, 0);
}

int gfx_ysize()
{
	return XDisplayHeight(gfx_display, 0);
}

/* Draw a string at point (x,y) */
void gfx_text( int x, int y , char *text )
	
{	
	//Font font=XLoadFont(gfx_display,-bitstream-charter-medium-r-normal--12-120-75-75-p-68-iso8859-1[65 70 80_90]);
	//XTextItem stritem={text,50,5,};
       XDrawString(gfx_display, gfx_window, gfx_gc, x, y, text, strlen(text));
	//XDrawText(gfx_display, gfx_window, gfx_gc, x, y,stritem,1);
}

/* Draw a single point at (x,y) */
void gfx_point( int x, int y )
{
	XDrawPoint(gfx_display, gfx_window, gfx_gc, x, y);
}

/* Draw a line from (x1,y1) to (x2,y2) */
void gfx_line( int x1, int y1, int x2, int y2 )
{
	XDrawLine(gfx_display, gfx_window, gfx_gc, x1, y1, x2, y2);
}

/* Draw a circle; uses a version of Bresenham's circle algorithm */
void gfx_circle( int xCtr, int yCtr, int r )
{
   int x, y;	/* vert. and horiz. distance from center */
   int p;	/* parameter to control changes in y */

   y = r;
   p = 3 - 2*r;

   for (x = 0; x <= y; x++)
   {
	gfx_point(xCtr + x, yCtr + y);
	gfx_point(xCtr + x, yCtr - y);
	gfx_point(xCtr - x, yCtr + y);
	gfx_point(xCtr - x, yCtr - y);
	gfx_point(xCtr + y, yCtr + x);
	gfx_point(xCtr + y, yCtr - x);
	gfx_point(xCtr - y, yCtr + x);
	gfx_point(xCtr - y, yCtr - x);

	if (p < 0)
	   p = p + 4 * x + 6;
	else
	{
	   p = p + 4 * (x - y) + 10;
	   y--;
	}
   } 
}

/* Draw an ellipse; uses Xlib's native arc drawing function */
void gfx_ellipse( int xCtr, int yCtr, int r1, int r2 )
{
	XDrawArc(gfx_display, gfx_window, gfx_gc, xCtr-r1, yCtr-r2, 2*r1, 2*r2, 0, 360*64);
}

/* Draw a rectangle, given its top left corner and its (positive) width and height */
void gfx_rectangle( int x, int y, int w, int h )
{ 
	XDrawRectangle(gfx_display, gfx_window, gfx_gc, x, y, w, h);
}

/* Draw a filled rectangle; similar setup to gfx_rectangle, but "one pixel in" on all four sides */
void gfx_fill_rectangle( int x, int y, int w, int h )
{ 
	XFillRectangle(gfx_display, gfx_window, gfx_gc, x, y, w, h);
}

/* Draw a polygon; uses Xlib's XPoint struct; the num_pts points are in the pointsarr array */
/* (if the last point is the same as the first point, the polygon will be a closed one) */
void gfx_polygon( XPoint *pointsarr, int num_pts )
{
	XDrawLines(gfx_display, gfx_window, gfx_gc, pointsarr, num_pts, CoordModeOrigin);
}

/* Draw a triangle connecting (x1,y1), (x2,y2), and (x3,y3) */
void gfx_triangle( int x1, int y1, int x2, int y2, int x3, int y3 )
{
	gfx_line(x1,y1,x2,y2);
	gfx_line(x2,y2,x3,y3);
	gfx_line(x3,y3,x1,y1);
}
bool gfx_keyPress(bool keys[6]){
	XEvent report,pause;
	int i;
		XNextEvent (gfx_display, &report);
		switch (report.type)
		{
		case KeyPress:
			//  fprintf (stdout, "key #%ld was pressed.\n",
			//   (long) XLookupKeysym (&report.xkey, 0));

			switch((long) XLookupKeysym (&report.xkey, 0)){
			case 97:
				keys[0] = 1;
				break;
			case 119:
				keys[1] = 1;
				break;
			case 100:
				keys[2] = 1;
				break;
			case 65361:
				keys[3] = 1;
				break;
			case 65362:
				keys[4] = 1;
				break;
			case 65363:
				keys[5] = 1;
				break;
			case 65307:
				printf("escaped\n");
				return 1;
				break;
			case 112:
				printf("paused\n");
				while(1){
					//pause=XEvent(NULL);
					XNextEvent (gfx_display, &pause);
					if( (long) XLookupKeysym (&pause.xkey, 0) == 112)
						break;
					else
						printf("press p to continue\n");					
				}
				break;
			default:
				//printf("wrong key");
				break;
			}

			int counter;
			/*
			for(counter=0;counter<6;counter++)
				printf("%i ",keys[counter]);
			printf("\n\n");
			*/
			break;
		case KeyRelease:
			{
				unsigned short is_retriggered = 0;

				if (XEventsQueued(gfx_display, QueuedAfterReading))
				{
					XEvent nev;
					XPeekEvent(gfx_display, &nev);

					if (nev.type == KeyPress && nev.xkey.time == report.xkey.time &&
						nev.xkey.keycode == report.xkey.keycode)
					{

						// delete retriggered KeyPress event
						XNextEvent (gfx_display, &report);
						is_retriggered = 1;
					}
				}

				if (!is_retriggered){
					//   fprintf (stdout, "key #%ld was released.\n",
					// (long) XLookupKeysym (&report.xkey, 0));

					switch((long) XLookupKeysym (&report.xkey, 0)){
					case 97:
						keys[0] = 0;
						break;
					case 119:
						keys[1] = 0;
						break;
					case 100:
						keys[2] = 0;
						break;
					case 65361:
						keys[3] = 0;
						break;
					case 65362:
						keys[4] = 0;
						break;
					case 65363:
						keys[5] = 0;
						break;	
					default:
						//printf("wrong key");
						break;
					}
					int counter;
					/*
					for(counter=0;counter<6;counter++)
						printf("%i ",keys[counter]);
					printf("\n\n");
					*/
				}
			}
			break;
		}
	return 0;
}
