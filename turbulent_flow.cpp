#include "header.h"

FImage *turbulent_flow(
 FImage *srgb_color_image,
 int octaves,
 float persistence,
 float frequency0,
 float beta
)

{

 int w = srgb_color_image->width;
 int h = srgb_color_image->height;

 FImage *srgb_texture_image = new FImage(w, h);

 FastNoise myNoise; // Create a FastNoise object

 myNoise.SetInterp(FastNoise::Quintic);
 myNoise.SetNoiseType(FastNoise::Perlin);

 for (int y = 0; y < h; y++)
 {
     for (int x = 0; x < w; x++)
     {

         float total = 0.0;
         float frequency = frequency0;
         float amplitude = 1.0;
         float maxValue = 0.0; //Used for normalizing result from 0.0 to 1.0
         for ( int i= 0 ; i< octaves ; i++ ) {
            myNoise.SetFrequency(frequency);
            float noise = myNoise.GetNoise(x,y);
            //fprintf(stdout,"noise = %f\n",noise);
            //all outputs are approximately bounded from -1.0 to 1.0
            //so we need to scale them so that they are from 0.0 to 1.0
            noise = (noise+1.)/2.;

            //make sure noise is between 0.0 and 1.0
            if ( noise < 0.0 )
             noise= 0.0;
            if ( noise > 1.0 )
             noise= 1.0;

            noise = noise * amplitude;
            total += noise;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2.0;
         } 
         float noise = total / maxValue;
         srgb_texture_image->set(x, y, noise, noise, noise);
     }
 }

  PImage *Pimage = srgb_texture_image->toPImage();
  Pimage->write((char *)"srgb_texture_image.png");

  FImage *srgb_color_image_new = modify_color(
   srgb_color_image,
   srgb_texture_image,
   beta
  );

  return srgb_color_image_new;

}
