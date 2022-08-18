#ifndef __FIMAGE_H__
#define __FIMAGE_H__
// floating point rgb
class FImage
{
  private: float *data;

  public: int width, height;

  FImage(PImage *source);

  FImage(const FImage *other);

  FImage(int w, int h);

  ~FImage();

  void setPImage(PImage *source);

  PImage *toPImage();

  private: void setSingle(int x, int y, int c, float v);

  private: float getSingle(int x, int y, int c);

  public: void get(int x, int y, float vec3[3]);

  // bilinear interpolation
  void get(float xp, float yp, float vec3[3]);

  void set(int x, int y, float r, float g, float b);

  void set(int x, int y, float vec3[3]);

  public: int border(int pos, int axisLength);
};
#endif
