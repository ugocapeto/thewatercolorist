#include "header.h"
// floating point rgb
  FImage::FImage(PImage *source)
  {
    data = (float *)0;
    setPImage(source);
  }

  FImage::FImage(const FImage *other)
  {
    width = other->width;
    height = other->height;
    data = new float[width*height*3];
    for (int i = 0; i < width*height*3; i++)
    {
      data[i] = other->data[i];
    }
  }

  FImage::FImage(int w, int h)
  {
    width = w;
    height = h;
    data = new float[width*height*3];
  }

  FImage::~FImage()
  {
    if (data != 0)
     free(data);
  }

  void FImage::setPImage(PImage *source)
  {
    if ( data != (float *)0 )
     delete [] data;
    width = source->width;
    height = source->height;
    data = new float[width*height*3];
    for (int i = 0; i < width*height*3; i++)
    {
      data[i] = source->pixels[i] / 255.f;
    }
  }

  PImage *FImage::toPImage()
  {
    PImage *image = new PImage(width, height);
    for (int i = 0; i < width*height*3; i++)
    {
      image->pixels[i] = data[i]*255.f;
      if ( !(image->pixels[i] >= 0) ) {
        //fprintf(stdout,"rgb value is out of bounds!\n");
        //force abnormal process termination
        //abort();
        image->pixels[i] = 0;
      }
      if ( !(image->pixels[i] <= 255) ) {
        //fprintf(stdout,"rgb value is out of bounds!\n");
        //force abnormal process termination
        //abort();
        image->pixels[i] = 255;
      }
    }
    return image;
  }

  void FImage::setSingle(int x, int y, int c, float v)
  {
    data[3*(y*width+x)+c] = v;
  }

  float FImage::getSingle(int x, int y, int c)
  {
    return data[3*(y*width+x)+c];
  }

  void FImage::get(int x, int y, float vec3[3])
  {
    vec3[0]= getSingle(x, y, 0);
    vec3[1]= getSingle(x, y, 1);
    vec3[2]= getSingle(x, y, 2);
  }

  // bilinear interpolation
  void FImage::get(float xp, float yp, float vec3[3])
  {
    int x = floor(xp);
    int y = floor(yp);

    int x0 = border(x, width);
    int x1 = border(x+1, width);
    int y0 = border(y, height);
    int y1 = border(y+1, height);

    float a = xp - (float)(x);
    float c = yp - (float)(y);

    vec3[0] = (getSingle(x0, y0, 0) * (1.f - a) + getSingle(x1, y0, 0) * a) * (1.f - c)
      + (getSingle(x0, y1, 0) * (1.f - a) + getSingle(x1, y1, 0) * a) * c;
    vec3[1] = (getSingle(x0, y0, 1) * (1.f - a) + getSingle(x1, y0, 1) * a) * (1.f - c)
      + (getSingle(x0, y1, 1) * (1.f - a) + getSingle(x1, y1, 1) * a) * c;
    vec3[2] = (getSingle(x0, y0, 2) * (1.f - a) + getSingle(x1, y0, 2) * a) * (1.f - c)
      + (getSingle(x0, y1, 2) * (1.f - a) + getSingle(x1, y1, 2) * a) * c;
  }

  void FImage::set(int x, int y, float r, float g, float b)
  {
    setSingle(x, y, 0, r);
    setSingle(x, y, 1, g);
    setSingle(x, y, 2, b);
  }

  void FImage::set(int x, int y, float vec3[3])
  {
    setSingle(x, y, 0, vec3[0]);
    setSingle(x, y, 1, vec3[1]);
    setSingle(x, y, 2, vec3[2]);
  }

  int FImage::border(int pos, int axisLength)
  {
    // pos lies in image
    if (pos < axisLength && pos >= 0) return pos;

    if (axisLength == 1) return 0;

    do
    {
      if (pos < 0)
      {
        pos = -pos - 1;
      } else
      {
        pos = axisLength - 1 - (pos  - axisLength);
      }
    }  
    while ( abs(pos) >= abs(axisLength) );

    return pos;
  }
