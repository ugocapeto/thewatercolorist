#include "header.h"
FImage *GaussianBlur(FImage *f, float sigma)
{
  float TWO_PI=2.0f*acosf(-1.0f);
  int k_size = (int)(2.0f * floorf( sqrtf(-log(0.1f) * 2 * (sigma*sigma)) ) + 1.0f);
  int r = (k_size - 1) / 2;
  float sigmasquare = sigma * sigma;

  // compute gaussian kernel
  float *kernel = new float[k_size];
  for (int i = -r; i <= r; ++i)
  {
    kernel[i+r] = expf(-i*i/(2*sigmasquare)) / (sqrtf(sigmasquare*TWO_PI));
  }

  // normalize kernel
  float sum = 0;
  for (int j = 0; j < k_size; ++j)
  {
    sum += kernel[j];
  }

  for (int j = 0; j < k_size; ++j)
  {
    kernel[j] = (kernel[j] / sum);
  }

  FImage *f2 = convolveHorizontal1D(f, kernel, k_size);
  FImage *f3 = convolveVertical1D(f2, kernel, k_size);
  return f3;
}

FImage *convolveVertical1D(FImage *f, float *kernel, int k_len) 
{
  int r = (k_len - 1) / 2;
  int w = f->width;
  int h = f->height;

  FImage *out = new FImage(w, h); 
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {        
      float sum[3];
      sum[0] = 0.f;
      sum[1] = 0.f;
      sum[2] = 0.f;

      // center pixel
      float center[3];
      f->get(x, y, center);
      sum[0] += kernel[r] * center[0];
      sum[1] += kernel[r] * center[1];
      sum[2] += kernel[r] * center[2];

      // neighbor pixels
      for (int i = 1; i <= r; ++i)    
      {
        float a[3];
        f->get(x, f->border(y-i, h), a);
        float b[3];
        f->get(x, f->border(y+i, h), b);

        sum[0] += kernel[r+i] * b[0];  
        sum[1] += kernel[r+i] * b[1];  
        sum[2] += kernel[r+i] * b[2];  

        sum[0] += kernel[r-i] * a[0];  
        sum[1] += kernel[r-i] * a[1];  
        sum[2] += kernel[r-i] * a[2];
      }
      out->set(x, y, sum);
    }
  }
  return out;
}

FImage *convolveHorizontal1D(FImage *f, float *kernel, int k_len) 
{
  int r = (k_len - 1) / 2;
  int w = f->width;
  int h = f->height;

  FImage *out = new FImage(w, h); 
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {        
      float sum[3];
      sum[0] = 0.f;
      sum[1] = 0.f;
      sum[2] = 0.f;

      // center pixel
      float center[3];
      f->get(x, y, center);
      sum[0] += kernel[r] * center[0];
      sum[1] += kernel[r] * center[1];
      sum[2] += kernel[r] * center[2];

      // neighbor pixels
      for (int i = 1; i <= r; ++i)    
      {
        float a[3];
        f->get(f->border(x-i, w), y, a);
        float b[3];
        f->get(f->border(x+i, w), y, b);

        sum[0] += kernel[r+i] * b[0];  
        sum[1] += kernel[r+i] * b[1];  
        sum[2] += kernel[r+i] * b[2];  

        sum[0] += kernel[r-i] * a[0];  
        sum[1] += kernel[r-i] * a[1];  
        sum[2] += kernel[r-i] * a[2];
      }
      out->set(x, y, sum);
    }
  }
  return out;
}
