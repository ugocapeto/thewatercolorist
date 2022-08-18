#include "header.h"
float illuminant[3] = {0.95047f, 1.00000f, 1.08883f}; // D65
float f(float t)
{
  return (t > powf(6.f/29.f, 3.f)) ? powf(t, 1.f/3.f) : (1.f/3.f) * powf(29.f/6.f, 2.f) * t + (4.f/29.f);
}

float fi(float t)
{
  return (t > 6.f/29.f) ? powf(t, 3.f) : 3.f * powf(6.f/29.f, 2.f) * (t - (4.f/29.f));
}

FImage *convert_srgb2Lab(FImage *input)
{
  int w = input->width;
  int h = input->height;

  FImage *out = new FImage(w, h);

  float X, Y, Z;

  for (int y = 0; y < h; y++) 
  {
    for (int x = 0; x < w; x++) 
    {
      float rgb[3];
      input->get(x, y, rgb);

      float r = rgb[0];
      float g = rgb[1];
      float b = rgb[2];

      // RGB to XYZ
      // assuming sRGB (D65)
      if (r <= 0.04045f)
        r = r/12.92f;
      else
        r =  powf(((r + 0.055f) / 1.055f), 2.4f);

      if (g <= 0.04045f)
        g = g/12.92f;
      else
        g =  powf(((g + 0.055f) / 1.055f), 2.4f);

      if (b <= 0.04045f)
        b = b/12.92f;
      else
        b =  powf(((b + 0.055f) / 1.055f), 2.4f);

      //sRGB D65, http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
      float RGB2XYZ_MATRIX[3][3] =
        {
        { 0.4124564f, 0.3575761f, 0.1804375f }, 
        { 0.2126729f, 0.7151522f, 0.0721750f }, 
        { 0.0193339f, 0.1191920f, 0.9503041f }
      };

      X = RGB2XYZ_MATRIX[0][0] * r + RGB2XYZ_MATRIX[0][1] * g + RGB2XYZ_MATRIX[0][2] * b;
      Y = RGB2XYZ_MATRIX[1][0] * r + RGB2XYZ_MATRIX[1][1] * g + RGB2XYZ_MATRIX[1][2] * b;
      Z = RGB2XYZ_MATRIX[2][0] * r + RGB2XYZ_MATRIX[2][1] * g + RGB2XYZ_MATRIX[2][2] * b;

      // XYZ to Lab      
      out->set(x, y, 
        116.f * f(Y / illuminant[1]) - 16.f, 
        500.f * (f(X/ illuminant[0]) - f(Y/ illuminant[1])), 
        200.f * (f(Y/ illuminant[1]) - f(Z/ illuminant[2])));
    }
  }
  return out;
}

FImage *convert_Lab2srgb(FImage *input)
{
  int w = input->width;
  int h = input->height;

  FImage *out = new FImage(w, h);

  float X, Y, Z, r, g, b;

  for (int y = 0; y < h; y++) 
  {
    for (int x = 0; x < w; x++) 
    {
      float Lab[3];
      input->get(x, y, Lab);

      // chromatic adaption, reference white
      Y = illuminant[1] * fi((1.f/116.f) * (Lab[0] + 16.f)); // Y
      X = illuminant[0] * fi((1.f/116.f) * (Lab[0] + 16.f) + (1.f/500.f) * Lab[1]); // X
      Z = illuminant[2] * fi((1.f/116.f) * (Lab[0] + 16.f) - (1.f/200.f) * Lab[2]); // Z

      //sRGB D65, http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
      float XYZ2RGB_MATRIX[3][3] =
        {
        { 3.2404542f, -1.5371385f, -0.4985314f }, 
        { -0.9692660f, 1.8760108f, 0.0415560f }, 
        { 0.0556434f, -0.2040259f, 1.0572252f }
      };

      r = XYZ2RGB_MATRIX[0][0] * X + XYZ2RGB_MATRIX[0][1] * Y + XYZ2RGB_MATRIX[0][2] * Z;
      g = XYZ2RGB_MATRIX[1][0] * X + XYZ2RGB_MATRIX[1][1] * Y + XYZ2RGB_MATRIX[1][2] * Z;
      b = XYZ2RGB_MATRIX[2][0] * X + XYZ2RGB_MATRIX[2][1] * Y + XYZ2RGB_MATRIX[2][2] * Z;

      if (r <= 0.0031308f)
        r = r * 12.92f;
      else
        r = 1.055f * powf(r, 1.0f / 2.4f) - 0.055f;
      if (g <= 0.0031308f)
        g = g * 12.92f;
      else
        g = 1.055f * powf(g, 1.0f / 2.4f) - 0.055f;
      if (b <= 0.0031308f)
        b = b * 12.92f;
      else
        b = 1.055f * powf(b, 1.0f / 2.4f) - 0.055f;
        
      out->set(x, y, r, g, b);
    }
  }
  return out;
}

FImage *convert_srgb2HSL(FImage *input)
{
  int w = input->width;
  int h = input->height;

  FImage *out = new FImage(w, h);

  RGBConverter duh;

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      float rgb[3];
      input->get(x, y, rgb);

      double hsl[3];
      duh.rgbToHsl((double)rgb[0], (double)rgb[1], (double)rgb[2], hsl);

      out->set(x, y, (float)hsl[0], (float)hsl[1], (float)hsl[2]);
    }
  }
  return out;
 }

FImage *convert_HSL2srgb(FImage *input)
{
  int w = input->width;
  int h = input->height;

  FImage *out = new FImage(w, h);

  RGBConverter duh;

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      float hsl[3];
      input->get(x, y, hsl);

      double rgb[3];
      duh.hslToRgb((double)hsl[0], (double)hsl[1], (double)hsl[2], rgb);

      out->set(x, y, (float)rgb[0], (float)rgb[1], (float)rgb[2]);
    }
  }
  return out;
 }

FImage *convert_srgb2HSV(FImage *input)
{
  int w = input->width;
  int h = input->height;

  FImage *out = new FImage(w, h);

  RGBConverter duh;

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      float rgb[3];
      input->get(x, y, rgb);

      double hsv[3];
      duh.rgbToHsv((double)rgb[0], (double)rgb[1], (double)rgb[2], hsv);

      out->set(x, y, (float)hsv[0], (float)hsv[1], (float)hsv[2]);
    }
  }
  return out;
 }

FImage *convert_HSV2srgb(FImage *input)
{
  int w = input->width;
  int h = input->height;

  FImage *out = new FImage(w, h);

  RGBConverter duh;

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      float hsv[3];
      input->get(x, y, hsv);

      double rgb[3];
      duh.hsvToRgb((double)hsv[0], (double)hsv[1], (double)hsv[2], rgb);

      out->set(x, y, (float)rgb[0], (float)rgb[1], (float)rgb[2]);
    }
  }
  return out;
 }
