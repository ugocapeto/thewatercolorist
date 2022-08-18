#ifndef __IMAGE_ABSTRACTION_FILTER_H__
#define __IMAGE_ABSTRACTION_FILTER_H__
class lic_t
{
  public: float p[3];
  public: float t[3];
  public: float w;
  public: float dw;
};
#endif

FImage *tangentFlow(FImage *input, float tensor_sigma);

FImage *orientationAlignedBilateralFilter(FImage *sourceLab, FImage *tfm, float sigma_d, float sigma_r, int n);

void run_oabf(int pass, FImage *sourceLab, FImage *target, FImage *tfm, float sigma_d, float sigma_r);

FImage *fdog(FImage *src, FImage *tfm, 
  int fdog_n, float fdog_sigma_e, float fdog_tau, float fdog_sigma_m, float fdog_phi);

void run_fdog_0(FImage *img, FImage *dst, FImage *tfm, 
  float sigma_e, float tau);

void run_fdog_1(FImage *img, FImage *dst, FImage *tfm, 
  float sigma_m, float phi);

FImage *overlay(FImage *edges, FImage *img);

float sign(float x);

void step(FImage *tfm, lic_t *s);

FImage *blendImages(FImage *img, FImage *edges, float ec[3]);

FImage *quantize(FImage *img, int quant_levels, float phi_q);

FImage *lineIntegralConvolution(FImage *tfm, float sigma);

void drawArrows(FImage *tfm);

float constrain(float amt, float low, float high);

int constrain(int amt, int low, int high);
