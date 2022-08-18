#ifndef __PIMAGE_H__
#define __PIMAGE_H__
class PImage
{

  public: int width;

  public: int height;

  public: int *pixels;

  PImage(int w, int h);

  PImage(char *filename);

  ~PImage();

  void set(int x, int y, int intensity);

  void set(int x, int y, int r, int g, int b);

  void filter(char *kind, float param);

  void get(int x, int y, int vec3[3]);

  void write(char *filename);

};
#endif
