#include "header.h"

void thewatercolorist_main(
 char *filename_image,
 char *filename_paper_texture,
 float beta_paper_texture,
 float beta_turbulent_flow,
 int n_edge_darkening,
 float beta_edge_darkening,
 char *filename_output_image
)

{

 int octaves;
 float persistence = 0.5;
 float frequency0;

 PImage *Pimage = new PImage(filename_image);
 FImage *Fimage = new FImage(Pimage);

 //apply paper texture

 fprintf(stdout,"Applying paper texture ...\n");

 FImage *Fimage2 = paper_texture(
  Fimage,
  filename_paper_texture,
  beta_paper_texture
 );

 PImage *Pimage2 = Fimage2->toPImage();
 Pimage2->write((char *)"image_after_paper_texture.png");

 fprintf(stdout,"Applying paper texture ... done.\n");

 //apply turbulent flow

 fprintf(stdout,"Applying turbulent flow ...\n");

 octaves = 6;
 frequency0 = 0.01;
 FImage *Fimage3 = turbulent_flow(
  Fimage2,
  octaves,
  persistence,
  frequency0,
  beta_turbulent_flow
 );

 PImage *Pimage3 = Fimage3->toPImage();
 Pimage3->write((char *)"image_after_turbulent_flow.png");

 fprintf(stdout,"Applying turbulent flow ... done.\n");

 //apply edge darkening

 fprintf(stdout,"Applying edge darkening ...\n");

 //FImage *Fimage4 = edge_darkening(
 // Fimage,
 // Fimage3,
 // beta_edge_darkening
 //);
 FImage *Fimage4 = edge_darkening_2(
  Fimage,
  Fimage3,
  n_edge_darkening,
  beta_edge_darkening
 );

 PImage *Pimage4 = Fimage4->toPImage();
 Pimage4->write((char *)"image_after_edge_darkening.png");

 fprintf(stdout,"Applying edge darkening ... done.\n");

 Pimage4->write(filename_output_image);

}
