/*
 * $Id: preview.c,v 1.6 1999/04/20 06:18:25 jcp Exp $
 *
 * Copyright (C) 1998, John C. Peterson <mailto:jaypee@netcom.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2. A copy is included in this distribution in the file
 * named "LICENSE".
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License, version 2, for more details.
 *
 * Note that the version 1.3 release of fplan (and older releases)
 * are the work of Steve Tynor <mailto:tynor@atlanta.twr.com>. Those
 * versions of fplan are in the public domain, (provided the source
 * code headers are left intact). Here is the original header;
 * 
 */

/*
 *----------------------------------------------------------------------------
 *	FPLAN - Flight Planner
 *	Steve Tynor
 *	tynor@prism.gatech.edu
 *
 *	This program is in the public domain. Permission to copy,
 * distribute, modify this program is hearby given as long as this header
 * remains. If you redistribute this program after modifying it, please
 * document your changes so that I do not take the blame (or credit) for
 * those changes.  If you fix bugs or add features, please send me a
 * patch so that I can keep the 'official' version up-to-date.
 *
 *	Bug reports are welcome and I'll make an attempt to fix those
 * that are reported.
 *
 *	USE AT YOUR OWN RISK! I assume no responsibility for any
 * errors in this program, its database or documentation. I will make an
 * effort to fix bugs, but if you crash and burn because, for example,
 * fuel estimates in this program were inaccurate, it's your own fault
 * for trusting somebody else's code! Remember, as PIC, it's _your_
 * responsibility to do complete preflight planning. Use this program as
 * a flight planning aid, but verify its results before using them.
 *----------------------------------------------------------------------------
 */

static char rcsid[] = "$Id: preview.c,v 1.6 1999/04/20 06:18:25 jcp Exp $";

#ifdef GFX_GNOME

/*
 *==============================================================================
 * Graphics routines for GNOME
 *==============================================================================
 */

#include <math.h>
#include <gtk/gtk.h>
#include <gnome.h>

#include "common.h"

static double min_lat;
static double max_lat;
static double min_lon;
static double max_lon;
static double pixels_per_unit = 60.0;

static BOOLEAN brief_mode;
static BOOLEAN draw_grid;

static GtkWidget *canvas;
static GSList *brief_items;
static guint line_type, text_type, rect_type, group_type;

/*----------------------------------------------------------------------------*/
static GnomeCanvasItem *
gnome_draw_line(double x1, double y1, double x2, double y2, int pixel_width,
		GdkLineStyle line_style, GdkCapStyle cap_style,
		GdkJoinStyle join_style)
{
   GnomeCanvasPoints *points = gnome_canvas_points_new(2);
   points->coords[0] = -x1; points->coords[1] = -y1;
   points->coords[2] = -x2; points->coords[3] = -y2;
   return gnome_canvas_item_new (gnome_canvas_root (GNOME_CANVAS(canvas)), line_type,
	"points", points,
	"width_pixels", pixel_width,
	"fill_color", "black",
	"line_style", line_style,
	"cap_style", cap_style,
	"join_style", join_style,
	NULL);
}

/*----------------------------------------------------------------------------*/
static GnomeCanvasItem *
gnome_draw_text(double x, double y, char *text)
{
   return gnome_canvas_item_new (gnome_canvas_root (GNOME_CANVAS(canvas)), text_type,
	"text", text, "x", -x, "y", -y, "anchor", GTK_ANCHOR_SOUTH_WEST,
	"fill_color", "black", NULL);
}

/*----------------------------------------------------------------------------*/
static void
gnome_draw_grid()
{
   int first_lat, last_lat, first_lon, last_lon, this_lat, this_lon;
   double d_lat, d_lon;
   GnomeCanvasItem *item;
   char buffer[32];

   first_lat = ceil(min_lat);
   last_lat = floor(max_lat);
   first_lon = ceil(min_lon);
   last_lon = floor(max_lon);

   for (this_lat = first_lat; this_lat <= last_lat; this_lat++) {
      d_lat = this_lat;
      item = gnome_draw_line (min_lon, d_lat, max_lon, d_lat, 1,
		GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
      sprintf (buffer, "%d%c", this_lat, (this_lat > 0) ? 'N' : 'S');
      item = gnome_draw_text (min_lon+0.03, d_lat+0.03, g_strdup(buffer));
      /* FIXME: what to do with item? */
   }

   for (this_lon = first_lon; this_lon <= last_lon; this_lon++) {
      d_lon = this_lon;
      item = gnome_draw_line (d_lon, min_lat, d_lon, max_lat, 1,
		GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
      sprintf (buffer, "%d%c", this_lon, (this_lon > 0) ? 'W' : 'E');
      item = gnome_draw_text (d_lon+0.03, min_lat+0.03, g_strdup(buffer));
      /* FIXME: what to do with item? */
   }

}

/*----------------------------------------------------------------------------*/
static void
gnome_brief_mode(void) {
   if (brief_mode)
      g_slist_foreach(brief_items, (GFunc) gnome_canvas_item_hide, NULL);
   else
      g_slist_foreach(brief_items, (GFunc) gnome_canvas_item_show, NULL);
}

/*----------------------------------------------------------------------------*/
static void
gnome_draw_db (DB_INFO *db)
{
   GnomeCanvasItem *dot, *str, *group;
   double x = db->longitude, y = db->latitude;

   group = gnome_canvas_item_new (gnome_canvas_root (GNOME_CANVAS(canvas)), group_type,
	"x", -x, "y", -y, NULL);
   dot = gnome_canvas_item_new (GNOME_CANVAS_GROUP(group), rect_type,
	"x1", -0.01, "y1", -0.01, "x2", 0.01, "y2", 0.01,
	"width_pixels", 3, "outline_color", "black", NULL);
   str = gnome_canvas_item_new (GNOME_CANVAS_GROUP(group), text_type,
	"x", 0.01, "y", 0.01, "fill_color", "black",
	"anchor", GTK_ANCHOR_SOUTH_WEST,
	"text", (db->type != WP_INCREMENTAL) ? db->ident : db->name,
	NULL);

   if (db->type == WP_INCREMENTAL)
	 brief_items = g_slist_append (brief_items, group);
}

/*----------------------------------------------------------------------------*/
static void
gnome_briefp_clicked(GtkWidget *briefp)
{
   brief_mode = GTK_TOGGLE_BUTTON(briefp)->active;
   gnome_brief_mode();
}

/*----------------------------------------------------------------------------*/
static void
gnome_zoom_update(GtkAdjustment *adjustment)
{
   pixels_per_unit = adjustment->value;
   gnome_canvas_set_pixels_per_unit(GNOME_CANVAS(canvas), pixels_per_unit);
}

/*----------------------------------------------------------------------------*/
static void
gnome_scroll_to(double wx, double wy)
{
   int cx, cy;

   gnome_canvas_w2c(GNOME_CANVAS(canvas), wx, wy, &cx, &cy);
   gnome_canvas_scroll_to (GNOME_CANVAS(canvas), cx, cy);
}

/*----------------------------------------------------------------------------*/
static void
gnome_scroll_tofirst (GtkWidget *ignore)
{
   gnome_scroll_to (-waypoints[0].db->longitude, -waypoints[0].db->latitude);
}

/*----------------------------------------------------------------------------*/
static void
gnome_scroll_tolast (GtkWidget *ignore)
{
   gnome_scroll_to (-waypoints[num_waypoints-1].db->longitude,
		    -waypoints[num_waypoints-1].db->latitude);
}

/*----------------------------------------------------------------------------*/
void
gnome_draw (brief)
     BOOLEAN brief;
{
   GtkWidget *frame, *window, *vbox;
   GtkWidget *buttonbox, *quit, *first, *last, *briefp;
   GtkObject *range;
   GtkWidget *scalebox, *scale, *spin;
   GnomeCanvasPoints *points;
   int i;
   int width, height;
   char *fake_argv[2] = {"fplan", NULL};

   min_max_lat_lon (&min_lat, &max_lat, &min_lon, &max_lon);

   brief_mode = brief;

   gnome_init("fplan", NULL, 1, fake_argv);

   line_type = gnome_canvas_line_get_type();
   text_type = gnome_canvas_text_get_type();
   rect_type = gnome_canvas_rect_get_type();
   group_type = gnome_canvas_group_get_type();

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), vbox);

   frame = gtk_scrolled_window_new (NULL, NULL);
   gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

   canvas = gnome_canvas_new ();
   width = ceil(MIN((max_lon-min_lon+0.5)*pixels_per_unit, 400));
   width = MAX(width, 300);
   height = ceil(MIN((max_lat-min_lat+0.5)*pixels_per_unit, 400));
   height = MAX(height, 200);
   gtk_widget_set_usize (canvas, width, height);
   gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas),
        -(max_lon+0.25), -(max_lat+0.25), -(min_lon-0.25), -(min_lat-0.25));
   gnome_canvas_set_pixels_per_unit(GNOME_CANVAS(canvas), pixels_per_unit);
   gtk_container_add (GTK_CONTAINER (frame), canvas);

   gnome_draw_grid ();

   for (i = 0; i < num_cached; i++)
      gnome_draw_db (cache[i]);

   points = gnome_canvas_points_new (num_waypoints);
   for (i = 0; i < num_waypoints; i++) {
      points->coords[i*2] = -waypoints[i].db->longitude;
      points->coords[i*2+1] = -waypoints[i].db->latitude;
      gnome_draw_db (waypoints[i].db);
   }
   gnome_canvas_item_new (gnome_canvas_root (GNOME_CANVAS(canvas)), line_type,
	"points", points, "width_pixels", 1, "fill_color", "black",
	"line_style", GDK_LINE_SOLID,
	"cap_style", GDK_CAP_BUTT,
	"join_style", GDK_JOIN_MITER,
	NULL);

   gnome_brief_mode();

   buttonbox = gtk_hbox_new(FALSE, 6);
   quit = gtk_button_new_with_label("Quit");
   gtk_signal_connect(GTK_OBJECT(quit), "clicked",
	(GtkSignalFunc) gtk_main_quit, NULL);
   first = gtk_button_new_with_label("To First");
   gtk_signal_connect(GTK_OBJECT(first), "clicked",
	(GtkSignalFunc) gnome_scroll_tofirst, NULL);
   last = gtk_button_new_with_label("To Last");
   gtk_signal_connect(GTK_OBJECT(last), "clicked",
	(GtkSignalFunc) gnome_scroll_tolast, NULL);
   briefp = gtk_check_button_new_with_label("Brief");
   GTK_TOGGLE_BUTTON(briefp)->active = brief_mode;
   gtk_signal_connect(GTK_OBJECT(briefp), "clicked",
	(GtkSignalFunc) gnome_briefp_clicked, canvas);
   gtk_box_pack_start (GTK_BOX (buttonbox), quit, TRUE, TRUE, 0);
   gtk_box_pack_start (GTK_BOX (buttonbox), first, TRUE, TRUE, 0);
   gtk_box_pack_start (GTK_BOX (buttonbox), last, TRUE, TRUE, 0);
   gtk_box_pack_start (GTK_BOX (buttonbox), briefp, TRUE, TRUE, 0);
   gtk_box_pack_start (GTK_BOX (vbox), buttonbox, FALSE, TRUE, 0);

   scalebox = gtk_hbox_new(FALSE, 4);
   range = gtk_adjustment_new (pixels_per_unit, 15.0, 500.0, 1.0, 1.0, 0.0);
   scale = gtk_hscale_new (GTK_ADJUSTMENT(range));
   gtk_range_set_update_policy (GTK_RANGE(scale), GTK_UPDATE_CONTINUOUS);
   spin = gtk_spin_button_new (GTK_ADJUSTMENT(range), 5.0, 1);
   gtk_signal_connect(GTK_OBJECT(range), "value_changed",
	(GtkSignalFunc) gnome_zoom_update, NULL);
   gtk_box_pack_start (GTK_BOX (scalebox), scale, TRUE, TRUE, 0);
   gtk_box_pack_start (GTK_BOX (scalebox), spin, TRUE, TRUE, 0);
   gtk_box_pack_start (GTK_BOX (vbox), scalebox, FALSE, TRUE, 0);

   gtk_widget_show_all(window);

   gtk_main ();
}

#endif /* GFX_GNOME */

#ifdef GFX_XVIEW

/*
 *==============================================================================
 * Graphics routines for XView, Xlib
 *==============================================================================
 */

#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xview/scrollbar.h>

#include "common.h"

static BOOLEAN brief_mode;
static BOOLEAN center_scrollbars;

#define PIXELS_PER_UNIT 10	/* basic unit for scrollbar motion */

#define PAGE_LENGTH 5		/* number of units per scroll page */

static int c_width;		/* canvas width (pixels) */
static int c_height;		/* canvas height (pixels) */
static int v_width;		/* visible width (pixels) */
static int v_height;		/* visible height (pixels) */

static double min_lat, max_lat;	/* min and max latitude of bounding box */
static double min_lon, max_lon;	/* min and max longitude of bounding box */
static double delta_lat, delta_lon; /* bounding box width, height (degrees) */

#define DEFAULT_SCALE   100
static int scale = DEFAULT_SCALE;	/* scale factor in percent */
static double degrees_to_pixels;	/* conversion from degrees to pixels */

static int margin_w;	/* width margin of bounding lat, lon box (pixels) */
static int margin_h;	/* height margin of bounding lat, lon box (pixels) */

#define MIN_MARGIN_W   (2*PIXELS_PER_UNIT)
#define MIN_MARGIN_H   (2*PIXELS_PER_UNIT)

static Frame frame;
static Panel panel;
static Canvas canvas;
static Xv_Font font;
static Xv_Window paint_window;
static Scrollbar scrollbar_w;
static Scrollbar scrollbar_h;
static Display *display;
static Window xwindow;
static GC gc;

/* function prototypes */

static void xv_draw_pt ( int x, int y );
static void xv_draw_str ( int x, int y, char *str );
static void xv_draw_db ( DB_INFO *db );
static void xv_draw_leg ( DB_INFO *db1, DB_INFO *db2 );
static void xv_draw_grid (double min_lat, double max_lat,
			  double min_lon, double max_lon);
static void xv_project ( double latitude, double longitude, int *x, int *y );
static void xv_scroll_to ( double latitude, double longitude );

extern void xv_quit_event_proc ( Panel_item item, Event *event );
extern void xv_tofirst_event_proc ( Panel_item item, Event *event );
extern void xv_tolast_event_proc ( Panel_item item, Event *event );
extern void xv_brief_event_proc ( Panel_item item, Event *event );
extern void xv_scale_event_proc ( Panel_item item, Event *event );
extern void xv_resize_event_proc ( Xv_Window window, Event *event );
extern void xv_redraw_event_proc ( Canvas canvas );

/*----------------------------------------------------------------------------*/
static void 
xv_draw_pt (int x, int y)
{
   XFillRectangle (display, xwindow, gc, x-1, y-1, 3, 3);
}

/*----------------------------------------------------------------------------*/
static void 
xv_draw_str (int x, int y, char *str)
{
   if (str != NULL) {
      XDrawString (display, xwindow, gc, x, y, str, strlen (str));
   }
}

/*----------------------------------------------------------------------------*/
static void 
xv_draw_db (DB_INFO *db)
{
   int x, y;

   xv_project (db->latitude, db->longitude, &x, &y);
   xv_draw_pt (x, y);

   switch (db->type) {
   case WP_INCREMENTAL:
   case WP_INTERSECTION:
   case WP_LAT_LON:
   case WP_RELATIVE:
      break;
   default:
      xv_draw_str (x+3, y-3, db->ident);
      break;
   }
}

/*----------------------------------------------------------------------------*/
static void 
xv_draw_leg (DB_INFO *db1, DB_INFO *db2)
{
   int x1, y1;
   int x2, y2;

   xv_project (db1->latitude, db1->longitude, &x1, &y1);
   xv_project (db2->latitude, db2->longitude, &x2, &y2);

   XDrawLine (display, xwindow, gc, x1, y1, x2, y2);
}

/*----------------------------------------------------------------------------*/
static void
xv_draw_grid (double min_lat, double max_lat,
	      double min_lon, double max_lon)
{
   char buffer[32];
   int lat, lat_1, lat_2;
   int lon, lon_1, lon_2;
   int x1, y1;
   int x2, y2;

   XSetLineAttributes (display, gc, 1, LineOnOffDash, CapButt, JoinMiter);

   min_lon -= (margin_w - 2*MIN_MARGIN_W) / degrees_to_pixels; 
   max_lon += (margin_w - 2*MIN_MARGIN_W) / degrees_to_pixels;

   min_lat -= (margin_h - 2*MIN_MARGIN_H) / degrees_to_pixels;
   max_lat += (margin_h - 2*MIN_MARGIN_H) / degrees_to_pixels;

   lon_1 = min_lon;
   lon_2 = max_lon;
   if (lon_1 < min_lon) lon_1++;

   for (lon = lon_1; lon <= lon_2; lon++) {
      xv_project (min_lat, (double) lon, &x1, &y1);
      xv_project (max_lat, (double) lon, &x2, &y2);
      XDrawLine (display, xwindow, gc, x1, y1, x2, y2);
      sprintf (buffer, "%d%c", lon, (lon > 0) ? 'W' : 'E');
      xv_draw_str (x2, y2-3, buffer);
   }

   lat_1 = min_lat;
   lat_2 = max_lat;
   if (lat_1 < min_lat) lat_1++;

   for (lat = lat_1; lat <= lat_2; lat++) {
      xv_project ((double) lat, min_lon, &x1, &y1);
      xv_project ((double) lat, max_lon, &x2, &y2);
      XDrawLine (display, xwindow, gc, x1, y1, x2, y2);
      sprintf (buffer, "%d%c", lat, (lat > 0) ? 'N' : 'S');
      xv_draw_str (x1, y1-3, buffer);
   }

   XSetLineAttributes (display, gc, 0, LineSolid, CapButt, JoinMiter);
}

/*----------------------------------------------------------------------------*/
static void 
xv_project (double latitude, double longitude, int *x, int *y)
{
   /*
    * simple unprojected latitude, longitude coordinate system
    */

   *x = margin_w + (int) ((max_lon - longitude) * (double) degrees_to_pixels);
   *y = margin_h + (int) ((max_lat - latitude ) * (double) degrees_to_pixels);
}

/*----------------------------------------------------------------------------*/
static void 
xv_scroll_to (double latitude, double longitude)
{
   int xc, yc;
   int xv, yv;
   double x0, y0;

   xv_project (latitude, longitude, &xc, &yc);

   /*
    * try to position the canvas to center the given point
    */

   x0 = MAX (0, MIN ((double)xc - 0.5 * (double)v_width, c_width - v_width));
   y0 = MAX (0, MIN ((double)yc - 0.5 * (double)v_height, c_height - v_height));

   xv = 0.5 + (x0 / PIXELS_PER_UNIT);
   yv = 0.5 + (y0 / PIXELS_PER_UNIT);

   xv_set (scrollbar_w, SCROLLBAR_VIEW_START, xv, NULL);
   xv_set (scrollbar_h, SCROLLBAR_VIEW_START, yv, NULL);
}

/*----------------------------------------------------------------------------*/
void 
xv_quit_event_proc (Panel_item item, Event *event)
{
   xv_destroy_safe (frame);
}

/*----------------------------------------------------------------------------*/
void 
xv_tofirst_event_proc (Panel_item item, Event *event)
{
   xv_scroll_to (waypoints[0].db->latitude,
		 waypoints[0].db->longitude);
}

/*----------------------------------------------------------------------------*/
void 
xv_tolast_event_proc (Panel_item item, Event *event)
{
   xv_scroll_to (waypoints[num_waypoints-1].db->latitude,
		 waypoints[num_waypoints-1].db->longitude);
}

/*----------------------------------------------------------------------------*/
void 
xv_brief_event_proc (Panel_item item, Event *event)
{
   brief_mode = (int) xv_get (item, PANEL_VALUE);
   xv_redraw_event_proc (canvas);
}

/*----------------------------------------------------------------------------*/
void 
xv_scale_event_proc (Panel_item item, Event *event)
{
   scale = (int) xv_get (item, PANEL_VALUE);
   xv_redraw_event_proc (canvas);
}

/*----------------------------------------------------------------------------*/
void 
xv_resize_event_proc (Xv_Window window, Event *event)
{
   switch (event_action (event)) {
   case WIN_RESIZE:
      xv_redraw_event_proc (canvas);
      break;
   default:
      break;
   }
}

/*----------------------------------------------------------------------------*/
void 
xv_redraw_event_proc (Canvas canvas)
{
   double degrees_to_pixels_w;	/* degrees to pixels for width */
   double degrees_to_pixels_h;	/* degrees to pixels for height */
   int width;
   int height;
   int i;

   paint_window = canvas_paint_window (canvas);
   display = (Display *) XV_DISPLAY_FROM_WINDOW (paint_window);
   xwindow = (Window) xv_get (paint_window, XV_XID);
   gc = (GC) DefaultGC (display, DefaultScreen (display));

   /* compute the dimensions of the canvas */

   v_width = (int) xv_get (scrollbar_w, SCROLLBAR_VIEW_LENGTH);
   v_height = (int) xv_get (scrollbar_h, SCROLLBAR_VIEW_LENGTH);

   v_width *= PIXELS_PER_UNIT;	/* convert from units to pixels */
   v_height *= PIXELS_PER_UNIT;

   degrees_to_pixels_w  = (double) (v_width - 2*MIN_MARGIN_W) / delta_lon;
   degrees_to_pixels_h  = (double) (v_height - 2*MIN_MARGIN_H) / delta_lat;

   degrees_to_pixels = MIN (degrees_to_pixels_w, degrees_to_pixels_h);
   degrees_to_pixels *= ((double) scale / 100.0);

   width = MAX (delta_lon*degrees_to_pixels + 4*MIN_MARGIN_W,
		v_width + 2*MIN_MARGIN_W);
   height = MAX (delta_lat*degrees_to_pixels + 4*MIN_MARGIN_H,
		 v_height + 2*MIN_MARGIN_H);

   width = ((width+PIXELS_PER_UNIT-1) / PIXELS_PER_UNIT) * PIXELS_PER_UNIT;
   height = ((height+PIXELS_PER_UNIT-1) / PIXELS_PER_UNIT) * PIXELS_PER_UNIT;

   margin_w = 0.5 * (width - delta_lon*degrees_to_pixels);
   margin_h = 0.5 * (height - delta_lat*degrees_to_pixels);

   /* update the canvas dimensions only if they have changed */

   if ((c_width != width) || (c_height != height)) {
      /* update the canvas dimensions */
      c_width = width;
      c_height = height;
      xv_set (canvas, CANVAS_WIDTH, c_width, NULL);
      xv_set (canvas, CANVAS_HEIGHT, c_height, NULL);
      /* scroll to center of bounding box */
      center_scrollbars = TRUE;
   }

   /* clear the canvas and redraw */

   XClearWindow (display, xwindow);

   /* draw latitude, longitude grid */

   xv_draw_grid (min_lat, max_lat, min_lon, max_lon);

   /* draw all waypoints in the lookup cache */

   for (i = 0; i < num_cached; i++) {
      xv_draw_db (cache[i]);
   }

   /* draw the waypoints and legs */

   for (i = 0; i < num_waypoints - 1; i++) {
      xv_draw_leg (waypoints[i].db, waypoints[i+1].db);
      switch (waypoints[i].db->type) {
      case WP_INCREMENTAL:
	 if (!brief_mode)
	    xv_draw_db (waypoints[i].db);
	 break;
      case WP_INTERSECTION:
      case WP_LAT_LON:
      case WP_RELATIVE:
	 xv_draw_db (waypoints[i].db);
	 break;
      default:
	 break;
      }
   }

   if (center_scrollbars) {
      xv_scroll_to (0.5 * (min_lat + max_lat),
		    0.5 * (min_lon + max_lon));
      center_scrollbars = FALSE;
   }
}

/*----------------------------------------------------------------------------*/
void 
xv_draw (BOOLEAN brief)
{

   brief_mode = brief;

   min_max_lat_lon (&min_lat, &max_lat, &min_lon, &max_lon);
   delta_lat = max_lat - min_lat;
   delta_lon = max_lon - min_lon;

   frame = (Frame) xv_create (XV_NULL,            FRAME,
			      FRAME_LABEL,        "fplan: preview mode",
			      FRAME_MIN_SIZE,     350, 422,
			      FRAME_NO_CONFIRM,   TRUE,
			      FRAME_SHOW_LABEL,   TRUE,
			      FRAME_SHOW_RESIZE_CORNER, TRUE,
			      XV_WIDTH,           400,
			      XV_HEIGHT,          472,
			      WIN_EVENT_PROC,     xv_resize_event_proc,
			      WIN_CONSUME_EVENTS, WIN_RESIZE, NULL,
			      NULL);

   font = (Xv_Font) xv_find (frame, FONT, FONT_FAMILY, "Fixed", NULL);

   if (!font) font = (Xv_Font) xv_get (frame, XV_FONT);

   panel = (Panel) xv_create (frame, PANEL, XV_FONT, font, NULL);
   
   xv_create (panel,              PANEL_BUTTON,
	      PANEL_LABEL_STRING, "Quit",
	      PANEL_NOTIFY_PROC,  xv_quit_event_proc,
	      PANEL_ITEM_X,       xv_col (panel,0),
	      PANEL_ITEM_Y,       xv_row (panel,0),
	      NULL);

   xv_create (panel,              PANEL_BUTTON,
	      PANEL_LABEL_STRING, "To First",
	      PANEL_NOTIFY_PROC,  xv_tofirst_event_proc,
	      NULL);

   xv_create (panel,              PANEL_BUTTON,
	      PANEL_LABEL_STRING, "To Last",
	      PANEL_NOTIFY_PROC,  xv_tolast_event_proc,
	      NULL);

   xv_create (panel,                PANEL_CYCLE,
	      PANEL_LABEL_STRING,   "Brief:",
	      PANEL_CHOICE_STRINGS, "No", "Yes", 0,
	      PANEL_NOTIFY_PROC,    xv_brief_event_proc,
	      PANEL_VALUE,          brief_mode,
	      NULL);

   xv_create (panel,              PANEL_SLIDER,
	      PANEL_LABEL_STRING, "Scale(%):",
	      PANEL_VALUE,        scale,
	      PANEL_MIN_VALUE,    20,
	      PANEL_MAX_VALUE,    500,
	      PANEL_SLIDER_WIDTH, 175,
	      PANEL_NOTIFY_PROC,  xv_scale_event_proc,
	      NULL);

   window_fit_height (panel);

   canvas = xv_create (frame,                 CANVAS,
		       CANVAS_WIDTH,          1,
		       CANVAS_HEIGHT,         1,
		       CANVAS_AUTO_EXPAND,    FALSE, /* we handle this */
		       CANVAS_AUTO_SHRINK,    FALSE, /* we handle this */
		       CANVAS_REPAINT_PROC,   xv_redraw_event_proc,
		       CANVAS_RETAINED,       TRUE,
		       CANVAS_X_PAINT_WINDOW, TRUE,
		       NULL);

   scrollbar_w = xv_create (canvas,                    SCROLLBAR,
			    SCROLLBAR_DIRECTION,       SCROLLBAR_HORIZONTAL,
			    SCROLLBAR_PIXELS_PER_UNIT, PIXELS_PER_UNIT,
			    SCROLLBAR_PAGE_LENGTH,     PAGE_LENGTH,
			    SCROLLBAR_SPLITTABLE,      FALSE,
			    NULL);

   scrollbar_h = xv_create (canvas,                    SCROLLBAR,
			    SCROLLBAR_DIRECTION,       SCROLLBAR_VERTICAL,
			    SCROLLBAR_PIXELS_PER_UNIT, PIXELS_PER_UNIT,
			    SCROLLBAR_PAGE_LENGTH,     PAGE_LENGTH,
			    SCROLLBAR_SPLITTABLE,      FALSE,
			    NULL);

   window_fit (canvas);

   window_fit (frame);

   center_scrollbars = TRUE;

   xv_main_loop (frame);
}

#endif /* GFX_XVIEW */
