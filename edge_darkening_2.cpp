#include "header.h"

FImage *edge_darkening_2(
 FImage *srgb_color_image,
 FImage *srgb_color_image2,
 int n,
 float beta
)

{

 int i;
 int j;
 int k;

  // compute kernel for gradient in y direction
  int *kernel = new int[n*n];
  k = -1;
  for ( i = 0 ; i < n/2 ; i++ ) {
     int kk = k;
     for ( j = 0 ; j < n/2 ; j++ ) {
        kernel[i*n+j] = kk;
        kernel[(n-1-i)*n+j] =-kk;
        kernel[i*n+(n-1-j)] = kk;
        kernel[(n-1-i)*n+(n-1-j)] =-kk;
        kk--;
     }
     j = n/2;
     kernel[i*n+j] = kk;
     kernel[(n-1-i)*n+j] =-kk;
     k--;
  }
  i = n/2;
  for ( j = 0 ; j < n ; j++ ) {
     kernel[i*n+j] = 0;
  }

  int w = srgb_color_image->width;
  int h = srgb_color_image->height;

  FImage *srgb_texture_image = new FImage(w, h);

  float min_grad = +1.0e32;
  float max_grad = -1.0e32;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {

      //compute gradient in the x direction for all 3 rgb channels
      float gradx3[3];
      for ( int cind= 0 ; cind< 3 ; cind++ )
       gradx3[cind] = 0.0;
      for ( i = 0 ; i < n ; i++ ) {
         int i2 = i - n/2;
         int y2 = y + i2;

         if ( !(y2 >= 0) )
          y2 = 0;
         if ( !(y2 < h) )
          y2 = h-1;

         for ( j = 0 ; j < n ; j++ ) {
            int j2 = j - n/2;
            int x2 = x + j2;
            //kernel for the gradient in the x direction is the transpose of the
            //kernel for the gradient in the y direction
            int val = kernel[j*n+i];

            if ( !(x2 >= 0) )
             x2 = 0;
            if ( !(x2 < w) )
             x2 = w-1;

            float rgb3[3];
            srgb_color_image->get(x2, y2, rgb3);
            for ( int cind = 0 ; cind< 3 ; cind++ ) {
               gradx3[cind] += (float)val * rgb3[cind]; 
            }
         }
      } 

      //compute gradient in the y direction for all 3 rgb channels
      float grady3[3];
      for ( int cind= 0 ; cind< 3 ; cind++ )
       grady3[cind] = 0.0;
      for ( i = 0 ; i < n ; i++ ) {
         int i2 = i - n/2;
         int y2 = y + i2;

         if ( !(y2 >= 0) ) 
          y2 = 0;
         if ( !(y2 < h) )
          y2 = h-1;

         for ( j = 0 ; j < n ; j++ ) {
            int j2 = j - n/2;
            int x2 = x + j2;
            int val = kernel[i*n+j];

            if ( !(x2 >= 0) )
             x2 = 0;
            if ( !(x2 < w) )
             x2 = w-1;

            float rgb3[3];
            srgb_color_image->get(x2, y2, rgb3);
            for ( int cind = 0 ; cind< 3 ; cind++ ) {
               grady3[cind] += (float)val * rgb3[cind];
            }
         }
      }

      float grad3[3];
      for ( int cind= 0 ; cind< 3 ; cind++ )
       grad3[cind] = fabsf(gradx3[cind]) + fabsf(grady3[cind]);

      float grad = (grad3[0] + grad3[1] + grad3[2] ) / 3.0;

      srgb_texture_image->set(x, y, grad, grad, grad);

      if ( grad < min_grad )
       min_grad = grad;
      if ( grad > max_grad ) 
       max_grad = grad;
    }
  }

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {

      float grad3[3];
      srgb_texture_image->get(x, y, grad3);

      float grad = grad3[0];

      float val = (grad - min_grad) / (max_grad - min_grad); 

      val = 1.0 - val;
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
