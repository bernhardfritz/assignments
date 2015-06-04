#ifndef __COLOR_CONVERSION_H__
#define __COLOR_CONVERSION_H__

typedef struct {
  double r;       // percent
  double g;       // percent
  double b;       // percent
} rgb;

typedef struct {
  double h;       // angle in degrees
  double s;       // percent
  double v;       // percent
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

#endif // __COLOR_CONVERSION_H__
