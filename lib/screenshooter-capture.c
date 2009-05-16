/*  $Id$
 *
 *  Copyright © 2008-2009 Jérôme Guelfucci <jerome.guelfucci@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "screenshooter-capture.h"



/* Prototypes */



static GdkWindow *get_active_window (GdkScreen *screen, gboolean *needs_unref);
static GdkPixbuf *get_window_screenshot (GdkWindow *window, gboolean show_mouse);
static GdkPixbuf *get_rectangle_screenshot (void);



/* Internals */



static GdkWindow
*get_active_window (GdkScreen *screen, gboolean *needs_unref)
{
  GdkWindow *window, *window2;

  TRACE ("Get the active window");

  window = gdk_screen_get_active_window (screen);

  /* If there is no active window, we fallback to the whole screen. */
  if (G_UNLIKELY (window == NULL))
    {
      TRACE ("No active window, fallback to the root window");

      window = gdk_get_default_root_window ();
      *needs_unref = FALSE;
    }
  else if (gdk_window_get_type_hint (window) == GDK_WINDOW_TYPE_HINT_DESKTOP)
    {
      /* If the active window is the desktop, grab the whole screen */
      TRACE ("The active window is the desktop, fallback to the root window");

      g_object_unref (window);

      window = gdk_get_default_root_window ();
      *needs_unref = FALSE;
    }
  else
    {
      /* Else we find the toplevel window to grab the decorations. */
      TRACE ("Active window is a normal window, grab the toplevel window");

      window2 = gdk_window_get_toplevel (window);

      g_object_unref (window);

      window = window2;
    }

  return window;
}



static GdkPixbuf
*get_window_screenshot (GdkWindow *window, gboolean show_mouse)
{
  gint x_orig, y_orig;
  gint width, height;

  GdkPixbuf *screenshot;
  GdkWindow *root;

  GdkRectangle *rectangle = g_new0 (GdkRectangle, 1);

  /* Get the root window */
  TRACE ("Get the root window");

  root = gdk_get_default_root_window ();

  TRACE ("Get the frame extents");

  gdk_window_get_frame_extents (window, rectangle);

  /* Don't grab thing offscreen. */

  TRACE ("Make sure we don't grab things offscreen");

  x_orig = rectangle->x;
  y_orig = rectangle->y;
  width  = rectangle->width;
  height = rectangle->height;

  if (x_orig < 0)
    {
      width = width + x_orig;
      x_orig = 0;
    }

  if (y_orig < 0)
    {
      height = height + y_orig;
      y_orig = 0;
    }

  if (x_orig + width > gdk_screen_width ())
    width = gdk_screen_width () - x_orig;

  if (y_orig + height > gdk_screen_height ())
    height = gdk_screen_height () - y_orig;

  g_free (rectangle);

  /* Take the screenshot from the root GdkWindow, to grab things such as
   * menus. */

  TRACE ("Grab the screenshot");

  screenshot = gdk_pixbuf_get_from_drawable (NULL, root, NULL,
                                             x_orig, y_orig, 0, 0,
                                             width, height);

  /* Add the mouse pointer to the grabbed screenshot */

  TRACE ("Get the mouse cursor and its image");

  if (show_mouse)
    {
        GdkCursor *cursor;
        GdkPixbuf *cursor_pixbuf;

        cursor = gdk_cursor_new_for_display (gdk_display_get_default (), GDK_LEFT_PTR);
        cursor_pixbuf = gdk_cursor_get_image (cursor);

        if (G_LIKELY (cursor_pixbuf != NULL))
          {
            GdkRectangle rectangle_window, rectangle_cursor;
            gint cursorx, cursory, xhot, yhot;

            TRACE ("Get the coordinates of the cursor");

            gdk_window_get_pointer (root, &cursorx, &cursory, NULL);

            TRACE ("Get the cursor hotspot");

            sscanf (gdk_pixbuf_get_option (cursor_pixbuf, "x_hot"), "%d", &xhot);
            sscanf (gdk_pixbuf_get_option (cursor_pixbuf, "y_hot"), "%d", &yhot);

            /* rectangle_window stores the window coordinates */
            rectangle_window.x = x_orig;
            rectangle_window.y = y_orig;
            rectangle_window.width = width;
            rectangle_window.height = height;

            /* rectangle_cursor stores the cursor coordinates */
            rectangle_cursor.x = cursorx;
            rectangle_cursor.y = cursory;
            rectangle_cursor.width = gdk_pixbuf_get_width (cursor_pixbuf);
            rectangle_cursor.height = gdk_pixbuf_get_height (cursor_pixbuf);

            /* see if the pointer is inside the window */
            if (gdk_rectangle_intersect (&rectangle_window,
                                         &rectangle_cursor,
                                         &rectangle_cursor))
              {
                TRACE ("Compose the two pixbufs");

                gdk_pixbuf_composite (cursor_pixbuf, screenshot,
                                      cursorx - x_orig -xhot, cursory - y_orig -yhot,
                                      rectangle_cursor.width, rectangle_cursor.height,
                                      cursorx - x_orig - xhot, cursory - y_orig -yhot,
                                      1.0, 1.0,
                                      GDK_INTERP_BILINEAR,
                                      255);
              }

            g_object_unref (cursor_pixbuf);
          }

        gdk_cursor_unref (cursor);
    }

  return screenshot;
}



static GdkPixbuf
*get_rectangle_screenshot (void)
{
  GdkPixbuf *screenshot = NULL;

  /* Get root window */
  TRACE ("Get the root window");

  GdkWindow *root_window =  gdk_get_default_root_window ();

  GdkGCValues gc_values;
  GdkGC *gc;
  GdkGrabStatus grabstatus_mouse, grabstatus_keyboard;

  GdkGCValuesMask values_mask =
    GDK_GC_FUNCTION | GDK_GC_FILL	| GDK_GC_CLIP_MASK |
    GDK_GC_SUBWINDOW | GDK_GC_CLIP_X_ORIGIN | GDK_GC_CLIP_Y_ORIGIN |
    GDK_GC_EXPOSURES | GDK_GC_LINE_WIDTH | GDK_GC_LINE_STYLE |
    GDK_GC_CAP_STYLE | GDK_GC_JOIN_STYLE;

  GdkColor gc_white = {0, 65535, 65535, 65535};
  GdkColor gc_black = {0, 0, 0, 0};

  GdkEventMask mask =
    GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK |
    GDK_BUTTON_RELEASE_MASK;
  GdkCursor *xhair_cursor = gdk_cursor_new (GDK_CROSSHAIR);

  gboolean pressed = FALSE;
  gboolean done = FALSE;
  gboolean cancelled = FALSE;
  gint x, y, w, h;

  /*Set up graphics context for a XOR rectangle that will be drawn as
   * the user drags the mouse */
  TRACE ("Initialize the graphics context");

  gc_values.function           = GDK_XOR;
  gc_values.line_width         = 2;
  gc_values.line_style         = GDK_LINE_ON_OFF_DASH;
  gc_values.fill               = GDK_SOLID;
  gc_values.cap_style          = GDK_CAP_BUTT;
  gc_values.join_style         = GDK_JOIN_MITER;
  gc_values.graphics_exposures = FALSE;
  gc_values.clip_x_origin      = 0;
  gc_values.clip_y_origin      = 0;
  gc_values.clip_mask          = None;
  gc_values.subwindow_mode     = GDK_INCLUDE_INFERIORS;

  gc = gdk_gc_new_with_values (root_window, &gc_values, values_mask);
  gdk_gc_set_rgb_fg_color (gc, &gc_white);
  gdk_gc_set_rgb_bg_color (gc, &gc_black);

  /* Change cursor to cross-hair */
  TRACE ("Set the cursor");

  grabstatus_mouse =
    gdk_pointer_grab (root_window, FALSE, mask, NULL, xhair_cursor, GDK_CURRENT_TIME);

  grabstatus_keyboard = gdk_keyboard_grab (root_window, FALSE, GDK_CURRENT_TIME);

  while (!done && grabstatus_mouse == GDK_GRAB_SUCCESS
               && grabstatus_keyboard == GDK_GRAB_SUCCESS)
    {
      gint x1, y1, x2, y2;
      GdkEvent *event;

      event = gdk_event_get ();

      if (event == NULL)
        continue;

      switch (event->type)
        {
          /* Start dragging the rectangle out */

          case GDK_BUTTON_PRESS:

            TRACE ("Start dragging the rectangle");

            x = x2 = x1 = event->button.x;
            y = y2 = y1 = event->button.y;
            w = 0; h = 0;
            pressed = TRUE;
            break;

          /* Finish dragging the rectangle out */
          case GDK_BUTTON_RELEASE:
            if (pressed)
              {
                if (w > 0 && h > 0)
                  {
                    /* Remove the rectangle drawn previously */

                    TRACE ("Remove the rectangle drawn previously");

                    gdk_draw_rectangle (root_window,
                                        gc,
                                        FALSE,
                                        x, y, w, h);
                    done = TRUE;
                  }
                else
                  {
                    /* The user has not dragged the mouse, start again */

                    TRACE ("Mouse was not dragged, start again");

                    pressed = FALSE;
                  }
              }
          break;

          /* The user is moving the mouse */
          case GDK_MOTION_NOTIFY:
            if (pressed)
              {
                TRACE ("Mouse is moving");

                if (w > 0 && h > 0)
                  {
                    /* Remove the rectangle drawn previously */

                     TRACE ("Remove the rectangle drawn previously");

                     gdk_draw_rectangle (root_window,
                                         gc,
                                         FALSE,
                                         x, y, w, h);
                  }

                x2 = event->motion.x;
                y2 = event->motion.y;

                x = MIN (x1, x2);
                y = MIN (y1, y2);
                w = ABS (x2 - x1);
                h = ABS (y2 - y1);

                /* Draw  the rectangle as the user drags  the mouse */

                TRACE ("Draw the new rectangle");

                if (w > 0 && h > 0)
                  gdk_draw_rectangle (root_window,
                                      gc,
                                      FALSE,
                                      x, y, w, h);

              }
            break;

          case GDK_KEY_PRESS:
            if (event->key.keyval == GDK_Escape)
              {
                TRACE ("Escape key was pressed, cancel the screenshot.");

                if (pressed)
                  {
                    if (w > 0 && h > 0)
                      {
                        /* Remove the rectangle drawn previously */

                         TRACE ("Remove the rectangle drawn previously");

                         gdk_draw_rectangle (root_window,
                                             gc,
                                             FALSE,
                                             x, y, w, h);
                      }
                  }

                done = TRUE;
                cancelled = TRUE;
              }

            break;

          default:
            break;
        }

      gdk_event_free (event);
    }

  if (grabstatus_mouse == GDK_GRAB_SUCCESS)
    {
      TRACE ("Ungrab the pointer");

      gdk_pointer_ungrab(GDK_CURRENT_TIME);
    }

  if (grabstatus_keyboard == GDK_GRAB_SUCCESS)
    {
      TRACE ("Ungrab the keyboard");

      gdk_keyboard_ungrab (GDK_CURRENT_TIME);
    }

  /* Get the screenshot's pixbuf */

  if (G_LIKELY (!cancelled))
    {
      TRACE ("Get the pixbuf for the screenshot");

      screenshot =
        gdk_pixbuf_get_from_drawable (NULL, root_window, NULL, x, y, 0, 0, w, h);
    }

  if (G_LIKELY (gc != NULL))
    g_object_unref (gc);

  gdk_cursor_unref (xhair_cursor);

  return screenshot;
}



/* Public */



/**
 * screenshooter_take_screenshot:
 * @region: the region to be screenshoted. It can be FULLSCREEN, ACTIVE_WINDOW or SELECT.
 * @delay: the delay before the screenshot is taken, in seconds.
 * @mouse: whether the mouse pointer should be displayed on the screenshot.
 *
 * Takes a screenshot with the given options. If @region is FULLSCREEN, the screenshot
 * is taken after @delay seconds. If @region is ACTIVE_WINDOW, a delay of @delay seconds
 * ellapses, then the active window is detected and captured. If @region is SELECT, @delay
 * will be ignored and the user will have to select a portion of the screen with the
 * mouse.
 *
 * @show_mouse is only taken into account when @region is FULLSCREEN or ACTIVE_WINDOW.
 *
 * Return value: a #GdkPixbuf containing the screenshot or %NULL (if @region is SELECT,
 * the user can cancel the operation).
 **/
GdkPixbuf *screenshooter_take_screenshot (gint region, gint delay, gboolean show_mouse)
{
  GdkPixbuf *screenshot = NULL;
  GdkWindow *window = NULL;
  GdkScreen *screen;

  /* gdk_get_default_root_window () does not need to be unrefed,
   * needs_unref enables us to unref *window only if a non default
   * window has been grabbed. */
  gboolean needs_unref = TRUE;

  /* Get the screen on which the screenshot should be taken */
  screen = gdk_screen_get_default ();

  /* wait for n=delay seconds */
  if (region != SELECT)
    sleep (delay);

  /* Get the window/desktop we want to screenshot*/
  if (region == FULLSCREEN)
    {
      TRACE ("We grab the entire screen");

      window = gdk_get_default_root_window ();
      needs_unref = FALSE;
    }
  else if (region == ACTIVE_WINDOW)
    {
      TRACE ("We grab the active window");

      window = get_active_window (screen, &needs_unref);
    }

  if (region == FULLSCREEN || region == ACTIVE_WINDOW)
    {
      TRACE ("Get the screenshot of the given window");

      screenshot = get_window_screenshot (window, show_mouse);

      if (needs_unref)
	      g_object_unref (window);
    }
  else if (region == SELECT)
    {
      TRACE ("Let the user select the region to screenshot");

      screenshot = get_rectangle_screenshot ();
    }


	return screenshot;
}
