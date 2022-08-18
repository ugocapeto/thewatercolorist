#include "header.h"

FImage *paper_texture(
 FImage *srgb_color_image,
 char *filename_paper_texture,
 float beta
)

{

 //char command[256];

 int w = srgb_color_image->width;
 int h = srgb_color_image->height;

 //let's use imagemagick to resize/scale the paper texture
 //so that it matches srgb_color_image's dimensions
 //sprintf(command,"convert %s -resize %dx%d! -colorspace gray resize_paper_texture.png",
 // filename_paper_texture,w,h);
 //system(command);

 //because system calls are a nighmare on windows 64bit under mingw,
 //it's best to use my own function to resize/scale the paper texture
 resize_rgb_image(
  filename_paper_texture,
  w,
  h,
  (char *)"resize_paper_texture.png"
 );

 PImage *Pimage = new PImage((char *)"resize_paper_texture.png");
 FImage *srgb_texture_image = new FImage(Pimage);

  //Do not allow lightening
  //by shifting the intensities
  //Actually, I think it looks better if the colors are lightened
  //So, I am commenting out this code
  /*
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {

      float texture3[3];
      srgb_texture_image->get(x, y, texture3);
      float texture = texture3[0];

      texture = texture / 2.0;

      srgb_texture_image->set(x, y, texture, texture, texture);
    }
  }
  */

 FImage *srgb_color_image_new = modify_color(
  srgb_color_image,
  srgb_texture_image,
  beta
 );

 return srgb_color_image_new;

}
