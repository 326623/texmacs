
/******************************************************************************
* MODULE     : x_font.cpp
* DESCRIPTION: server_ps_fonts and server_tex_fonts under X windows
* COPYRIGHT  : (C) 1999  Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license and comes WITHOUT
* ANY WARRANTY WHATSOEVER. See the file $TEXMACS_PATH/LICENSE for more details.
* If you don't have this file, write to the Free Software Foundation, Inc.,
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
******************************************************************************/

#include "X/x_window.hpp"
#include "X/x_font.hpp"

/******************************************************************************
* Displaying characters
******************************************************************************/

bool char_clip= true;

#define conv(x) ((SI) (((double) (x))*(fn->unit)))

void
x_drawable_rep::draw_clipped (Pixmap pm, Pixmap bm, int w, int h, SI x, SI y) {
  int x1=cx1-ox, y1=cy2-oy, x2= cx2-ox, y2= cy1-oy;
  decode (x , y );
  decode (x1, y1);
  decode (x2, y2);
  y--; // top-left origin to bottom-left origin conversion
  int X1= max (x1- x, 0); if (X1>=w) return;
  int Y1= max (y1- y, 0); if (Y1>=h) return;
  int X2= min (x2- x, w); if (X2<0) return;
  int Y2= min (y2- y, h); if (Y2<0) return;

  if (char_clip) {
#ifdef OS_WIN32_LATER
    /*
    int X, Y, N;
    for (Y=Y1; Y<Y2; Y++) {
      for (X=X1, N=0; X<X2; X++) {
	if (XGetBitmapPixel (bm, X, Y)) N++;
	else {
	  if (N > 0)
	    XCopyArea (dpy, (Drawable)pm, win, gc, X-N, Y, N, 1, x+X1-N, y+Y1);
	  N= 0;
	}
      }
      if (N > 0)
	XCopyArea (dpy, (Drawable)pm, win, gc, X, Y-N, N, 1, x+X1-N, y+Y1);
    }
    */
    XCopyClipped (dpy, (Drawable) pm, (Drawable) bm, win, gc,
		  X, Y-N, N, 1, x+X1-N, y+Y1);
    // FIXME: Dan should write a routine for this.
#else
    XSetClipMask (dpy, gc, bm);
    XSetClipOrigin (dpy, gc, x, y);
    XCopyArea (dpy, (Drawable) pm, win, gc, X1, Y1, X2-X1, Y2-Y1, x+X1, y+Y1);
    set_clipping (cx1- ox, cy1- oy, cx2- ox, cy2- oy);
#endif
  }
  else
    XCopyArea (dpy, (Drawable) pm, win, gc, X1, Y1, X2-X1, Y2-Y1, x+X1, y+Y1);
}

void
x_drawable_rep::draw (int c, font_glyphs fng, SI x, SI y) {
  // get the pixmap
  x_character xc (c, fng, sfactor, cur_fg, cur_bg);
  Pixmap pm= (Pixmap) dis->character_pixmap [xc];
  if (pm == 0) {
    dis->prepare_color (sfactor, cur_fg, cur_bg);
    x_character col_entry (0, font_glyphs (), sfactor, cur_fg, cur_bg);
    color* cols= (color*) dis->color_scale [col_entry];
    SI xo, yo;
    glyph pre_gl= fng->get (c); if (nil (pre_gl)) return;
    glyph gl= shrink (pre_gl, sfactor, sfactor, xo, yo);
    int i, j, w= gl->width, h= gl->height;
    pm= XCreatePixmap (dis->dpy, dis->root, w, h, dis->depth);
    for (j=0; j<h; j++)
      for (i=0; i<w; i++) {
	color col= cols [gl->get_x(i,j)];
	XSetForeground (dis->dpy, dis->pixmap_gc, dis->cmap[col]);
	XDrawPoint (dis->dpy, (Drawable) pm, dis->pixmap_gc, i, j);
      }
    dis->character_pixmap (xc)= (pointer) pm;
  }

  // get the bitmap
  xc= x_character (c, fng, sfactor, 0, 0);
  Bitmap bm= (Bitmap) dis->character_bitmap [xc];
  if (bm == NULL) {
    SI xo, yo;
    glyph pre_gl= fng->get (c); if (nil (pre_gl)) return;
    glyph gl= shrink (pre_gl, sfactor, sfactor, xo, yo);
    int i, j, b, on, w= gl->width, h= gl->height;
    int byte_width= ((w-1)>>3)+1;
    char* data= new char [byte_width * h];
    for (i=0; i<(byte_width * h); i++) data[i]=0;

    for (j=0; j<h; j++)
      for (i=0; i<w; i++) {
	b = j*byte_width + (i>>3);
	on= (gl->get_x(i,j)!=0 ? 1:0);
	if (on) data[b]= data[b] | (1<<(i&7));
      }
    bm= new Bitmap_rep;
    bm->bm    = XCreateBitmapFromData (dis->dpy, dis->root, data, w, h);
    bm->width = gl->width;
    bm->height= gl->height;
    bm->xoff  = xo;
    bm->yoff  = yo;
    dis->character_bitmap (xc)= (pointer) bm;
    delete[] data;
  }

  // draw the character
  draw_clipped (pm, bm->bm, bm->width, bm->height,
		x- bm->xoff*sfactor, y+ bm->yoff*sfactor);
}

#undef conv

/******************************************************************************
* Server fonts
******************************************************************************/

static string the_default_display_font ("");
font the_default_wait_font;

void
x_display_rep::set_default_font (string name) {
  the_default_display_font= name;
}

font
x_display_rep::default_font_sub (bool tt) {
  string s= the_default_display_font;
  if (s == "") s= "ecrm11@300";
  int i, j, n= N(s);
  for (j=0; j<n; j++) if ((s[j] >= '0') && (s[j] <= '9')) break;
  string fam= s (0, j);
  for (i=j; j<n; j++) if (s[j] == '@') break;
  int sz= (j<n? as_int (s (i, j)): 10);
  if (j<n) j++;
  int dpi= (j<n? as_int (s (j, n)): 300);
  if (N(fam) >= 2) {
    string ff= fam (0, 2);
    if (((out_lan == "russian") || (out_lan == "ukrainian")) &&
	((ff == "cm") || (ff == "ec"))) {
      fam= "la" * fam (2, N(fam)); ff= "la"; if (sz<100) sz *= 100; }
    if (ff == "ec")
      return tex_ec_font (this, tt? ff * "tt": fam, sz, dpi);
    if (ff == "la")
      return tex_la_font (this, tt? ff * "tt": fam, sz, dpi, 1000);
    if (ff == "pu") tt= false;
    if ((ff == "cm") || (ff == "pn") || (ff == "pu"))
      return tex_cm_font (this, tt? ff * "tt": fam, sz, dpi);
  }
  return tex_font (this, fam, sz, dpi);
  // if (out_lan == "german") return tex_font (this, "ygoth", 14, 300, 0);
  // return tex_font (this, "rpagk", 10, 300, 0);
  // return tex_font (this, "rphvr", 10, 300, 0);
  // return ps_font (this, "b&h-lucidabright-medium-r-normal", 11, 300);
}

font
x_display_rep::default_font (bool tt) {
  font fn= default_font_sub (tt);
  the_default_wait_font= fn;
  return fn;
}

/******************************************************************************
* Loading postscript fonts
******************************************************************************/

void
x_display_rep::get_ps_char (Font fn, int c, metric& ex, glyph& gl) {
  XCharStruct xcs;
  int i1, i2, i3;
  char temp[1]; temp[0]= (char) c;

  XQueryTextExtents (dpy, fn, temp, 1, &i1, &i2, &i3, &xcs);
  ex->x1= 0;
  ex->y1= (-1-xcs.descent) * PIXEL;
  ex->x2= xcs.width * PIXEL;
  ex->y2= (-1+xcs.ascent) * PIXEL;
  ex->x3= xcs.lbearing * PIXEL;
  ex->y3= (-1-xcs.descent) * PIXEL;
  ex->x4= xcs.rbearing * PIXEL;
  ex->y4= (-1+xcs.ascent) * PIXEL;

  int w   = xcs.rbearing- xcs.lbearing;
  int h   = xcs.ascent+ xcs.descent;
  int xoff=  -xcs.lbearing;
  int yoff= h-xcs.descent ;
  if ((w == 0) || (h == 0)) return;

  Pixmap pm= XCreatePixmap (dpy, root, w, h, depth);
  XSetForeground (dpy, pixmap_gc, white);
  XFillRectangle (dpy, pm, pixmap_gc, 0, 0, w, h);
  XSetForeground (dpy, pixmap_gc, black);
  XSetFont (dpy, pixmap_gc, fn);
  XDrawString (dpy, pm, pixmap_gc, xoff, yoff, temp, 1);
  XImage* im= XGetImage (dpy, pm, 0, 0, w, h, 0xffffffff, ZPixmap);

  int i, j;
  gl= glyph (w, h, xoff, yoff);
  for (j=0; j<h; j++)
    for (i=0; i<w; i++) {
      int c = im->f.get_pixel (im, i, j);
      int on= (c == black? 1: 0);
      gl->set_x (i, j, on);
    }
  gl->lwidth= xcs.width;

  im->f.destroy_image (im);
  XFreePixmap (dpy, pm);
}

void
x_display_rep::load_system_font (string family, int size, int dpi,
				 font_metric& fnm, font_glyphs& fng)
{
  string fn_name= "ps:" * family * as_string (size) * "@" * as_string (dpi);
  if (font_metric::instances -> contains (fn_name)) {
    fnm= font_metric (fn_name);
    fng= font_glyphs (fn_name);
  }

  string name= "-" * family;
  string sz1= as_string ((size*dpi)/72);
  string sz2= as_string (10*((size*dpi)/72));
  name << "-*-" * sz1 * "-" * sz2 * "-*-*-*-*-*-*";
  if (size == 0) name= family;

  if (DEBUG_VERBOSE) cout << "TeXmacs] Loading ps font " << name << "\n";
  char* temp= as_charp (name);
  Font fn = XLoadFont (dpy, temp);
  delete[] temp;
  if (XQueryFont (dpy, fn) == NULL) {
    if (DEBUG_VERBOSE) cout << "TeXmacs] Font " << name << " not found\n";
    if (DEBUG_VERBOSE) cout << "TeXmacs] Using default font instead\n";
    fn = XLoadFont (dpy, "*");
    if (XQueryFont (dpy, fn) == NULL)
      fatal_error ("Could not load default X font", "x_ps_font_rep::prepare");
  }

  int i;
  metric* texs= new metric[256];
  glyph * gls = new glyph [256];
  for (i=0; i<=255; i++)
    get_ps_char (fn, i, texs[i], gls[i]);
  fnm= std_font_metric (fn_name, texs, 0, 255);
  fng= std_font_glyphs (fn_name, gls , 0, 255);
}

/******************************************************************************
* The implementation
******************************************************************************/

x_font_rep::x_font_rep (
  display dis, string name, string family2, int size2, int dpi2):
    font_rep (dis, name)
{
  metric ex;
  dis->load_system_font (family2, size2, dpi2, fnm, fng);

  family       = family2;
  size         = size2;
  dpi          = dpi2;

  // get main font parameters
  get_extents ("f", ex);
  y1= ex->y1;
  y2= ex->y2;
  display_size = y2-y1;
  design_size  = size << 8;

  // get character dimensions
  get_extents ("x", ex);
  yx           = ex->y4;
  get_extents ("M", ex);
  wM           = ex->x4;

  // compute other heights
  yfrac        = yx >> 1;
  ysub_lo_base = -yx/3;
  ysub_hi_lim  = (5*yx)/6;
  ysup_lo_lim  = yx/2;
  ysup_lo_base = (5*yx)/6;
  ysup_hi_lim  = yx;
  yshift       = yx/6;

  // compute other widths
  wpt          = (dpi*PIXEL)/72;
  wquad        = (wpt*design_size) >> 8;
  wline        = wquad/20;

  // get fraction bar parameters
  get_extents ("-", ex);
  yfrac= (ex->y3 + ex->y4) >> 1;

  // get space length
  get_extents (" ", ex);
  spc  = space ((3*(ex->x2-ex->x1))>>2, ex->x2-ex->x1, (ex->x2-ex->x1)<<1);
  extra= spc;
  sep  = wquad/10;

  // get_italic space
  get_extents ("f", ex);
  SI italic_spc= (ex->x4-ex->x3)-(ex->x2-ex->x1);
  slope= ((double) italic_spc) / ((double) display_size);
  if (slope<0.15) slope= 0.0;
}

void
x_font_rep::get_extents (string s, metric& ex) {
  if (N(s)==0) {
    ex->x1= ex->x3= ex->x2= ex->x4=0;
    ex->y3= ex->y1= 0; ex->y4= ex->y2= yx;
  }
  else {
    QN c= s[0];
    metric_struct* first= fnm->get (c);
    ex->x1= first->x1; ex->y1= first->y1;
    ex->x2= first->x2; ex->y2= first->y2;
    ex->x3= first->x3; ex->y3= first->y3;
    ex->x4= first->x4; ex->y4= first->y4;
    SI x= first->x2;

    int i;
    for (i=1; i<N(s); i++) {
      QN c= s[i];
      metric_struct* next= fnm->get (c);
      ex->x1= min (ex->x1, x+ next->x1); ex->y1= min (ex->y1, next->y1);
      ex->x2= max (ex->x2, x+ next->x2); ex->y2= max (ex->y2, next->y2);
      ex->x3= min (ex->x3, x+ next->x3); ex->y3= min (ex->y3, next->y3);
      ex->x4= max (ex->x4, x+ next->x4); ex->y4= max (ex->y4, next->y4);
      x += next->x2;
    }
  }
}

void
x_font_rep::get_xpositions (string s, SI* xpos) {
  register int i, n= N(s);
  if (n == 0) return;
  
  register SI x= 0;
  for (i=0; i<N(s); i++) {
    metric_struct* next= fnm->get ((QN) s[i]);
    x += next->x2;
    xpos[i+1]= x;
  }
}

void
x_font_rep::draw (ps_device dev, string s, SI x, SI y) {
  if (N(s)!=0) {
    int i;
    for (i=0; i<N(s); i++) {
      QN c= s[i];
      dev->draw (c, fng, x, y);
      metric_struct* ex= fnm->get (c);
      x += ex->x2;
    }
  }
}

glyph
x_font_rep::get_glyph (string s) {
  if (N(s)!=1) return font_rep::get_glyph (s);
  int c= ((QN) s[0]);
  glyph gl= fng->get (c);
  if (nil (gl)) return font_rep::get_glyph (s);
  return gl;
}

/******************************************************************************
* Interface
******************************************************************************/

font
x_font (display dis, string family, int size, int dpi) {
  string name= "ps:" * family * as_string (size) * "@" * as_string (dpi);
  if (font::instances -> contains (name)) return font (name);
  else return new x_font_rep (dis, name, family, size, dpi);
}
