#include "header.h"

FImage *modify_color_hsv(
 FImage *srgb_color_image,
 FImage *srgb_texture_image,
 float beta
)

//srgb_texture_image is a grayscale image (stored as srgb image)
//intensity varies from 0.0 to 1.0 (all 3 channels have same intensity)
//if intensity is 0.5, the color in srgb_color_image is unchanged
//if intensity is < 0.5, the color in srgb_color_image is darkened
//if intensity is > 0.5, the color in srgb_color_image is lightened

{

  //print srgb_color_image
  PImage *Pimage = srgb_color_image->toPImage();
  Pimage->write((char *)"srgb_color_image.png");

  //print srgb_texture_image
  PImage *Pimage2 = srgb_texture_image->toPImage();
  Pimage2->write((char *)"srgb_texture_image.png");

  int w = srgb_color_image->width;
  int h = srgb_color_image->height;

  //convert srgb_color_image to HSV_color_image
  FImage *HSV_color_image = convert_srgb2HSV(srgb_color_image);

  //convert HSV_color_image to srgb_color_image2
  FImage *srgb_color_image2 = convert_HSV2srgb(HSV_color_image);
  //print srgb_color_image2
  PImage *Pimage3 = srgb_color_image2->toPImage();
  Pimage3->write((char *)"srgb_color_image2.png");

  FImage *HSV_color_image_new = new FImage(w, h);

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {

      float hsv3[3];
      HSV_color_image->get(x, y, hsv3);

      float texture3[3];
      srgb_texture_image->get(x, y, texture3);
      float texture = texture3[0];

      //compute the density
      float d = 1.0 + beta*( texture - 0.5);

      float hsv3_new[3];
      hsv3_new[0] = hsv3[0];
      float C;
      float Cp;

      //compute the new saturation (the S in HSV)
      C = hsv3[1];
      Cp = C * ( 1.0 - ( 1.0 - C ) * ( d - 1.0 ) );
      Cp = C;
      hsv3_new[1] = Cp;

      //compute the new value (the V in HSV)
      C = hsv3[2];
      Cp = C * ( 1.0 - ( 1.0 - C ) * ( d - 1.0 ) );
      //Cp = C;
      hsv3_new[2] = Cp;

      HSV_color_image_new->set(x, y, hsv3_new);
    }
  }

  //convert HSV_color_image_new to srgb_color_image_new
  FImage *srgb_color_image_new = convert_HSV2srgb(HSV_color_image_new);
  //print srgb_color_image_new
  PImage *Pimage4 = srgb_color_image_new->toPImage();
  Pimage4->write((char *)"srgb_color_image_new.png");

  return srgb_color_image_new;
}
