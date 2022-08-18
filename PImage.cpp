#include "header.h"
// integer rgb
  PImage::PImage(int w, int h)
  {
    width = w;
    height = h;
    pixels = new int[width*height*3];
  }

  PImage::PImage(char *filename)
  {
    int err_flag= load_rgb_image(
     filename,
     &pixels,
     &width,
     &height
    );

    if ( err_flag == 1 ) {
      fprintf(stdout,"Could not load the rgb image in file %s!\n",filename);
      //force abnormal process termination
      abort();
    }
  }

  PImage::~PImage()
  {
    fprintf(stdout,"Destructing PImage object!\n");
    if (pixels != 0)
     free(pixels);
  }

  void PImage::set(int x, int y, int intensity)
  {
    set(x , y, intensity, intensity, intensity);
  }

  void PImage::set(int x, int y, int r, int g, int b)
  {
    if ( !(r >= 0) ) {
      //fprintf(stdout,"red channel intensity is out of bounds!\n");
      //force abnormal process termination
      //abort();
      r = 0;
    }
    if ( !(r <= 255) ) {
      //fprintf(stdout,"red channel intensity is out of bounds!\n");
      //force abnormal process termination
      //abort();
      r = 255;
    }
    if ( !(g >= 0) ) {
      //fprintf(stdout,"green channel intensity is out of bounds!\n");
      //force abnormal process termination
      //abort();
      g = 0;
    }
    if ( !(g <= 255) ) {
      //fprintf(stdout,"green channel intensity is out of bounds!\n");
      //force abnormal process termination
      //abort();
      g = 255;
    }
    if ( !(b >= 0) ) {
      //fprintf(stdout,"blue channel intensity is out of bounds!\n");
      //force abnormal process termination
      //abort();
      b = 0;
    }
    if ( !(b <= 255) ) {
      //fprintf(stdout,"blue channel intensity is out of bounds!\n");
      //force abnormal process termination
      //abort();
      b = 255;
    }
    pixels[3*(y*width+x)+0] = r;
    pixels[3*(y*width+x)+1] = g;
    pixels[3*(y*width+x)+2] = b;
  }

  void PImage::filter(char *kind, float param)
  {
    if ( strcmp(kind,"THRESHOLD") == 0 ) {
      for ( int i= 0 ; i< width*height*3 ; i++ ) {
         int intensity_int= pixels[i];
         float intensity_float= (float)intensity_int/255.f;
         if ( intensity_float < param )
          pixels[i]= 0;
         else
          pixels[i]= 255;
      }
    }
    else {
      fprintf(stdout,"Sorry, not implemented yet!\n");
      //force abnormal process termination
      abort();
    }
  }

  void PImage::get(int x, int y, int vec3[3])
  {
    vec3[0] = pixels[3*(y*width+x)+0];
    vec3[1] = pixels[3*(y*width+x)+1];
    vec3[2] = pixels[3*(y*width+x)+2];
  }

  void PImage::write(char *filename)
  {
    int err_flag= write_rgb_image(
     filename,
     pixels,
     width,
     height
    );

    if ( err_flag == 1 ) {
      fprintf(stdout,"Could not write the rgb image to file %s!\n",filename);
      //force abnormal process termination
      abort();
    }
  }
