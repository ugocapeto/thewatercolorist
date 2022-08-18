#include "header.h"

FImage *edge_darkening(
 FImage *srgb_color_image,
 FImage *srgb_color_image2,
 float beta
)

{

  float rgb3b[3];
  float rgb3c[3];

  int w = srgb_color_image->width;
  int h = srgb_color_image->height;

  FImage *srgb_texture_image = new FImage(w, h);

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {

      float rgb3[3];
      srgb_color_image->get(x, y, rgb3);

      //compute gradient in the x direction for all 3 rgb channels
      float gradx3[3];
      int x2 = x-1;
      if ( !(x2 >= 0) )
       x2 = 0;
      srgb_color_image->get(x2, y, rgb3b);
      x2 = x+1;
      if ( !(x2 < w) )
       x2 = w-1;
      srgb_color_image->get(x2, y, rgb3c);
      for ( int cind= 0 ; cind< 3 ; cind++ )
       gradx3[cind] = rgb3c[cind] - rgb3b[cind];

      //compute gradient in the y direction for all 3 rgb channels
      float grady3[3];
      int y2 = y-1;
      if ( !(y2 >= 0) )
       y2 = 0;
      srgb_color_image->get(x, y2, rgb3b);
      y2 = y+1;
      if ( !(y2 < h) )
       y2 = h-1;
      srgb_color_image->get(x, y2, rgb3c);
      for ( int cind= 0 ; cind< 3 ; cind++ )
       grady3[cind] = rgb3c[cind] - rgb3b[cind];

      float grad3[3];
      for ( int cind= 0 ; cind< 3 ; cind++ )
       grad3[cind] = fabsf(gradx3[cind]) + fabsf(grady3[cind]);

      float grad = (grad3[0] + grad3[1] + grad3[2] ) / 3.0;

      float val = 1.0 - grad;
      val = val / 2.0;

      srgb_texture_image->set(x, y, val, val, val);
    }
  }

  FImage *srgb_color_image_new = modify_color(
   srgb_color_image2,
   srgb_texture_image,
   beta
  );

  return srgb_color_image_new;

}
