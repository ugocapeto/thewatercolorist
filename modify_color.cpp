#include "header.h"

FImage *modify_color(
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

 //use the HSL color space
 FImage *srgb_color_image_new = modify_color_hsl(
  srgb_color_image,
  srgb_texture_image,
  beta
 );

 //use the HSV color space
 //FImage *srgb_color_image_new = modify_color_hsv(
 // srgb_color_image,
 // srgb_texture_image,
 // beta
 //);

 return srgb_color_image_new;

}
