#include "stdafx.h"

#include <Math.h>

// Clip Encode (for ClipLine,etc.)
static int ClipEncode(Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom);
int CompareInt(const void *a, const void *b);

static int *PolyInts = NULL;
static int PolyAllocated = 0;


#define clip_xmin(surface) surface->clip_rect.x
#define clip_xmax(surface) surface->clip_rect.x+surface->clip_rect.w-1
#define clip_ymin(surface) surface->clip_rect.y
#define clip_ymax(surface) surface->clip_rect.y+surface->clip_rect.h-1

#ifndef M_PI
#define M_PI	3.141592654
#endif

#ifndef ABS
#define ABS(a) (((a)<0) ? -(a) : (a))
#endif

// Rotozoom Things

#define SMOOTHING_OFF		0
#define SMOOTHING_ON		1

    typedef struct tColorRGBA 
	{
		Uint8 r;
		Uint8 g;
		Uint8 b;
		Uint8 a;
    }
	tColorRGBA;

    typedef struct tColorY 
	{
		Uint8 y;
    } 
	tColorY;

//#################
int CompareInt(const void *a, const void *b)
{
    return (*(const int *) a) - (*(const int *) b);
}

// ----- Pixel - fast, no blending, no locking, clipping 
int FastPixelColorNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color) // Fast Pixel Set V.1.0
{
    int bpp;
    Uint8 *p;

    /*
     * Honor clipping setup at pixel level 
     */
    if ((x >= clip_xmin(dst)) && (x <= clip_xmax(dst)) && (y >= clip_ymin(dst)) && (y <= clip_ymax(dst))) {

	/*
	 * Get destination format 
	 */
	bpp = dst->format->BytesPerPixel;
	p = (Uint8 *) dst->pixels + y * dst->pitch + x * bpp;
	switch (bpp) {
	case 1:
	    *p = color;
	    break;
	case 2:
	    *(Uint16 *) p = color;
	    break;
	case 3:
	    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		p[0] = (color >> 16) & 0xff;
		p[1] = (color >> 8) & 0xff;
		p[2] = color & 0xff;
	    } else {
		p[0] = color & 0xff;
		p[1] = (color >> 8) & 0xff;
		p[2] = (color >> 16) & 0xff;
	    }
	    break;
	case 4:
	    *(Uint32 *) p = color;
	    break;
	}			/* switch */


    }

    return (0);
}

// Pixel mit Lock und Clipping
int PixelColor(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color) // Pixel mit Lock V.1.0
{
    Uint8 alpha;
    Uint32 mcolor;
    int result = 0;

    /*
     * Lock the surface 
     */
    if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	    return (-1);
	}
    }

    /*
     * Setup color 
     */
    alpha = color & 0x000000ff;
    mcolor =
	SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
		    (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

    /*
     * Draw 
     */
    result = PutPixelAlpha(dst, x, y, mcolor, alpha);

    /*
     * Unlock the surface 
     */
    if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
    }

    return (result);
}

// Pixel mit Clipping
int PixelColorNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color) // Standard Pixel
{
    Uint8 alpha;
    Uint32 mcolor;
    int result = 0;

    /*
     * Setup color 
     */
    alpha = color & 0x000000ff;
    mcolor =
	SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
		    (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

    /*
     * Draw 
     */
    result = PutPixelAlpha(dst, x, y, mcolor, alpha);

    return (result);
}

// Pixel mit Alpha und Clipping und Lock
int PutPixelAlpha(SDL_Surface * surface, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha) // Pixel mit Alpha V.1.0
{
    Uint32 Rmask = surface->format->Rmask, Gmask =
	surface->format->Gmask, Bmask = surface->format->Bmask, Amask = surface->format->Amask;
    Uint32 R, G, B, A = 0;

    if (x >= clip_xmin(surface) && x <= clip_xmax(surface)
	&& y >= clip_ymin(surface) && y <= clip_ymax(surface)) {

	switch (surface->format->BytesPerPixel) {
	case 1:{		/* Assuming 8-bpp */
		if (alpha == 255) {
		    *((Uint8 *) surface->pixels + y * surface->pitch + x) = color;
		} else {
		    Uint8 *pixel = (Uint8 *) surface->pixels + y * surface->pitch + x;

		    Uint8 dR = surface->format->palette->colors[*pixel].r;
		    Uint8 dG = surface->format->palette->colors[*pixel].g;
		    Uint8 dB = surface->format->palette->colors[*pixel].b;
		    Uint8 sR = surface->format->palette->colors[color].r;
		    Uint8 sG = surface->format->palette->colors[color].g;
		    Uint8 sB = surface->format->palette->colors[color].b;

		    dR = dR + ((sR - dR) * alpha >> 8);
		    dG = dG + ((sG - dG) * alpha >> 8);
		    dB = dB + ((sB - dB) * alpha >> 8);

		    *pixel = SDL_MapRGB(surface->format, dR, dG, dB);
		}
	    }
	    break;

	case 2:{		/* Probably 15-bpp or 16-bpp */
		if (alpha == 255) {
		    *((Uint16 *) surface->pixels + y * surface->pitch / 2 + x) = color;
		} else {
		    Uint16 *pixel = (Uint16 *) surface->pixels + y * surface->pitch / 2 + x;
		    Uint32 dc = *pixel;

		    R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
		    G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
		    B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
		    if (Amask)
			A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;

		    *pixel = R | G | B | A;
		}
	    }
	    break;

	case 3:{		/* Slow 24-bpp mode, usually not used */
		Uint8 *pix = (Uint8 *) surface->pixels + y * surface->pitch + x * 3;
		Uint8 rshift8 = surface->format->Rshift / 8;
		Uint8 gshift8 = surface->format->Gshift / 8;
		Uint8 bshift8 = surface->format->Bshift / 8;
		Uint8 ashift8 = surface->format->Ashift / 8;


		if (alpha == 255) {
		    *(pix + rshift8) = color >> surface->format->Rshift;
		    *(pix + gshift8) = color >> surface->format->Gshift;
		    *(pix + bshift8) = color >> surface->format->Bshift;
		    *(pix + ashift8) = color >> surface->format->Ashift;
		} else {
		    Uint8 dR, dG, dB, dA = 0;
		    Uint8 sR, sG, sB, sA = 0;

		    pix = (Uint8 *) surface->pixels + y * surface->pitch + x * 3;

		    dR = *((pix) + rshift8);
		    dG = *((pix) + gshift8);
		    dB = *((pix) + bshift8);
		    dA = *((pix) + ashift8);

		    sR = (color >> surface->format->Rshift) & 0xff;
		    sG = (color >> surface->format->Gshift) & 0xff;
		    sB = (color >> surface->format->Bshift) & 0xff;
		    sA = (color >> surface->format->Ashift) & 0xff;

		    dR = dR + ((sR - dR) * alpha >> 8);
		    dG = dG + ((sG - dG) * alpha >> 8);
		    dB = dB + ((sB - dB) * alpha >> 8);
		    dA = dA + ((sA - dA) * alpha >> 8);

		    *((pix) + rshift8) = dR;
		    *((pix) + gshift8) = dG;
		    *((pix) + bshift8) = dB;
		    *((pix) + ashift8) = dA;
		}
	    }
	    break;

	case 4:{		/* Probably 32-bpp */
		if (alpha == 255) {
		    *((Uint32 *) surface->pixels + y * surface->pitch / 4 + x) = color;
		} else {
		    Uint32 *pixel = (Uint32 *) surface->pixels + y * surface->pitch / 4 + x;
		    Uint32 dc = *pixel;

		    R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
		    G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
		    B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
		    if (Amask)
			A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;

		    *pixel = R | G | B | A;
		}
	    }
	    break;
	}
    }

    return (0);
}
// Linie mit Clipping und Lock
int LineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) // Linie mit Clipping und Lock V.1.0
{
    int pixx, pixy;
    int x, y;
    int dx, dy;
    int ax, ay;
    int sx, sy;
    int swaptmp;
    Uint8 *pixel;
    Uint8 *colorptr;

    /*
     * Clip line and test if we have to draw 
     */
    if (!(ClipLine(dst, &x1, &y1, &x2, &y2))) {
	return (0);
    }

    /*
     * Test for special cases of straight lines or single point 
     */
    if (x1 == x2) {
	if (y1 < y2) {
	    return (VLineColor(dst, x1, y1, y2, color));
	} else if (y1 > y2) {
	    return (VLineColor(dst, x1, y2, y1, color));
	} else {
	    return (PixelColor(dst, x1, y1, color));
	}
    }
    if (y1 == y2) {
	if (x1 < x2) {
	    return (HLineColor(dst, x1, x2, y1, color));
	} else if (x1 > x2) {
	    return (HLineColor(dst, x2, x1, y1, color));
	}
    }

    /*
     * Variable setup 
     */
    dx = x2 - x1;
    dy = y2 - y1;
    sx = (dx >= 0) ? 1 : -1;
    sy = (dy >= 0) ? 1 : -1;

    /* Lock surface */
    if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	    return (-1);
	}
    }

    /*
     * Check for alpha blending 
     */
    if ((color & 255) == 255) {

	/*
	 * No alpha blending - use fast pixel routines 
	 */

	/*
	 * Setup color 
	 */
	colorptr = (Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
	} else {
	    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	}

	/*
	 * More variable setup 
	 */
	dx = sx * dx + 1;
	dy = sy * dy + 1;
	pixx = dst->format->BytesPerPixel;
	pixy = dst->pitch;
	pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y1;
	pixx *= sx;
	pixy *= sy;
	if (dx < dy) {
	    swaptmp = dx;
	    dx = dy;
	    dy = swaptmp;
	    swaptmp = pixx;
	    pixx = pixy;
	    pixy = swaptmp;
	}

	/*
	 * Draw 
	 */
	x = 0;
	y = 0;
	switch (dst->format->BytesPerPixel) {
	case 1:
	    for (; x < dx; x++, pixel += pixx) {
		*pixel = color;
		y += dy;
		if (y >= dx) {
		    y -= dx;
		    pixel += pixy;
		}
	    }
	    break;
	case 2:
	    for (; x < dx; x++, pixel += pixx) {
		*(Uint16 *) pixel = color;
		y += dy;
		if (y >= dx) {
		    y -= dx;
		    pixel += pixy;
		}
	    }
	    break;
	case 3:
	    for (; x < dx; x++, pixel += pixx) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		    pixel[0] = (color >> 16) & 0xff;
		    pixel[1] = (color >> 8) & 0xff;
		    pixel[2] = color & 0xff;
		} else {
		    pixel[0] = color & 0xff;
		    pixel[1] = (color >> 8) & 0xff;
		    pixel[2] = (color >> 16) & 0xff;
		}
		y += dy;
		if (y >= dx) {
		    y -= dx;
		    pixel += pixy;
		}
	    }
	    break;
	default:		/* case 4 */
	    for (; x < dx; x++, pixel += pixx) {
		*(Uint32 *) pixel = color;
		y += dy;
		if (y >= dx) {
		    y -= dx;
		    pixel += pixy;
		}
	    }
	    break;
	}

    } else {

	/*
	 * Alpha blending required - use single-pixel blits 
	 */

	ax = ABS(dx) << 1;
	ay = ABS(dy) << 1;
	x = x1;
	y = y1;
	if (ax > ay) {
	    int d = ay - (ax >> 1);

	    while (x != x2) {
		PixelColorNolock (dst, x, y, color);
		if (d > 0 || (d == 0 && sx == 1)) {
		    y += sy;
		    d -= ax;
		}
		x += sx;
		d += ay;
	    }
	} else {
	    int d = ax - (ay >> 1);

	    while (y != y2) {
		PixelColorNolock (dst, x, y, color);
		if (d > 0 || ((d == 0) && (sy == 1))) {
		    x += sx;
		    d -= ay;
		}
		y += sy;
		d += ax;
	    }
	}
	PixelColorNolock (dst, x, y, color);

    }

    /* Unlock surface */
    if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
    }

    return (0);
}

// Linie mit Alpha und Clipping und Lock
int LineRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) // Linie mit Alpha und Clipping und Lock V.1.0
{
    /*
     * Draw 
     */
    return (LineColor(dst, x1, y1, x2, y2, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

#define CLIP_LEFT_EDGE   0x1
#define CLIP_RIGHT_EDGE  0x2
#define CLIP_BOTTOM_EDGE 0x4
#define CLIP_TOP_EDGE    0x8
#define CLIP_INSIDE(a)   (!a)
#define CLIP_REJECT(a,b) (a&b)
#define CLIP_ACCEPT(a,b) (!(a|b))

// Clip Encode
static int ClipEncode(Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom) // Clip Encode V.1.0
{
    int code = 0;

    if (x < left) {
	code |= CLIP_LEFT_EDGE;
    } else if (x > right) {
	code |= CLIP_RIGHT_EDGE;
    }
    if (y < top) {
	code |= CLIP_TOP_EDGE;
    } else if (y > bottom) {
	code |= CLIP_BOTTOM_EDGE;
    }
    return code;
}

// Linie mit Clipping
static int ClipLine(SDL_Surface * dst, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2) // Liune mit Clipping V.1.0
{
    Sint16 left, right, top, bottom;
    int code1, code2;
    int draw = 0;
    Sint16 swaptmp;
    float m;

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    while (1) {
	code1 = ClipEncode(*x1, *y1, left, top, right, bottom);
	code2 = ClipEncode(*x2, *y2, left, top, right, bottom);
	if (CLIP_ACCEPT(code1, code2)) {
	    draw = 1;
	    break;
	} else if (CLIP_REJECT(code1, code2))
	    break;
	else {
	    if (CLIP_INSIDE(code1)) {
		swaptmp = *x2;
		*x2 = *x1;
		*x1 = swaptmp;
		swaptmp = *y2;
		*y2 = *y1;
		*y1 = swaptmp;
		swaptmp = code2;
		code2 = code1;
		code1 = swaptmp;
	    }
	    if (*x2 != *x1) {
		m = (*y2 - *y1) / (float) (*x2 - *x1);
	    } else {
		m = 1.0f;
	    }
	    if (code1 & CLIP_LEFT_EDGE) {
		*y1 += (Sint16) ((left - *x1) * m);
		*x1 = left;
	    } else if (code1 & CLIP_RIGHT_EDGE) {
		*y1 += (Sint16) ((right - *x1) * m);
		*x1 = right;
	    } else if (code1 & CLIP_BOTTOM_EDGE) {
		if (*x2 != *x1) {
		    *x1 += (Sint16) ((bottom - *y1) / m);
		}
		*y1 = bottom;
	    } else if (code1 & CLIP_TOP_EDGE) {
		if (*x2 != *x1) {
		    *x1 += (Sint16) ((top - *y1) / m);
		}
		*y1 = top;
	    }
	}
    }

    return draw;
}

// Horizontale Linie mit Clipping und Lock
int HLineColor(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color) // Horizontale Line with Lock and Clipping V.1.0
{
    Sint16 left, right, top, bottom;
    Uint8 *pixel, *pixellast;
    int dx;
    int pixx, pixy;
    Sint16 w;
    Sint16 xtmp;
    int result = -1;
    Uint8 *colorptr;

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Check visibility of hline 
     */
    if ((x1<left) && (x2<left)) {
     return(0);
    }
    if ((x1>right) && (x2>right)) {
     return(0);
    }
    if ((y<top) || (y>bottom)) {
     return (0);
    }

    /*
     * Clip x 
     */
    if (x1 < left) {
	x1 = left;
    }
    if (x2 > right) {
	x2 = right;
    }

    /*
     * Swap x1, x2 if required 
     */
    if (x1 > x2) {
	xtmp = x1;
	x1 = x2;
	x2 = xtmp;
    }

    /*
     * Calculate width 
     */
    w = x2 - x1;

    /*
     * Sanity check on width 
     */
    if (w < 0) {
	return (0);
    }

    /*
     * Alpha check 
     */
    if ((color & 255) == 255) {

	/*
	 * No alpha-blending required 
	 */

	/*
	 * Setup color 
	 */
	colorptr = (Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
	} else {
	    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	}

	/*
	 * Lock surface 
	 */
	SDL_LockSurface(dst);

	/*
	 * More variable setup 
	 */
	dx = w;
	pixx = dst->format->BytesPerPixel;
	pixy = dst->pitch;
	pixel = ((Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y;

	/*
	 * Draw 
	 */
	switch (dst->format->BytesPerPixel) {
	case 1:
	    memset(pixel, color, dx);
	    break;
	case 2:
	    pixellast = pixel + dx + dx;
	    for (; pixel <= pixellast; pixel += pixx) {
		*(Uint16 *) pixel = color;
	    }
	    break;
	case 3:
	    pixellast = pixel + dx + dx + dx;
	    for (; pixel <= pixellast; pixel += pixx) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		    pixel[0] = (color >> 16) & 0xff;
		    pixel[1] = (color >> 8) & 0xff;
		    pixel[2] = color & 0xff;
		} else {
		    pixel[0] = color & 0xff;
		    pixel[1] = (color >> 8) & 0xff;
		    pixel[2] = (color >> 16) & 0xff;
		}
	    }
	    break;
	default:		/* case 4 */
	    dx = dx + dx;
	    pixellast = pixel + dx + dx;
	    for (; pixel <= pixellast; pixel += pixx) {
		*(Uint32 *) pixel = color;
	    }
	    break;
	}

	/*
	 * Unlock surface 
	 */
	SDL_UnlockSurface(dst);

	/*
	 * Set result code 
	 */
	result = 0;

    } else {

	/*
	 * Alpha blending blit 
	 */

	result = HLineAlpha(dst, x1, x1 + w, y, color);

    }

    return (result);
}
// Vertikale Linie mit Clipping und Lock
int VLineColor(SDL_Surface * dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color) // V.1.0
{
    Sint16 left, right, top, bottom;
    Uint8 *pixel, *pixellast;
    int dy;
    int pixx, pixy;
    Sint16 h;
    Sint16 ytmp;
    int result = -1;
    Uint8 *colorptr;

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Check visibility of vline 
     */
    if ((x<left) || (x>right)) {
     return (0);
    }
    if ((y1<top) && (y2<top)) {
     return(0);
    }
    if ((y1>bottom) && (y2>bottom)) {
     return(0);
    }

    /*
     * Clip y 
     */
    if (y1 < top) {
	y1 = top;
    }
    if (y2 > bottom) {
	y2 = bottom;
    }

    /*
     * Swap y1, y2 if required 
     */
    if (y1 > y2) {
	ytmp = y1;
	y1 = y2;
	y2 = ytmp;
    }

    /*
     * Calculate height 
     */
    h = y2 - y1;

    /*
     * Sanity check on height 
     */
    if (h < 0) {
	return (0);
    }

    /*
     * Alpha check 
     */
    if ((color & 255) == 255) {

	/*
	 * No alpha-blending required 
	 */

	/*
	 * Setup color 
	 */
	colorptr = (Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
	} else {
	    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	}

	/*
	 * Lock surface 
	 */
	SDL_LockSurface(dst);

	/*
	 * More variable setup 
	 */
	dy = h;
	pixx = dst->format->BytesPerPixel;
	pixy = dst->pitch;
	pixel = ((Uint8 *) dst->pixels) + pixx * (int) x + pixy * (int) y1;
	pixellast = pixel + pixy * dy;

	/*
	 * Draw 
	 */
	switch (dst->format->BytesPerPixel) {
	case 1:
	    for (; pixel <= pixellast; pixel += pixy) {
		*(Uint8 *) pixel = color;
	    }
	    break;
	case 2:
	    for (; pixel <= pixellast; pixel += pixy) {
		*(Uint16 *) pixel = color;
	    }
	    break;
	case 3:
	    for (; pixel <= pixellast; pixel += pixy) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		    pixel[0] = (color >> 16) & 0xff;
		    pixel[1] = (color >> 8) & 0xff;
		    pixel[2] = color & 0xff;
		} else {
		    pixel[0] = color & 0xff;
		    pixel[1] = (color >> 8) & 0xff;
		    pixel[2] = (color >> 16) & 0xff;
		}
	    }
	    break;
	default:		/* case 4 */
	    for (; pixel <= pixellast; pixel += pixy) {
		*(Uint32 *) pixel = color;
	    }
	    break;
	}

	/*
	 * Unlock surface 
	 */
	SDL_UnlockSurface(dst);

	/*
	 * Set result code 
	 */
	result = 0;

    } else {

	/*
	 * Alpha blending blit 
	 */

	result = VLineAlpha(dst, x, y1, y1 + h, color);

    }

    return (result);
}

/* Draw horizontal line with alpha enabled from RGBA color */
int HLineAlpha(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color) // Linie mit Alpha V.1.0
{
    return (FilledRectAlphaRGBA(dst, x1, y, x2, y, color));
}

/* Draw vertical line with alpha enabled from RGBA color */
int VLineAlpha(SDL_Surface * dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color)
{
    return (FilledRectAlphaRGBA(dst, x, y1, x, y2, color));
}

// Ungefülltes Rechteck mit Alpha
int RectangleRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    /*
     * Draw 
     */
    return (RectangleColor
	    (dst, x1, y1, x2, y2, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

// Ungefülltes Rechteck
int RectangleColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) // Rechteck V.1.0
{
    int result;
    Sint16 w, h, xtmp, ytmp;

    /*
     * Swap x1, x2 if required 
     */
    if (x1 > x2) {
	xtmp = x1;
	x1 = x2;
	x2 = xtmp;
    }

    /*
     * Swap y1, y2 if required 
     */
    if (y1 > y2) {
	ytmp = y1;
	y1 = y2;
	y2 = ytmp;
    }

    /*
     * Calculate width&height 
     */
    w = x2 - x1;
    h = y2 - y1;

    /*
     * Sanity check 
     */
    if ((w < 0) || (h < 0)) {
	return (0);
    }

    /*
     * Test for special cases of straight lines or single point 
     */
    if (x1 == x2) {
	if (y1 == y2) {
	    return (PixelColor(dst, x1, y1, color));
	} else {
	    return (VLineColor(dst, x1, y1, y2, color));
	}
    } else {
	if (y1 == y2) {
	    return (HLineColor(dst, x1, x2, y1, color));
	}
    }

    /*
     * Draw rectangle 
     */
    result = 0;
    result |= HLineColor(dst, x1, x2, y1, color);
    result |= HLineColor(dst, x1, x2, y2, color);
    y1 += 1;
    y2 -= 1;
    if (y1<=y2) {
     result |= HLineColor(dst, x1, y1, y2, color);
     result |= HLineColor(dst, x2, y1, y2, color);
    }
    return (result);

}

/* Filled rectangle with alpha blending, color in destination format */
int FilledRectAlpha(SDL_Surface * surface, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, Uint8 alpha) // Rectangle with Alpha V.1.0
{
    Uint32 Rmask = surface->format->Rmask, Gmask =
	surface->format->Gmask, Bmask = surface->format->Bmask, Amask = surface->format->Amask;
    Uint32 R, G, B, A = 0;
    Sint16 x, y;

    switch (surface->format->BytesPerPixel) {
    case 1:{			/* Assuming 8-bpp */
	    Uint8 *row, *pixel;
	    Uint8 dR, dG, dB;

	    Uint8 sR = surface->format->palette->colors[color].r;
	    Uint8 sG = surface->format->palette->colors[color].g;
	    Uint8 sB = surface->format->palette->colors[color].b;

	    for (y = y1; y <= y2; y++) {
		row = (Uint8 *) surface->pixels + y * surface->pitch;
		for (x = x1; x <= x2; x++) {
		    pixel = row + x;

		    dR = surface->format->palette->colors[*pixel].r;
		    dG = surface->format->palette->colors[*pixel].g;
		    dB = surface->format->palette->colors[*pixel].b;

		    dR = dR + ((sR - dR) * alpha >> 8);
		    dG = dG + ((sG - dG) * alpha >> 8);
		    dB = dB + ((sB - dB) * alpha >> 8);

		    *pixel = SDL_MapRGB(surface->format, dR, dG, dB);
		}
	    }
	}
	break;

    case 2:{			/* Probably 15-bpp or 16-bpp */
	    Uint16 *row, *pixel;
	    Uint32 dR = (color & Rmask), dG = (color & Gmask), dB = (color & Bmask), dA = (color & Amask);

	    for (y = y1; y <= y2; y++) {
		row = (Uint16 *) surface->pixels + y * surface->pitch / 2;
		for (x = x1; x <= x2; x++) {
		    pixel = row + x;

		    R = ((*pixel & Rmask) + ((dR - (*pixel & Rmask)) * alpha >> 8)) & Rmask;
		    G = ((*pixel & Gmask) + ((dG - (*pixel & Gmask)) * alpha >> 8)) & Gmask;
		    B = ((*pixel & Bmask) + ((dB - (*pixel & Bmask)) * alpha >> 8)) & Bmask;
		    if (Amask)
			A = ((*pixel & Amask) + ((dA - (*pixel & Amask)) * alpha >> 8)) & Amask;

		    *pixel = R | G | B | A;
		}
	    }
	}
	break;

    case 3:{			/* Slow 24-bpp mode, usually not used */
	    Uint8 *row, *pix;
	    Uint8 dR, dG, dB, dA;
	    Uint8 rshift8 = surface->format->Rshift / 8;
	    Uint8 gshift8 = surface->format->Gshift / 8;
	    Uint8 bshift8 = surface->format->Bshift / 8;
	    Uint8 ashift8 = surface->format->Ashift / 8;

	    Uint8 sR = (color >> surface->format->Rshift) & 0xff;
	    Uint8 sG = (color >> surface->format->Gshift) & 0xff;
	    Uint8 sB = (color >> surface->format->Bshift) & 0xff;
	    Uint8 sA = (color >> surface->format->Ashift) & 0xff;

	    for (y = y1; y <= y2; y++) {
		row = (Uint8 *) surface->pixels + y * surface->pitch;
		for (x = x1; x <= x2; x++) {
		    pix = row + x * 3;

		    dR = *((pix) + rshift8);
		    dG = *((pix) + gshift8);
		    dB = *((pix) + bshift8);
		    dA = *((pix) + ashift8);

		    dR = dR + ((sR - dR) * alpha >> 8);
		    dG = dG + ((sG - dG) * alpha >> 8);
		    dB = dB + ((sB - dB) * alpha >> 8);
		    dA = dA + ((sA - dA) * alpha >> 8);

		    *((pix) + rshift8) = dR;
		    *((pix) + gshift8) = dG;
		    *((pix) + bshift8) = dB;
		    *((pix) + ashift8) = dA;
		}
	    }

	}
	break;

    case 4:{			/* Probably 32-bpp */
	    Uint32 *row, *pixel;
	    Uint32 dR = (color & Rmask), dG = (color & Gmask), dB = (color & Bmask), dA = (color & Amask);

	    for (y = y1; y <= y2; y++) {
		row = (Uint32 *) surface->pixels + y * surface->pitch / 4;
		for (x = x1; x <= x2; x++) {
		    pixel = row + x;

		    R = ((*pixel & Rmask) + ((dR - (*pixel & Rmask)) * alpha >> 8)) & Rmask;
		    G = ((*pixel & Gmask) + ((dG - (*pixel & Gmask)) * alpha >> 8)) & Gmask;
		    B = ((*pixel & Bmask) + ((dB - (*pixel & Bmask)) * alpha >> 8)) & Bmask;
		    if (Amask)
			A = ((*pixel & Amask) + ((dA - (*pixel & Amask)) * alpha >> 8)) & Amask;

		    *pixel = R | G | B | A;
		}
	    }
	}
	break;
    }

    return (0);
}
/* Draw rectangle with alpha enabled from RGBA color. */

int FilledRectAlphaRGBA(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) // Gefüllter Kreis mit Alpha vom RGBA V.1.0
{
    Uint8 alpha;
    Uint32 mcolor;
    int result = 0;

    /*
     * Lock the surface 
     */
    if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	    return (-1);
	}
    }

    /*
     * Setup color 
     */
    alpha = color & 0x000000ff;
    mcolor =
	SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
		    (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

    /*
     * Draw 
     */
    result = FilledRectAlpha(dst, x1, y1, x2, y2, mcolor, alpha);

    /*
     * Unlock the surface 
     */
    if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
    }

    return (result);
}

// Kreis
int Circle(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color) // Kreis V.1.0
{
    Sint16 left, right, top, bottom;
    int result;
    Sint16 x1, y1, x2, y2;
    Sint16 cx = 0;
    Sint16 cy = r;
    Sint16 ocx = (Sint16) 0xffff;
    Sint16 ocy = (Sint16) 0xffff;
    Sint16 df = 1 - r;
    Sint16 d_e = 3;
    Sint16 d_se = -2 * r + 5;
    Sint16 xpcx, xmcx, xpcy, xmcy;
    Sint16 ypcy, ymcy, ypcx, ymcx;
    Uint8 *colorptr;

    /*
     * Sanity check radius 
     */
    if (r < 0) {
	return (-1);
    }

    /*
     * Special case for r=0 - draw a point 
     */
    if (r == 0) {
	return (PixelColor(dst, x, y, color));
    }

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Test if bounding box of circle is visible 
     */
    x1 = x - r;
    x2 = x + r;
    y1 = y - r;
    y2 = y + r;
    if ((x1<left) && (x2<left)) {
     return(0);
    } 
    if ((x1>right) && (x2>right)) {
     return(0);
    } 
    if ((y1<top) && (y2<top)) {
     return(0);
    } 
    if ((y1>bottom) && (y2>bottom)) {
     return(0);
    } 

    /*
     * Draw circle 
     */
    result = 0;

    /* Lock surface */
    if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	    return (-1);
	}
    }

    /*
     * Alpha Check 
     */
    if ((color & 255) == 255) {

	/*
	 * No Alpha - direct memory writes 
	 */

	/*
	 * Setup color 
	 */
	colorptr = (Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
	} else {
	    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	}

	/*
	 * Draw 
	 */
	do {
	    if ((ocy != cy) || (ocx != cx)) {
		xpcx = x + cx;
		xmcx = x - cx;
		if (cy > 0) {
		    ypcy = y + cy;
		    ymcy = y - cy;
		    result |= FastPixelColorNolock(dst, xmcx, ypcy, color);
		    result |= FastPixelColorNolock(dst, xpcx, ypcy, color);
		    result |= FastPixelColorNolock(dst, xmcx, ymcy, color);
		    result |= FastPixelColorNolock(dst, xpcx, ymcy, color);
		} else {
		    result |= FastPixelColorNolock(dst, xmcx, y, color);
		    result |= FastPixelColorNolock(dst, xpcx, y, color);
		}
		ocy = cy;
		xpcy = x + cy;
		xmcy = x - cy;
		if (cx > 0) {
		    ypcx = y + cx;
		    ymcx = y - cx;
		    result |= FastPixelColorNolock(dst, xmcy, ypcx, color);
		    result |= FastPixelColorNolock(dst, xpcy, ypcx, color);
		    result |= FastPixelColorNolock(dst, xmcy, ymcx, color);
		    result |= FastPixelColorNolock(dst, xpcy, ymcx, color);
		} else {
		    result |= FastPixelColorNolock(dst, xmcy, y, color);
		    result |= FastPixelColorNolock(dst, xpcy, y, color);
		}
		ocx = cx;
	    }
	    /*
	     * Update 
	     */
	    if (df < 0) {
		df += d_e;
		d_e += 2;
		d_se += 2;
	    } else {
		df += d_se;
		d_e += 2;
		d_se += 4;
		cy--;
	    }
	    cx++;
	} while (cx <= cy);

	/*
	 * Unlock surface 
	 */
	SDL_UnlockSurface(dst);

    } else {

	/*
	 * Using Alpha - blended pixel blits 
	 */

	do {
	    /*
	     * Draw 
	     */
	    if ((ocy != cy) || (ocx != cx)) {
		xpcx = x + cx;
		xmcx = x - cx;
		if (cy > 0) {
		    ypcy = y + cy;
		    ymcy = y - cy;
		    result |= PixelColorNolock (dst, xmcx, ypcy, color);
		    result |= PixelColorNolock (dst, xpcx, ypcy, color);
		    result |= PixelColorNolock (dst, xmcx, ymcy, color);
		    result |= PixelColorNolock (dst, xpcx, ymcy, color);
		} else {
		    result |= PixelColorNolock (dst, xmcx, y, color);
		    result |= PixelColorNolock (dst, xpcx, y, color);
		}
		ocy = cy;
		xpcy = x + cy;
		xmcy = x - cy;
		if (cx > 0) {
		    ypcx = y + cx;
		    ymcx = y - cx;
		    result |= PixelColorNolock (dst, xmcy, ypcx, color);
		    result |= PixelColorNolock (dst, xpcy, ypcx, color);
		    result |= PixelColorNolock (dst, xmcy, ymcx, color);
		    result |= PixelColorNolock (dst, xpcy, ymcx, color);
		} else {
		    result |= PixelColorNolock (dst, xmcy, y, color);
		    result |= PixelColorNolock (dst, xpcy, y, color);
		}
		ocx = cx;
	    }
	    /*
	     * Update 
	     */
	    if (df < 0) {
		df += d_e;
		d_e += 2;
		d_se += 2;
	    } else {
		df += d_se;
		d_e += 2;
		d_se += 4;
		cy--;
	    }
	    cx++;
	} while (cx <= cy);

    }	/* Alpha check */

    /* Unlock surface */
    if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
    }

    return (result);
}


int FilledCircleColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color) // Gefüllter Kreis V.1.0
{
    Sint16 left, right, top, bottom;
    int result;
    Sint16 x1, y1, x2, y2;
    Sint16 cx = 0;
    Sint16 cy = r;
    Sint16 ocx = (Sint16) 0xffff;
    Sint16 ocy = (Sint16) 0xffff;
    Sint16 df = 1 - r;
    Sint16 d_e = 3;
    Sint16 d_se = -2 * r + 5;
    Sint16 xpcx, xmcx, xpcy, xmcy;
    Sint16 ypcy, ymcy, ypcx, ymcx;

    /*
     * Sanity check radius 
     */
    if (r < 0) {
	return (-1);
    }

    /*
     * Special case for r=0 - draw a point 
     */
    if (r == 0) {
	return (PixelColor(dst, x, y, color));
    }

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Test if bounding box of circle is visible 
     */
    x1 = x - r;
    x2 = x + r;
    y1 = y - r;
    y2 = y + r;
    if ((x1<left) && (x2<left)) {
     return(0);
    } 
    if ((x1>right) && (x2>right)) {
     return(0);
    } 
    if ((y1<top) && (y2<top)) {
     return(0);
    } 
    if ((y1>bottom) && (y2>bottom)) {
     return(0);
    } 

    /*
     * Draw 
     */
    result = 0;
    do {
	xpcx = x + cx;
	xmcx = x - cx;
	xpcy = x + cy;
	xmcy = x - cy;
	if (ocy != cy) {
	    if (cy > 0) {
		ypcy = y + cy;
		ymcy = y - cy;
		result |= HLineColor(dst, xmcx, xpcx, ypcy, color);
		result |= HLineColor(dst, xmcx, xpcx, ymcy, color);
	    } else {
		result |= HLineColor(dst, xmcx, xpcx, y, color);
	    }
	    ocy = cy;
	}
	if (ocx != cx) {
	    if (cx != cy) {
		if (cx > 0) {
		    ypcx = y + cx;
		    ymcx = y - cx;
		    result |= HLineColor(dst, xmcy, xpcy, ymcx, color);
		    result |= HLineColor(dst, xmcy, xpcy, ypcx, color);
		} else {
		    result |= HLineColor(dst, xmcy, xpcy, y, color);
		}
	    }
	    ocx = cx;
	}
	/*
	 * Update 
	 */
	if (df < 0) {
	    df += d_e;
	    d_e += 2;
	    d_se += 2;
	} else {
	    df += d_se;
	    d_e += 2;
	    d_se += 4;
	    cy--;
	}
	cx++;
    } while (cx <= cy);

    return (result);
}

// Ellipse mit Clipping und Lock und Alpha
int EllipseRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a) // Ellipse mit Alpha V.1.0
{
    /*
     * Draw 
     */
    return (EllipseColor(dst, x, y, rx, ry, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

// Ellipse mit Clipping und Lock
int EllipseColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color) // Ellipse V.1.0
{
    Sint16 left, right, top, bottom;
    int result;
    Sint16 x1, y1, x2, y2;
    int ix, iy;
    int h, i, j, k;
    int oh, oi, oj, ok;
    int xmh, xph, ypk, ymk;
    int xmi, xpi, ymj, ypj;
    int xmj, xpj, ymi, ypi;
    int xmk, xpk, ymh, yph;
    Uint8 *colorptr;

    /*
     * Sanity check radii 
     */
    if ((rx < 0) || (ry < 0)) {
	return (-1);
    }

    /*
     * Special case for rx=0 - draw a vline 
     */
    if (rx == 0) {
	return (VLineColor(dst, x, y - ry, y + ry, color));
    }
    /*
     * Special case for ry=0 - draw a hline 
     */
    if (ry == 0) {
	return (HLineColor(dst, x - rx, x + rx, y, color));
    }

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Test if bounding box of ellipse is visible 
     */
    x1 = x - rx;
    x2 = x + rx;
    y1 = y - ry;
    y2 = y + ry;
    if ((x1<left) && (x2<left)) {
     return(0);
    } 
    if ((x1>right) && (x2>right)) {
     return(0);
    } 
    if ((y1<top) && (y2<top)) {
     return(0);
    } 
    if ((y1>bottom) && (y2>bottom)) {
     return(0);
    } 

    /*
     * Init vars 
     */
    oh = oi = oj = ok = 0xFFFF;

    /*
     * Draw 
     */
    result = 0;

    /* Lock surface */
    if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	    return (-1);
	}
    }

    /*
     * Check alpha 
     */
    if ((color & 255) == 255) {

	/*
	 * No Alpha - direct memory writes 
	 */

	/*
	 * Setup color 
	 */
	colorptr = (Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	    color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
	} else {
	    color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	}


	if (rx > ry) {
	    ix = 0;
	    iy = rx * 64;

	    do {
		h = (ix + 32) >> 6;
		i = (iy + 32) >> 6;
		j = (h * ry) / rx;
		k = (i * ry) / rx;

		if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
		    xph = x + h;
		    xmh = x - h;
		    if (k > 0) {
			ypk = y + k;
			ymk = y - k;
			result |= FastPixelColorNolock(dst, xmh, ypk, color);
			result |= FastPixelColorNolock(dst, xph, ypk, color);
			result |= FastPixelColorNolock(dst, xmh, ymk, color);
			result |= FastPixelColorNolock(dst, xph, ymk, color);
		    } else {
			result |= FastPixelColorNolock(dst, xmh, y, color);
			result |= FastPixelColorNolock(dst, xph, y, color);
		    }
		    ok = k;
		    xpi = x + i;
		    xmi = x - i;
		    if (j > 0) {
			ypj = y + j;
			ymj = y - j;
			result |= FastPixelColorNolock(dst, xmi, ypj, color);
			result |= FastPixelColorNolock(dst, xpi, ypj, color);
			result |= FastPixelColorNolock(dst, xmi, ymj, color);
			result |= FastPixelColorNolock(dst, xpi, ymj, color);
		    } else {
			result |= FastPixelColorNolock(dst, xmi, y, color);
			result |= FastPixelColorNolock(dst, xpi, y, color);
		    }
		    oj = j;
		}

		ix = ix + iy / rx;
		iy = iy - ix / rx;

	    } while (i > h);
	} else {
	    ix = 0;
	    iy = ry * 64;

	    do {
		h = (ix + 32) >> 6;
		i = (iy + 32) >> 6;
		j = (h * rx) / ry;
		k = (i * rx) / ry;

		if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
		    xmj = x - j;
		    xpj = x + j;
		    if (i > 0) {
			ypi = y + i;
			ymi = y - i;
			result |= FastPixelColorNolock(dst, xmj, ypi, color);
			result |= FastPixelColorNolock(dst, xpj, ypi, color);
			result |= FastPixelColorNolock(dst, xmj, ymi, color);
			result |= FastPixelColorNolock(dst, xpj, ymi, color);
		    } else {
			result |= FastPixelColorNolock(dst, xmj, y, color);
			result |= FastPixelColorNolock(dst, xpj, y, color);
		    }
		    oi = i;
		    xmk = x - k;
		    xpk = x + k;
		    if (h > 0) {
			yph = y + h;
			ymh = y - h;
			result |= FastPixelColorNolock(dst, xmk, yph, color);
			result |= FastPixelColorNolock(dst, xpk, yph, color);
			result |= FastPixelColorNolock(dst, xmk, ymh, color);
			result |= FastPixelColorNolock(dst, xpk, ymh, color);
		    } else {
			result |= FastPixelColorNolock(dst, xmk, y, color);
			result |= FastPixelColorNolock(dst, xpk, y, color);
		    }
		    oh = h;
		}

		ix = ix + iy / ry;
		iy = iy - ix / ry;

	    } while (i > h);
	}

    } else {

	if (rx > ry) {
	    ix = 0;
	    iy = rx * 64;

	    do {
		h = (ix + 32) >> 6;
		i = (iy + 32) >> 6;
		j = (h * ry) / rx;
		k = (i * ry) / rx;

		if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
		    xph = x + h;
		    xmh = x - h;
		    if (k > 0) {
			ypk = y + k;
			ymk = y - k;
			result |= PixelColorNolock (dst, xmh, ypk, color);
			result |= PixelColorNolock (dst, xph, ypk, color);
			result |= PixelColorNolock (dst, xmh, ymk, color);
			result |= PixelColorNolock (dst, xph, ymk, color);
		    } else {
			result |= PixelColorNolock (dst, xmh, y, color);
			result |= PixelColorNolock (dst, xph, y, color);
		    }
		    ok = k;
		    xpi = x + i;
		    xmi = x - i;
		    if (j > 0) {
			ypj = y + j;
			ymj = y - j;
			result |= PixelColorNolock (dst, xmi, ypj, color);
			result |= PixelColorNolock (dst, xpi, ypj, color);
			result |= PixelColorNolock (dst, xmi, ymj, color);
			result |= PixelColor(dst, xpi, ymj, color);
		    } else {
			result |= PixelColorNolock (dst, xmi, y, color);
			result |= PixelColorNolock (dst, xpi, y, color);
		    }
		    oj = j;
		}

		ix = ix + iy / rx;
		iy = iy - ix / rx;

	    } while (i > h);
	} else {
	    ix = 0;
	    iy = ry * 64;

	    do {
		h = (ix + 32) >> 6;
		i = (iy + 32) >> 6;
		j = (h * rx) / ry;
		k = (i * rx) / ry;

		if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
		    xmj = x - j;
		    xpj = x + j;
		    if (i > 0) {
			ypi = y + i;
			ymi = y - i;
			result |= PixelColorNolock (dst, xmj, ypi, color);
			result |= PixelColorNolock (dst, xpj, ypi, color);
			result |= PixelColorNolock (dst, xmj, ymi, color);
			result |= PixelColorNolock (dst, xpj, ymi, color);
		    } else {
			result |= PixelColorNolock (dst, xmj, y, color);
			result |= PixelColorNolock (dst, xpj, y, color);
		    }
		    oi = i;
		    xmk = x - k;
		    xpk = x + k;
		    if (h > 0) {
			yph = y + h;
			ymh = y - h;
			result |= PixelColorNolock (dst, xmk, yph, color);
			result |= PixelColorNolock (dst, xpk, yph, color);
			result |= PixelColorNolock (dst, xmk, ymh, color);
			result |= PixelColorNolock (dst, xpk, ymh, color);
		    } else {
			result |= PixelColorNolock (dst, xmk, y, color);
			result |= PixelColorNolock (dst, xpk, y, color);
		    }
		    oh = h;
		}

		ix = ix + iy / ry;
		iy = iy - ix / ry;

	    } while (i > h);
	}

    }				/* Alpha check */

    /* Unlock surface */
    if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
    }

    return (result);
}
// Gefüllte Ellipse
int FilledEllipseColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color) // Gefüllte Ellipse V.1.0
{
    Sint16 left, right, top, bottom;
    int result;
    Sint16 x1, y1, x2, y2;
    int ix, iy;
    int h, i, j, k;
    int oh, oi, oj, ok;
    int xmh, xph;
    int xmi, xpi;
    int xmj, xpj;
    int xmk, xpk;

    /*
     * Sanity check radii 
     */
    if ((rx < 0) || (ry < 0)) {
	return (-1);
    }

    /*
     * Special case for rx=0 - draw a vline 
     */
    if (rx == 0) {
	return (VLineColor(dst, x, y - ry, y + ry, color));
    }
    /*
     * Special case for ry=0 - draw a hline 
     */
    if (ry == 0) {
	return (HLineColor(dst, x - rx, x + rx, y, color));
    }

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Test if bounding box of ellipse is visible 
     */
    x1 = x - rx;
    x2 = x + rx;
    y1 = y - ry;
    y2 = y + ry;
    if ((x1<left) && (x2<left)) {
     return(0);
    } 
    if ((x1>right) && (x2>right)) {
     return(0);
    } 
    if ((y1<top) && (y2<top)) {
     return(0);
    } 
    if ((y1>bottom) && (y2>bottom)) {
     return(0);
    } 

    /*
     * Init vars 
     */
    oh = oi = oj = ok = 0xFFFF;

    /*
     * Draw 
     */
    result = 0;
    if (rx > ry) {
	ix = 0;
	iy = rx * 64;

	do {
	    h = (ix + 32) >> 6;
	    i = (iy + 32) >> 6;
	    j = (h * ry) / rx;
	    k = (i * ry) / rx;

	    if ((ok != k) && (oj != k)) {
		xph = x + h;
		xmh = x - h;
		if (k > 0) {
		    result |= HLineColor(dst, xmh, xph, y + k, color);
		    result |= HLineColor(dst, xmh, xph, y - k, color);
		} else {
		    result |= HLineColor(dst, xmh, xph, y, color);
		}
		ok = k;
	    }
	    if ((oj != j) && (ok != j) && (k != j)) {
		xmi = x - i;
		xpi = x + i;
		if (j > 0) {
		    result |= HLineColor(dst, xmi, xpi, y + j, color);
		    result |= HLineColor(dst, xmi, xpi, y - j, color);
		} else {
		    result |= HLineColor(dst, xmi, xpi, y, color);
		}
		oj = j;
	    }

	    ix = ix + iy / rx;
	    iy = iy - ix / rx;

	} while (i > h);
    } else {
	ix = 0;
	iy = ry * 64;

	do {
	    h = (ix + 32) >> 6;
	    i = (iy + 32) >> 6;
	    j = (h * rx) / ry;
	    k = (i * rx) / ry;

	    if ((oi != i) && (oh != i)) {
		xmj = x - j;
		xpj = x + j;
		if (i > 0) {
		    result |= HLineColor(dst, xmj, xpj, y + i, color);
		    result |= HLineColor(dst, xmj, xpj, y - i, color);
		} else {
		    result |= HLineColor(dst, xmj, xpj, y, color);
		}
		oi = i;
	    }
	    if ((oh != h) && (oi != h) && (i != h)) {
		xmk = x - k;
		xpk = x + k;
		if (h > 0) {
		    result |= HLineColor(dst, xmk, xpk, y + h, color);
		    result |= HLineColor(dst, xmk, xpk, y - h, color);
		} else {
		    result |= HLineColor(dst, xmk, xpk, y, color);
		}
		oh = h;
	    }

	    ix = ix + iy / ry;
	    iy = iy - ix / ry;

	} while (i > h);
    }

    return (result);
}

// Gefüllte Ellipse mit Alpha vom RGBA
int FilledEllipseRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)// Gefüllte Ellipse mit Alpha vom RGBA V.1.0
{
    /*
     * Draw 
     */
    return (FilledEllipseColor
	    (dst, x, y, rx, ry, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

// Gefülltes Kreisdiagramm
int FilledpieColor(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color)// Gefülltes Kreisdiagramm V.1.0.1
{
    Sint16 left, right, top, bottom;
    Sint16 x1, y1, x2, y2;
    int result;
    double angle, start_angle, end_angle;
    double deltaAngle;
    double dr;
    int posX, posY;
    int numpoints, i;
    Sint16 *vx, *vy;

    /*
     * Sanity check radii 
     */
    if (rad < 0) {
	return (-1);
    }

    /*
     * Fixup angles
     */
    start = start % 360;
    end = end % 360;

    /*
     * Special case for rad=0 - draw a point 
     */
    if (rad == 0) {
	return (PixelColor(dst, x, y, color));
    }

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Test if bounding box of pie's circle is visible 
     */
    x1 = x - rad;
    x2 = x + rad;
    y1 = y - rad;
    y2 = y + rad;
    if ((x1<left) && (x2<left)) {
     return(0);
    } 
    if ((x1>right) && (x2>right)) {
     return(0);
    } 
    if ((y1<top) && (y2<top)) {
     return(0);
    } 
    if ((y1>bottom) && (y2>bottom)) {
     return(0);
    } 

    /*
     * Variable setup 
     */
    dr = (double) rad;
    deltaAngle = 3.0 / dr;
    start_angle = (double) start *(2.0 * M_PI / 360.0);
    end_angle = (double) end *(2.0 * M_PI / 360.0);
    if (start > end) {
	end_angle += (2.0 * M_PI);
    }

    /* Count points (rather than calculate it) */
    numpoints = 1;
    angle = start_angle;
    while (angle <= end_angle) {
	angle += deltaAngle;
	numpoints++;
    }

    /* Check size of array */
    if (numpoints == 1) {
	return (PixelColor(dst, x, y, color));
    } else if (numpoints == 2) {
	posX = x + (int) (dr * cos(start_angle));
	posY = y + (int) (dr * sin(start_angle));
	return (LineColor(dst, x, y, posX, posY, color));
    }

    /* Allocate vertex array */
    //vx = vy = (Uint16 *) malloc(2 * sizeof(Uint16) * numpoints); old
	vx = vy = (Sint16*)((Uint16 *) malloc(2 * sizeof(Uint16) * numpoints)); // new
    if (vx == NULL) {
	return (-1);
    }
    vy += numpoints;

    /* Center */
    vx[0] = x;
    vy[0] = y;

    /* Calculate and store vertices */
    i = 1;
    angle = start_angle;
    while (angle <= end_angle) {
	vx[i] = x + (int) (dr * cos(angle));
	vy[i] = y + (int) (dr * sin(angle));
	angle += deltaAngle;
	i++;
    }

    /* Draw */
    result = FilledPolygonColor(dst, vx, vy, numpoints, color);

    /* Free vertex array */
    free(vx);

    return (result);
}

// Gefülltes Kreisdiagramm mit Alpha
int FilledpieRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Sint16 rad,
	Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a) // Gefülltes Kreisdiagramm mit Alpha V.1.0
{
    return (FilledpieColor(dst, x, y, rad, start, end,
			   ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));

}

// Gefülltes Polygon
int FilledPolygonColor(SDL_Surface * dst, Sint16 * vx, Sint16 * vy, int n, int color) // Gefülltes Polygon V.1.0
{
    int result;
    int i;
    int y;
    int miny, maxy;
    int x1, y1;
    int x2, y2;
    int ind1, ind2;
    int ints;

    /*
     * Sanity check 
     */
    if (n < 3) {
	return -1;
    }

    /*
     * Allocate temp array, only grow array 
     */
    if (!PolyAllocated) {
	PolyInts = (int *) malloc(sizeof(int) * n);
	PolyAllocated = n;
    } else {
	if (PolyAllocated < n) {
	    PolyInts = (int *) realloc(PolyInts, sizeof(int) * n);
	    PolyAllocated = n;
	}
    }

    /*
     * Determine Y maxima 
     */
    miny = vy[0];
    maxy = vy[0];
    for (i = 1; (i < n); i++) {
	if (vy[i] < miny) {
	    miny = vy[i];
	} else if (vy[i] > maxy) {
	    maxy = vy[i];
	}
    }

    /*
     * Draw, scanning y 
     */
    result = 0;
    for (y = miny; (y <= maxy); y++) {
	ints = 0;
	for (i = 0; (i < n); i++) {
	    if (!i) {
		ind1 = n - 1;
		ind2 = 0;
	    } else {
		ind1 = i - 1;
		ind2 = i;
	    }
	    y1 = vy[ind1];
	    y2 = vy[ind2];
	    if (y1 < y2) {
		x1 = vx[ind1];
		x2 = vx[ind2];
	    } else if (y1 > y2) {
		y2 = vy[ind1];
		y1 = vy[ind2];
		x2 = vx[ind1];
		x1 = vx[ind2];
	    } else {
		continue;
	    }
	    if ((y >= y1) && (y < y2)) {
		PolyInts[ints++] = (y - y1) * (x2 - x1) / (y2 - y1) + x1;
	    } else if ((y == maxy) && (y > y1) && (y <= y2)) {
		PolyInts[ints++] = (y - y1) * (x2 - x1) / (y2 - y1) + x1;
	    }
	}
	qsort(PolyInts, ints, sizeof(int), CompareInt);

	for (i = 0; (i < ints); i += 2) {
	    result |= HLineColor(dst, PolyInts[i], PolyInts[i + 1], y, color);
	}
    }

    return (result);
}
// Gefülltes Polygon mit Alpha
int FilledPolygonRGBA(SDL_Surface * dst, Sint16 * vx, Sint16 * vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a) // Gefülltes Polygon mit Alpha V.1.0
{
    /*
     * Draw 
     */
    return (FilledPolygonColor(dst, vx, vy, n, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}
// Buchstabe mit Clipping
int CharacterColor(SDL_Surface * dst, Sint16 x, Sint16 y, char c, Uint32 color) //Buchstabe mit Clipping V.1.0
{
    Sint16 left, right, top, bottom;
    Sint16 x1, y1, x2, y2;
    SDL_Rect srect;
    SDL_Rect drect;
    int result;
    int ix, iy, k;
    unsigned char *charpos;
    unsigned char bits[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    unsigned char *bitpos;
    Uint8 *curpos;
    int forced_redraw;

    /*
     * Get clipping boundary 
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    /*
     * Test if bounding box of character is visible 
     */
    x1 = x;
    x2 = x + 8;
    y1 = y;
    y2 = y + 8;
    if ((x1<left) && (x2<left)) {
     return(0);
    } 
    if ((x1>right) && (x2>right)) {
     return(0);
    } 
    if ((y1<top) && (y2<top)) {
     return(0);
    } 
    if ((y1>bottom) && (y2>bottom)) {
     return(0);
    } 

    /*
     * Setup source rectangle for 8x8 bitmap 
     */
    srect.x = 0;
    srect.y = 0;
    srect.w = 8;
    srect.h = 8;

    /*
     * Setup destination rectangle for 8x8 bitmap 
     */
    drect.x = x;
    drect.y = y;
    drect.w = 8;
    drect.h = 8;

    /*
     * Create new 8x8 bitmap surface if not already present 
     */
    if (GfxFont[(unsigned char) c] == NULL) {
	GfxFont[(unsigned char) c] =
	    SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_HWSURFACE | SDL_SRCALPHA, 8, 8,
				 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	/*
	 * Check pointer 
	 */
	if (GfxFont[(unsigned char) c] == NULL) {
	    return (-1);
	}
	/*
	 * Definitely redraw 
	 */
	forced_redraw = 1;
    } else {
	forced_redraw = 0;
    }

    /*
     * Check if color has changed 
     */
    if ((GfxFontColor[(unsigned char) c] != color) || (forced_redraw)) {
	/*
	 * Redraw character 
	 */
	SDL_SetAlpha(GfxFont[(unsigned char) c], SDL_SRCALPHA, 255);
	GfxFontColor[(unsigned char) c] = color;

	/*
	 * Variable setup 
	 */
	k = (unsigned char) c;
	k *= 8;
	charpos = gfxFontdata;
	charpos += k;

	/*
	 * Clear bitmap 
	 */
	curpos = (Uint8 *) GfxFont[(unsigned char) c]->pixels;
	memset(curpos, 0, 8 * 8 * 4);

	/*
	 * Drawing loop 
	 */
	for (iy = 0; iy < 8; iy++) 
	{
	    bitpos = bits;
	    for (ix = 0; ix < 8; ix++)
		{
		if ((*charpos & *bitpos) == *bitpos) 
		{
		    memcpy(curpos, &color, 4);
		}
		bitpos++;
		curpos += 4;
	    }
	    charpos++;
	}
    }

    /*
     * Draw bitmap onto destination surface 
     */
    result = SDL_BlitSurface(GfxFont[(unsigned char) c], &srect, dst, &drect);

    return (result);
}

// Buchstabe mit Clipping und Alpha
int CharacterRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, char c, Uint8 r, Uint8 g, Uint8 b, Uint8 a)// Buchstabe mit Clipping und Alpha V.1.0
{
    /*
     * Draw 
     */
    return (CharacterColor(dst, x, y, c, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}
// String
int StringColor(SDL_Surface * dst, Sint16 x, Sint16 y, char *c, Uint32 color) // String V.1.0
{
    int result;
    int i, length;
    char *curchar;
    int curx;

    length = strlen(c);
    curchar = c;
    curx = x;
    result = 0;
    for (i = 0; i < length; i++) {
	result |= CharacterColor(dst, curx, y, *curchar, color);
	curchar++;
	curx += 8;
    }

    return (result);
}

// String mit Alpha
int StringRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, char *c, Uint8 r, Uint8 g, Uint8 b, Uint8 a) // String mit Alpha V.1.0
{
    /*
     * Draw 
     */
    return (StringColor(dst, x, y, c, ((Uint32) r << 24) | ((Uint32) g << 16) | ((Uint32) b << 8) | (Uint32) a));
}

/*  

  SDL_rotozoom.c - rotozoomer for 32bit or 8bit surfaces

  LGPL (c) A. Schiffler

*/

#ifdef WIN32
#include <windows.h>
#endif

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

/* 
 
 32bit Zoomer with optional anti-aliasing by bilinear interpolation.

 Zoomes 32bit RGBA/ABGR 'src' surface to 'dst' surface.
 
*/

// Zoom Surface RGBA
int ZoomSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int smooth) // Zoom Surface RGBA V.1.0
{
    int x, y, sx, sy, *sax, *say, *csax, *csay, csx, csy, ex, ey, t1, t2, sstep;
    tColorRGBA *c00, *c01, *c10, *c11;
    tColorRGBA *sp, *csp, *dp;
    int sgap, dgap;

    /*
     * Variable setup 
     */
    if (smooth) {
	/*
	 * For interpolation: assume source dimension is one pixel 
	 */
	/*
	 * smaller to avoid overflow on right and bottom edge.     
	 */
	sx = (int) (65536.0 * (float) (src->w - 1) / (float) dst->w);
	sy = (int) (65536.0 * (float) (src->h - 1) / (float) dst->h);
    } else {
	sx = (int) (65536.0 * (float) src->w / (float) dst->w);
	sy = (int) (65536.0 * (float) src->h / (float) dst->h);
    }

    /*
     * Allocate memory for row increments 
     */
    if ((sax = (int *) malloc((dst->w + 1) * sizeof(Uint32))) == NULL) {
	return (-1);
    }
    if ((say = (int *) malloc((dst->h + 1) * sizeof(Uint32))) == NULL) {
	free(sax);
	return (-1);
    }

    /*
     * Precalculate row increments 
     */
    csx = 0;
    csax = sax;
    for (x = 0; x <= dst->w; x++) {
	*csax = csx;
	csax++;
	csx &= 0xffff;
	csx += sx;
    }
    csy = 0;
    csay = say;
    for (y = 0; y <= dst->h; y++) {
	*csay = csy;
	csay++;
	csy &= 0xffff;
	csy += sy;
    }

    /*
     * Pointer setup 
     */
    sp = csp = (tColorRGBA *) src->pixels;
    dp = (tColorRGBA *) dst->pixels;
    sgap = src->pitch - src->w * 4;
    dgap = dst->pitch - dst->w * 4;

    /*
     * Switch between interpolating and non-interpolating code 
     */
    if (smooth) {

	/*
	 * Interpolating Zoom 
	 */

	/*
	 * Scan destination 
	 */
	csay = say;
	for (y = 0; y < dst->h; y++) {
	    /*
	     * Setup color source pointers 
	     */
	    c00 = csp;
	    c01 = csp;
	    c01++;
	    c10 = (tColorRGBA *) ((Uint8 *) csp + src->pitch);
	    c11 = c10;
	    c11++;
	    csax = sax;
	    for (x = 0; x < dst->w; x++) {

		/*
		 * Interpolate colors 
		 */
		ex = (*csax & 0xffff);
		ey = (*csay & 0xffff);
		t1 = ((((c01->r - c00->r) * ex) >> 16) + c00->r) & 0xff;
		t2 = ((((c11->r - c10->r) * ex) >> 16) + c10->r) & 0xff;
		dp->r = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->g - c00->g) * ex) >> 16) + c00->g) & 0xff;
		t2 = ((((c11->g - c10->g) * ex) >> 16) + c10->g) & 0xff;
		dp->g = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->b - c00->b) * ex) >> 16) + c00->b) & 0xff;
		t2 = ((((c11->b - c10->b) * ex) >> 16) + c10->b) & 0xff;
		dp->b = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->a - c00->a) * ex) >> 16) + c00->a) & 0xff;
		t2 = ((((c11->a - c10->a) * ex) >> 16) + c10->a) & 0xff;
		dp->a = (((t2 - t1) * ey) >> 16) + t1;

		/*
		 * Advance source pointers 
		 */
		csax++;
		sstep = (*csax >> 16);
		c00 += sstep;
		c01 += sstep;
		c10 += sstep;
		c11 += sstep;
		/*
		 * Advance destination pointer 
		 */
		dp++;
	    }
	    /*
	     * Advance source pointer 
	     */
	    csay++;
	    csp = (tColorRGBA *) ((Uint8 *) csp + (*csay >> 16) * src->pitch);
	    /*
	     * Advance destination pointers 
	     */
	    dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}

    } else {

	/*
	 * Non-Interpolating Zoom 
	 */

	csay = say;
	for (y = 0; y < dst->h; y++) {
	    sp = csp;
	    csax = sax;
	    for (x = 0; x < dst->w; x++) {
		/*
		 * Draw 
		 */
		*dp = *sp;
		/*
		 * Advance source pointers 
		 */
		csax++;
		sp += (*csax >> 16);
		/*
		 * Advance destination pointer 
		 */
		dp++;
	    }
	    /*
	     * Advance source pointer 
	     */
	    csay++;
	    csp = (tColorRGBA *) ((Uint8 *) csp + (*csay >> 16) * src->pitch);
	    /*
	     * Advance destination pointers 
	     */
	    dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}

    }

    /*
     * Remove temp arrays 
     */
    free(sax);
    free(say);

    return (0);
}

/* 
 
 8bit Zoomer without smoothing.

 Zoomes 8bit palette/Y 'src' surface to 'dst' surface.
 
*/
// Zoom Surface Y
int ZoomSurfaceY(SDL_Surface * src, SDL_Surface * dst) // Zoom Surface Y V.1.0
{
    Uint32 x, y, sx, sy, *sax, *say, *csax, *csay, csx, csy;
    Uint8 *sp, *dp, *csp;
    int dgap;

    /*
     * Variable setup 
     */
    sx = (Uint32) (65536.0 * (float) src->w / (float) dst->w);
    sy = (Uint32) (65536.0 * (float) src->h / (float) dst->h);

    /*
     * Allocate memory for row increments 
     */
    if ((sax = (Uint32 *) malloc(dst->w * sizeof(Uint32))) == NULL) {
	return (-1);
    }
    if ((say = (Uint32 *) malloc(dst->h * sizeof(Uint32))) == NULL) {
	if (sax != NULL) {
	    free(sax);
	}
	return (-1);
    }

    /*
     * Precalculate row increments 
     */
    csx = 0;
    csax = sax;
    for (x = 0; x < dst->w; x++) {
	csx += sx;
	*csax = (csx >> 16);
	csx &= 0xffff;
	csax++;
    }
    csy = 0;
    csay = say;
    for (y = 0; y < dst->h; y++) {
	csy += sy;
	*csay = (csy >> 16);
	csy &= 0xffff;
	csay++;
    }

    csx = 0;
    csax = sax;
    for (x = 0; x < dst->w; x++) {
	csx += (*csax);
	csax++;
    }
    csy = 0;
    csay = say;
    for (y = 0; y < dst->h; y++) {
	csy += (*csay);
	csay++;
    }

    /*
     * Pointer setup 
     */
    sp = csp = (Uint8 *) src->pixels;
    dp = (Uint8 *) dst->pixels;
    dgap = dst->pitch - dst->w;

    /*
     * Draw 
     */
    csay = say;
    for (y = 0; y < dst->h; y++) {
	csax = sax;
	sp = csp;
	for (x = 0; x < dst->w; x++) {
	    /*
	     * Draw 
	     */
	    *dp = *sp;
	    /*
	     * Advance source pointers 
	     */
	    sp += (*csax);
	    csax++;
	    /*
	     * Advance destination pointer 
	     */
	    dp++;
	}
	/*
	 * Advance source pointer (for row) 
	 */
	csp += ((*csay) * src->pitch);
	csay++;
	/*
	 * Advance destination pointers 
	 */
	dp += dgap;
    }

    /*
     * Remove temp arrays 
     */
    free(sax);
    free(say);

    return (0);
}

/* 
 
 32bit Rotozoomer with optional anti-aliasing by bilinear interpolation.

 Rotates and zoomes 32bit RGBA/ABGR 'src' surface to 'dst' surface.
 
*/
// Transform Surface RGBA
void TransformSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos, int smooth) // Transform Surface RGBA V.1.0
{
    int x, y, t1, t2, dx, dy, xd, yd, sdx, sdy, ax, ay, ex, ey, sw, sh;
    tColorRGBA c00, c01, c10, c11;
    tColorRGBA *pc, *sp;
    int gap;

    /*
     * Variable setup 
     */
    xd = ((src->w - dst->w) << 15);
    yd = ((src->h - dst->h) << 15);
    ax = (cx << 16) - (icos * cx);
    ay = (cy << 16) - (isin * cx);
    sw = src->w - 1;
    sh = src->h - 1;
	//pc = dst->pixel; // old 
    pc = (tColorRGBA*)dst->pixels; // ??? Richtig ??
    gap = dst->pitch - dst->w * 4;

    /*
     * Switch between interpolating and non-interpolating code 
     */
    if (smooth) {
	for (y = 0; y < dst->h; y++) {
	    dy = cy - y;
	    sdx = (ax + (isin * dy)) + xd;
	    sdy = (ay - (icos * dy)) + yd;
	    for (x = 0; x < dst->w; x++) {
		dx = (sdx >> 16);
		dy = (sdy >> 16);
		if ((dx >= -1) && (dy >= -1) && (dx < src->w) && (dy < src->h)) {
		    if ((dx >= 0) && (dy >= 0) && (dx < sw) && (dy < sh)) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			sp += 1;
			c01 = *sp;
			sp = (tColorRGBA *) ((Uint8 *) sp + src->pitch);
			sp -= 1;
			c10 = *sp;
			sp += 1;
			c11 = *sp;
		    } else if ((dx == sw) && (dy == sh)) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if ((dx == -1) && (dy == -1)) {
			sp = (tColorRGBA *) (src->pixels);
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if ((dx == -1) && (dy == sh)) {
			sp = (tColorRGBA *) (src->pixels);
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if ((dx == sw) && (dy == -1)) {
			sp = (tColorRGBA *) (src->pixels);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if (dx == -1) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			sp = (tColorRGBA *) ((Uint8 *) sp + src->pitch);
			c11 = *sp;
		    } else if (dy == -1) {
			sp = (tColorRGBA *) (src->pixels);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			sp += 1;
			c11 = *sp;
		    } else if (dx == sw) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			sp = (tColorRGBA *) ((Uint8 *) sp + src->pitch);
			c10 = *sp;
			c11 = *sp;
		    } else if (dy == sh) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			sp += 1;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    }
		    /*
		     * Interpolate colors 
		     */
		    ex = (sdx & 0xffff);
		    ey = (sdy & 0xffff);
		    t1 = ((((c01.r - c00.r) * ex) >> 16) + c00.r) & 0xff;
		    t2 = ((((c11.r - c10.r) * ex) >> 16) + c10.r) & 0xff;
		    pc->r = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.g - c00.g) * ex) >> 16) + c00.g) & 0xff;
		    t2 = ((((c11.g - c10.g) * ex) >> 16) + c10.g) & 0xff;
		    pc->g = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.b - c00.b) * ex) >> 16) + c00.b) & 0xff;
		    t2 = ((((c11.b - c10.b) * ex) >> 16) + c10.b) & 0xff;
		    pc->b = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.a - c00.a) * ex) >> 16) + c00.a) & 0xff;
		    t2 = ((((c11.a - c10.a) * ex) >> 16) + c10.a) & 0xff;
		    pc->a = (((t2 - t1) * ey) >> 16) + t1;
		}
		sdx += icos;
		sdy += isin;
		pc++;
	    }
	    pc = (tColorRGBA *) ((Uint8 *) pc + gap);
	}
    } else {
	for (y = 0; y < dst->h; y++) {
	    dy = cy - y;
	    sdx = (ax + (isin * dy)) + xd;
	    sdy = (ay - (icos * dy)) + yd;
	    for (x = 0; x < dst->w; x++) {
		dx = (short) (sdx >> 16);
		dy = (short) (sdy >> 16);
		if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
		    sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
		    sp += dx;
		    *pc = *sp;
		}
		sdx += icos;
		sdy += isin;
		pc++;
	    }
	    pc = (tColorRGBA *) ((Uint8 *) pc + gap);
	}
    }
}

/* 
 
 8bit Rotozoomer without smoothing

 Rotates and zoomes 8bit palette/Y 'src' surface to 'dst' surface.
 
*/
// Transform Surface Y
void TransformSurfaceY(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos) //Transform Surface Y V.1.0
{
    int x, y, dx, dy, xd, yd, sdx, sdy, ax, ay, sw, sh;
    tColorY *pc, *sp;
    int gap;

    /*
     * Variable setup 
     */
    xd = ((src->w - dst->w) << 15);
    yd = ((src->h - dst->h) << 15);
    ax = (cx << 16) - (icos * cx);
    ay = (cy << 16) - (isin * cx);
    sw = src->w - 1;
    sh = src->h - 1;
    //pc = dst->pixels; // old 
	pc = (tColorY*)dst->pixels; // new
    gap = dst->pitch - dst->w;
    /*
     * Clear surface to colorkey 
     */
    memset(pc, (unsigned char) (src->format->colorkey & 0xff), dst->pitch * dst->h);
    /*
     * Iterate through destination surface 
     */
    for (y = 0; y < dst->h; y++) {
	dy = cy - y;
	sdx = (ax + (isin * dy)) + xd;
	sdy = (ay - (icos * dy)) + yd;
	for (x = 0; x < dst->w; x++) {
	    dx = (short) (sdx >> 16);
	    dy = (short) (sdy >> 16);
	    if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
		sp = (tColorY *) (src->pixels);
		sp += (src->pitch * dy + dx);
		*pc = *sp;
	    }
	    sdx += icos;
	    sdy += isin;
	    pc++;
	}
	pc += gap;
    }
}

/* 
 
 rotozoomSurface()

 Rotates and zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'angle' is the rotation in degrees. 'zoom' a scaling factor. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

#define VALUE_LIMIT	0.001


/* Local rotozoom-size function with trig result return */

// Rotozoom Size Trig
void RotozoomSurfaceSizeTrig(int width, int height, double angle, double zoom, int *dstwidth, int *dstheight,
			     double *canglezoom, double *sanglezoom) // Rotozoom Size Trig V.1.0
{
    double x, y, cx, cy, sx, sy;
    double radangle;
    int dstwidthhalf, dstheighthalf;

    /*
     * Determine destination width and height by rotating a centered source box 
     */
    radangle = angle * (M_PI / 180.0);
    *sanglezoom = sin(radangle);
    *canglezoom = cos(radangle);
    *sanglezoom *= zoom;
    *canglezoom *= zoom;
    x = width / 2;
    y = height / 2;
    cx = *canglezoom * x;
    cy = *canglezoom * y;
    sx = *sanglezoom * x;
    sy = *sanglezoom * y;
    dstwidthhalf = MAX((int)
		       ceil(MAX(MAX(MAX(fabs(cx + sy), fabs(cx - sy)), fabs(-cx + sy)), fabs(-cx - sy))), 1);
    dstheighthalf = MAX((int)
			ceil(MAX(MAX(MAX(fabs(sx + cy), fabs(sx - cy)), fabs(-sx + cy)), fabs(-sx - cy))), 1);
    *dstwidth = 2 * dstwidthhalf;
    *dstheight = 2 * dstheighthalf;
}


/* Publically available rotozoom-size function */

// Rotozoom Size
void RotozoomSurfaceSize(int width, int height, double angle, double zoom, int *dstwidth, int *dstheight)// Rotozoom Size V.1.0
{
    double dummy_sanglezoom, dummy_canglezoom;

    RotozoomSurfaceSizeTrig(width, height, angle, zoom, dstwidth, dstheight, &dummy_sanglezoom, &dummy_canglezoom);
}


/* Publically available rotozoom function */

// Rotozoom
SDL_Surface *RotozoomSurface(SDL_Surface * src, double angle, double zoom, int smooth)// Rotozoom V.1.0
{
    SDL_Surface *rz_src;
    SDL_Surface *rz_dst;
    double zoominv;
    double sanglezoom, canglezoom, sanglezoominv, canglezoominv;
    int dstwidthhalf, dstwidth, dstheighthalf, dstheight;
    //double x, y, cx, cy, sx, sy;
    int is32bit;
    int i, src_converted;

    /*
     * Sanity check 
     */
    if (src == NULL)
	return (NULL);

    /*
     * Determine if source surface is 32bit or 8bit 
     */
    is32bit = (src->format->BitsPerPixel == 32);
    if ((is32bit) || (src->format->BitsPerPixel == 8)) {
	/*
	 * Use source surface 'as is' 
	 */
	rz_src = src;
	src_converted = 0;
    } else {
	/*
	 * New source surface is 32bit with a defined RGBA ordering 
	 */
	rz_src =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	SDL_BlitSurface(src, NULL, rz_src, NULL);
	src_converted = 1;
	is32bit = 1;
    }

    /*
     * Sanity check zoom factor 
     */
    if (zoom < VALUE_LIMIT) {
	zoom = VALUE_LIMIT;
    }
    zoominv = 65536.0 / (zoom * zoom);

    /*
     * Check if we have a rotozoom or just a zoom 
     */
    if (fabs(angle) > VALUE_LIMIT) {

	/*
	 * Angle!=0: full rotozoom 
	 */
	/*
	 * ----------------------- 
	 */

	/* Determine target size */
	RotozoomSurfaceSizeTrig(rz_src->w, rz_src->h, angle, zoom, &dstwidth, &dstheight, &canglezoom, &sanglezoom);

	/*
	 * Calculate target factors from sin/cos and zoom 
	 */
	sanglezoominv = sanglezoom;
	canglezoominv = canglezoom;
	sanglezoominv *= zoominv;
	canglezoominv *= zoominv;

	/* Calculate half size */
	dstwidthhalf = dstwidth / 2;
	dstheighthalf = dstheight / 2;

	/*
	 * Alloc space to completely contain the rotated surface 
	 */
	rz_dst = NULL;
	if (is32bit) {
	    /*
	     * Target surface is 32bit with source RGBA/ABGR ordering 
	     */
	    rz_dst =
		SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				     rz_src->format->Rmask, rz_src->format->Gmask,
				     rz_src->format->Bmask, rz_src->format->Amask);
	} else {
	    /*
	     * Target surface is 8bit 
	     */
	    rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
	}

	/*
	 * Lock source surface 
	 */
	SDL_LockSurface(rz_src);
	/*
	 * Check which kind of surface we have 
	 */
	if (is32bit) {
	    /*
	     * Call the 32bit transformation routine to do the rotation (using alpha) 
	     */
	    TransformSurfaceRGBA(rz_src, rz_dst, dstwidthhalf, dstheighthalf,
				 (int) (sanglezoominv), (int) (canglezoominv), smooth);
	    /*
	     * Turn on source-alpha support 
	     */
	    SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
	} else {
	    /*
	     * Copy palette and colorkey info 
	     */
	    for (i = 0; i < rz_src->format->palette->ncolors; i++) {
		rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	    }
	    rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	    /*
	     * Call the 8bit transformation routine to do the rotation 
	     */
	    TransformSurfaceY(rz_src, rz_dst, dstwidthhalf, dstheighthalf,
			      (int) (sanglezoominv), (int) (canglezoominv));
	    SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
	}
	/*
	 * Unlock source surface 
	 */
	SDL_UnlockSurface(rz_src);

    } else {

	/*
	 * Angle=0: Just a zoom 
	 */
	/*
	 * -------------------- 
	 */

	/*
	 * Calculate target size
	 */
	ZoomSurfaceSize(rz_src->w, rz_src->h, zoom, zoom, &dstwidth, &dstheight);

	/*
	 * Alloc space to completely contain the zoomed surface 
	 */
	rz_dst = NULL;
	if (is32bit) {
	    /*
	     * Target surface is 32bit with source RGBA/ABGR ordering 
	     */
	    rz_dst =
		SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				     rz_src->format->Rmask, rz_src->format->Gmask,
				     rz_src->format->Bmask, rz_src->format->Amask);
	} else {
	    /*
	     * Target surface is 8bit 
	     */
	    rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
	}

	/*
	 * Lock source surface 
	 */
	SDL_LockSurface(rz_src);
	/*
	 * Check which kind of surface we have 
	 */
	if (is32bit) {
	    /*
	     * Call the 32bit transformation routine to do the zooming (using alpha) 
	     */
	    ZoomSurfaceRGBA(rz_src, rz_dst, smooth);
	    /*
	     * Turn on source-alpha support 
	     */
	    SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
	} else {
	    /*
	     * Copy palette and colorkey info 
	     */
	    for (i = 0; i < rz_src->format->palette->ncolors; i++) {
		rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	    }
	    rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	    /*
	     * Call the 8bit transformation routine to do the zooming 
	     */
	    ZoomSurfaceY(rz_src, rz_dst);
	    SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
	}
	/*
	 * Unlock source surface 
	 */
	SDL_UnlockSurface(rz_src);
    }

    /*
     * Cleanup temp surface 
     */
    if (src_converted) {
	SDL_FreeSurface(rz_src);
    }

    /*
     * Return destination surface 
     */
    return (rz_dst);
}

/* 
 
 zoomSurface()

 Zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'zoomx' and 'zoomy' are scaling factors for width and height. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

#define VALUE_LIMIT	0.001

 // Zooming Size
void ZoomSurfaceSize(int width, int height, double zoomx, double zoomy, int *dstwidth, int *dstheight)// Zooming Size V.1.0
{
    /*
     * Sanity check zoom factors 
     */
    if (zoomx < VALUE_LIMIT) 
	{
		zoomx = VALUE_LIMIT;
    }
    if (zoomy < VALUE_LIMIT)
	{
		zoomy = VALUE_LIMIT;
    }

    /*
     * Calculate target size 
     */
    *dstwidth = (int) ((double) width * zoomx);
    *dstheight = (int) ((double) height * zoomy);
    if (*dstwidth < 1) 
	{
		*dstwidth = 1;
    }
    if (*dstheight < 1) 
	{
		*dstheight = 1;
    }
}

 // Zooming
SDL_Surface *ZoomSurface(SDL_Surface * src, double zoomx, double zoomy, int smooth)  // Zooming V.1.0
{
    SDL_Surface *rz_src;
    SDL_Surface *rz_dst;
    int dstwidth, dstheight;
    int is32bit;
    int i, src_converted;

    /*
     * Sanity check 
     */
    if (src == NULL)
	return (NULL);

    /*
     * Determine if source surface is 32bit or 8bit 
     */
    is32bit = (src->format->BitsPerPixel == 32);
    if ((is32bit) || (src->format->BitsPerPixel == 8)) {
	/*
	 * Use source surface 'as is' 
	 */
	rz_src = src;
	src_converted = 0;
    } 
	else
	{
	/*
	 * New source surface is 32bit with a defined RGBA ordering 
	 */
	rz_src =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	SDL_BlitSurface(src, NULL, rz_src, NULL);
	src_converted = 1;
	is32bit = 1;
    }

    /* Get size if target */
    ZoomSurfaceSize(rz_src->w, rz_src->h, zoomx, zoomy, &dstwidth, &dstheight);

    /*
     * Alloc space to completely contain the zoomed surface 
     */
    rz_dst = NULL;
    if (is32bit) {
	/*
	 * Target surface is 32bit with source RGBA/ABGR ordering 
	 */
	rz_dst =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				 rz_src->format->Rmask, rz_src->format->Gmask,
				 rz_src->format->Bmask, rz_src->format->Amask);
    } else {
	/*
	 * Target surface is 8bit 
	 */
	rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
    }

    /*
     * Lock source surface 
     */
    SDL_LockSurface(rz_src);
    /*
     * Check which kind of surface we have 
     */
    if (is32bit) {
	/*
	 * Call the 32bit transformation routine to do the zooming (using alpha) 
	 */
	ZoomSurfaceRGBA(rz_src, rz_dst, smooth);
	/*
	 * Turn on source-alpha support 
	 */
	SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
    } else {
	/*
	 * Copy palette and colorkey info 
	 */
	for (i = 0; i < rz_src->format->palette->ncolors; i++) {
	    rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	}
	rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	/*
	 * Call the 8bit transformation routine to do the zooming 
	 */
	ZoomSurfaceY(rz_src, rz_dst);
	SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
    }
    /*
     * Unlock source surface 
     */
    SDL_UnlockSurface(rz_src);

    /*
     * Cleanup temp surface 
     */
    if (src_converted) {
	SDL_FreeSurface(rz_src);
    }

    /*
     * Return destination surface 
     */
    return (rz_dst);
}
