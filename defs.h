// declare external variables
// they are defined in main.cpp

// params
// tensor
extern float tensor_sigma;
// orientation aligned bilateral filter
extern int n_e;
extern int n_a;
extern float sigma_d;
extern float sigma_r;
// flow based differences of Gaussian
extern int fdog_n;
extern float fdog_sigma_e;
extern float fdog_sigma_r;
extern float fdog_tau;
extern float fdog_sigma_m;
extern float fdog_phi;
// quantization
extern float phi_q;
extern int quant_levels;

// variables
extern PImage *input;
extern FImage *tfm;
extern FImage *sourceRGB;
extern FImage *sourceLab;
