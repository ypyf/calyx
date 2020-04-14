#ifndef __simple_graphics_h__
#define __simple_graphics_h__

#pragma once

#include <math.h>
#include <gl/gl.h>
#include <gl/GLU.h>

namespace base
{

#define MIN3(x,y,z)  ((y) <= (z) ? \
	((x) <= (y) ? (x) : (y)) \
	: \
	((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
	((x) >= (y) ? (x) : (y)) \
	: \
	((x) >= (z) ? (x) : (z)))

    struct rgb_color {
        double r, g, b;    /* Channel intensities between 0.0 and 1.0 */
    };

    struct hsl_color {
        double hue;
        double sat;
        double lig;
    };

    struct hsv_color {
        double hue;        /* Hue degree between 0.0 and 360.0 */
        double sat;        /* Saturation between 0.0 (gray) and 1.0 */
        double val;        /* Value between 0.0 (black) and 1.0 */
    };

    //rgb_color HSVtoRGB(hsv_color in)
    //{
    //	double      hh, p, q, t, ff;
    //	long        i;
    //	rgb_color	out;

    //	if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
    //		if(isnan(in.h)) {   // in.h == NAN
    //			out.r = in.v;
    //			out.g = in.v;
    //			out.b = in.v;
    //			return out;
    //		}
    //		// error - should never happen
    //		out.r = 0.0;
    //		out.g = 0.0;
    //		out.b = 0.0;
    //		return out;
    //	}
    //	hh = in.h;
    //	if(hh >= 360.0) hh = 0.0;
    //	hh /= 60.0;
    //	i = (long)hh;
    //	ff = hh - i;
    //	p = in.v * (1.0 - in.s);
    //	q = in.v * (1.0 - (in.s * ff));
    //	t = in.v * (1.0 - (in.s * (1.0 - ff)));

    //	switch(i) {
    //	case 0:
    //		out.r = in.v;
    //		out.g = t;
    //		out.b = p;
    //		break;
    //	case 1:
    //		out.r = q;
    //		out.g = in.v;
    //		out.b = p;
    //		break;
    //	case 2:
    //		out.r = p;
    //		out.g = in.v;
    //		out.b = t;
    //		break;

    //	case 3:
    //		out.r = p;
    //		out.g = q;
    //		out.b = in.v;
    //		break;
    //	case 4:
    //		out.r = t;
    //		out.g = p;
    //		out.b = in.v;
    //		break;
    //	case 5:
    //	default:
    //		out.r = in.v;
    //		out.g = p;
    //		out.b = q;
    //		break;
    //	}
    //	return out;     
    //}

    //rgb_color HSLtoRGB(double h, double s, double l)
    //{
    //	double r, g, b;
    //	double c, x, m;
    //	rgb_color rgb;

    //	c = (1.0 - fabs(2.0*l - 1.0)) * s;
    //	x = c * (1.0 - fabs(fmod((h/60.0), 2.0) - 1.0));
    //	m = l - c / 2.0;
    //	if (0.0 <= h && h < 60.0)
    //	{
    //		r = c; g = x; b = 0.0;
    //		rgb.r = r + m;
    //		rgb.g = g + m;
    //		rgb.b = b + m;
    //		return rgb;
    //	}
    //	if (60.0 <= h && h < 120.0)
    //	{
    //		r = x; g = c; b = 0.0;
    //		rgb.r = r + m;
    //		rgb.g = g + m;
    //		rgb.b = b + m;
    //		return rgb;
    //	}
    //	if (120.0 <= h && h < 180.0)
    //	{
    //		r = 0.0; g = c; b = x;
    //		rgb.r = r + m;
    //		rgb.g = g + m;
    //		rgb.b = b + m;
    //		return rgb;
    //	}
    //	if (180.0 <= h && h < 240.0)
    //	{
    //		r = 0.0; g = x; b = c;
    //		rgb.r = r + m;
    //		rgb.g = g + m;
    //		rgb.b = b + m;
    //		return rgb;
    //	}
    //	if (240.0 <= h && h < 300.0)
    //	{
    //		r = x; g = 0.0; b = c;
    //		rgb.r = r + m;
    //		rgb.g = g + m;
    //		rgb.b = b + m;
    //		return rgb;
    //	}
    //	if (300.0 <= h && h < 360.0)
    //	{
    //		r = c; g = 0.0; b = x;
    //		rgb.r = r + m;
    //		rgb.g = g + m;
    //		rgb.b = b + m;
    //		return rgb;
    //	}
    //}

    inline void putPixel(int x, int y, float color[])
    {
        glColor3fv(color);
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();
    }

    inline void drawLine()
    {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(15, 0, 0);
        glEnd();
    }
} // namespace end


#endif // __simple_graphics_h__