#include "header.h"

// params
// tensor
float tensor_sigma;
// orientation aligned bilateral filter
int n_e;
int n_a;
float sigma_d;
float sigma_r;
// flow based differences of Gaussian
int fdog_n;
float fdog_sigma_e;
float fdog_tau;
float fdog_sigma_m;
float fdog_phi;
// quantization
float phi_q;
int quant_levels;

// variables
PImage *input;
FImage *tfm;
FImage *sourceRGB;
FImage *sourceLab;

int main(
 int argc,
 char* argv[]
)

{

 char filename[256];
 char filename_image[256];
 char filename_paper_texture[256];
 FILE *fp;
 float beta_paper_texture;
 float beta_turbulent_flow;
 int n_edge_darkening;
 float beta_edge_darkening;
 char filename_output_image[256];

 /*
 Let's read in the input file
 */

 fp= fopen("thewatercolorist_input.txt","r");
 if ( fp == NULL ) {
    fprintf(stdout,"thewatercolorist_input.txt not found!\n");
    return 1;
 }

 /*
 Get filename for the image
 */

 fscanf(fp,"%s",filename);

 fprintf(stdout,"image = %s\n",filename);

 strcpy(filename_image,filename);

 /*
 Get filename for the paper texture
 */

 fscanf(fp,"%s",filename);

 fprintf(stdout,"paper texture = %s\n",filename);

 strcpy(filename_paper_texture,filename);

 /*
 Get beta (paper texture)
 */

 fscanf(fp,"%g",&beta_paper_texture);

 fprintf(stdout,"beta (paper texture) = %g\n",beta_paper_texture);

 /*
 Get beta (turbulent flow)
 */

 fscanf(fp,"%g",&beta_turbulent_flow);

 fprintf(stdout,"beta (turbulent flow) = %g\n",beta_turbulent_flow);

 /*
 Get gradient convolution kernel size (edge darkening)
 */

 fscanf(fp,"%d",&n_edge_darkening);

 fprintf(stdout,"gradient convolution kernel size (edge darkening) = %d\n",n_edge_darkening);

 /*
 Get beta (edge darkening)
 */

 fscanf(fp,"%g",&beta_edge_darkening);

 fprintf(stdout,"beta (edge_darkening) = %g\n",beta_edge_darkening);

 /*
 Get filename for the watercolor image
 */

 fscanf(fp,"%s",filename);

 fprintf(stdout,"output image = %s\n",filename);

 strcpy(filename_output_image,filename);

 /*
 We are done reading the input file
 */

 fclose(fp);

 /*
 Check that the gradient convolution kernel size (edge darkening) is odd
 */

 if ( !(n_edge_darkening%2 == 1) ) {
    fprintf(stdout,"gradient convolution kernel size (edge darkening) must be odd!\n");
    return 1;
 }

 thewatercolorist_main(
  filename_image,
  filename_paper_texture, 
  beta_paper_texture,
  beta_turbulent_flow,
  n_edge_darkening,
  beta_edge_darkening,
  filename_output_image
 );

 return 0;

}
