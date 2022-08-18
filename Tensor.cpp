#include "header.h"
FImage *computeStructureTensors(FImage *img)
{
  int w = img->width;
  int h = img->height;

  FImage *tensors = new FImage(w, h); 
  for (int y = 0; y < h; y++) 
  {
    for (int x = 0; x < w; x++) 
    {
      float uv[3];      
      uv[0] = x;      
      uv[1] = y;      
      uv[2] = 0.;      
      float d[3];
      d[0] = 1;
      d[1] = 1;
      d[2] = 0.;

      float dx[3];
      float dy[3];

      float a3[3];
      float b3[3];
      float c3[3];
      float d3[3];
      float e3[3];
      float f3[3];

      img->get(uv[0] - d[0], uv[1] - d[1], a3);
      img->get(uv[0] - d[0], uv[1]       , b3);
      img->get(uv[0] - d[0], uv[1] + d[1], c3);
      img->get(uv[0] + d[0], uv[1] - d[1], d3);
      img->get(uv[0] + d[0], uv[1]       , e3);
      img->get(uv[0] + d[0], uv[1] + d[1], f3);

      dx[0] = (
        -1.0 * a3[0] +
        -2.0 * b3[0] + 
        -1.0 * c3[0] +
        +1.0 * d3[0] +
        +2.0 * e3[0] + 
        +1.0 * f3[0]
      ) / 4.0;

      dx[1] = (
        -1.0 * a3[1] +
        -2.0 * b3[1] +                           
        -1.0 * c3[1] +
        +1.0 * d3[1] +
        +2.0 * e3[1] +                           
        +1.0 * f3[1]
      ) / 4.0;

      dx[2] = (
        -1.0 * a3[2] +
        -2.0 * b3[2] +                           
        -1.0 * c3[2] +
        +1.0 * d3[2] +
        +2.0 * e3[2] +                           
        +1.0 * f3[2]
      ) / 4.0;

      img->get(uv[0] - d[0], uv[1] - d[1], a3);
      img->get(uv[0]       , uv[1] - d[1], b3);
      img->get(uv[0] + d[0], uv[1] - d[1], c3);
      img->get(uv[0] - d[0], uv[1] + d[1], d3);
      img->get(uv[0]       , uv[1] + d[1], e3);
      img->get(uv[0] + d[0], uv[1] + d[1], f3);

      dy[0] = (
        -1.0 * a3[0] +
        -2.0 * b3[0] + 
        -1.0 * c3[0] +
        +1.0 * d3[0] +
        +2.0 * e3[0] + 
        +1.0 * f3[0]
      ) / 4.0;

      dy[1] = (
        -1.0 * a3[1] +
        -2.0 * b3[1] +                           
        -1.0 * c3[1] +
        +1.0 * d3[1] +
        +2.0 * e3[1] +                           
        +1.0 * f3[1]
      ) / 4.0;

      dy[2] = (
        -1.0 * a3[2] +
        -2.0 * b3[2] +                           
        -1.0 * c3[2] +
        +1.0 * d3[2] +
        +2.0 * e3[2] +                           
        +1.0 * f3[2]
      ) / 4.0;

      float E;
      E = dx[0]*dx[0]+
          dx[1]*dx[1]+
          dx[2]*dx[2];
      float F;
      F = dx[0]*dy[0]+
          dx[1]*dy[1]+
          dx[2]*dy[2];
      float G;
      G = dy[0]*dy[0]+
          dy[1]*dy[1]+
          dy[2]*dy[2];

      tensors->set(x, y, E, F, G);
    }
  }
  return tensors;
}

// major eigenvalue
float getLambda1(FImage *tensors, int x, int y)
{
  float g[3];
  tensors->get(x, y, g);
  float E = g[0];
  float F = g[1];
  float G = g[2];

  float det = sqrtf(powf(E - G, 2.f) + 4.f * F*F);
  return (E + G + det) * 0.5f;
}

// minor eigenvalue
float getLambda2(FImage *tensors, int x, int y)
{
  float g[3];
  tensors->get(x, y, g);
  float E = g[0];
  float F = g[1];
  float G = g[2];

  float det = sqrtf(powf(E - G, 2.f) + 4.f * F*F);
  return (E + G - det) * 0.5f;
}

// major eigenvector
void getV1(FImage *tensors, int x, int y, float v[3])
{
  float g[3];
  tensors->get(x, y, g);
  float E = g[0];
  float F = g[1];
  float G = g[2];

  float det = sqrtf(powf(E - G, 2.f) + 4.f * F*F);
  v[0] = 2.f*F;
  v[1] = G - E + det;
  v[2] = 0.f;
}

// minor eigenvector
void getV2(FImage *tensors, int x, int y, float v[3])
{
  float g[3];
  tensors->get(x, y, g);
  float E = g[0];
  float F = g[1];
  float G = g[2];

  float det = sqrtf(powf(E - G, 2.f) + 4.f * F*F);
  v[0] = 2.f*F;
  v[1] = G - E - det;
  v[2] = 0.f;
}

FImage *computeTangentFlowMap(FImage *tensors)
{
  int w = tensors->width;
  int h = tensors->height;

  FImage *tfm = new FImage(w, h); 
  for (int y = 0; y < h; y++) 
  {
    for (int x = 0; x < w; x++) 
    {      
      float lambda = getLambda2(tensors, x, y);
      float v[3];
      getV2(tensors, x, y, v);  

      float m = sqrtf(v[0]*v[0]+v[1]*v[1]);
      if (m > 0.f)
      {
        v[0] /= m;
        v[1] /= m;
        tfm->set(x, y, v[0], v[1], sqrtf(lambda));
      } else
      {
        tfm->set(x, y, 0, 1, 0);
      }
    }
  }
  return tfm;
}
