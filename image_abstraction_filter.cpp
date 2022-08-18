#include "header.h"

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage *tangentFlow(FImage *input, float tensor_sigma)
{
  // compute structure tensors from rgb image
  FImage *tensors = computeStructureTensors(input);

  // smooth tensors with Gaussian blur
  if (tensor_sigma > 0.5f)
  {
    FImage *tensors2 = GaussianBlur(tensors, tensor_sigma);
    delete tensors;
    tensors = new FImage(tensors2);
  }

  FImage *tfm = computeTangentFlowMap(tensors);

  return tfm;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage *orientationAlignedBilateralFilter(FImage *sourceLab, FImage *tfm, float sigma_d, float sigma_r, int n)
{
  int w = sourceLab->width;
  int h = sourceLab->height;

  FImage *t0 = new FImage(w, h);  
  FImage *t1 = new FImage(sourceLab);  

  for (int i = 0; i < n; i++)
  {
    run_oabf(0, t1, t0, tfm, sigma_d, sigma_r);
    run_oabf(1, t0, t1, tfm, sigma_d, sigma_r);
  }  

  return t1;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void run_oabf(int pass, FImage *sourceLab, FImage *target, FImage *tfm, float sigma_d, float sigma_r)
{
  int w = sourceLab->width;
  int h = sourceLab->height;

  for (int y = 0; y < h; y++) 
  {
    for (int x = 0; x < w; x++) 
    {        
      float uv[3];
      uv[0] = x;
      uv[1] = y;
      uv[2] = 0.f;
      float tangent[3];
      tfm->get(uv[0], uv[1], tangent);
      float t[3];
      if (pass == 0) {
        t[0]= tangent[1];
        t[1]=-tangent[0];
        t[2]= 0.f; 
      }
      else {
        t[0]= tangent[0];
        t[1]= tangent[1];
        t[2]= tangent[2];
      }

      if (fabsf(t[0]) >= fabsf(t[1]))
      {
        t[1] = t[1] / t[0];
        t[0] = 1.f;     
        t[2] = 0.;
      } else
      {
        t[0] = t[0] / t[1];
        t[1] = 1.f;
        t[2] = 0;
      }      

      float center[3];
      sourceLab->get(uv[0], uv[1], center);
      float sum[3];
      sum[0] = center[0];
      sum[1] = center[1];
      sum[2] = center[2];

      float norm = 1;
      float halfWidth = (2.0 * sigma_d) / sqrtf(t[0]*t[0]+t[1]*t[1]);

      //println("t: " + t + "  h: " + halfWidth);

      for (int d = 1; d <= halfWidth; d++) 
      {
        float uxn = uv[0] + d * t[0];
        float uyn = uv[1] + d * t[1];
        float uxp = uv[0] - d * t[0];
        float uyp = uv[1] - d * t[1];

        float c0[3];
        sourceLab->get(uxn, uyn, c0);
        float c1[3];
        sourceLab->get(uxp, uyp, c1);

        float e0 = sqrtf(powf(c0[0]-center[0], 2.f) + powf(c0[1]-center[1], 2.f) + powf(c0[2]-center[2], 2.f));
        float e1 = sqrtf(powf(c1[0]-center[0], 2.f) + powf(c1[1]-center[1], 2.f) + powf(c1[2]-center[2], 2.f));

        float kerneld = expf(-(d*d) / (2.f*sigma_d*sigma_d));
        float kernele0 = expf(-(e0*e0) / (2.f*sigma_r*sigma_r));
        float kernele1 = expf(-(e1*e1) / (2.f*sigma_r*sigma_r));      

        norm += kerneld * kernele0;
        norm += kerneld * kernele1;

        sum[0] += kerneld * kernele0 * c0[0];
        sum[1] += kerneld * kernele0 * c0[1];
        sum[2] += kerneld * kernele0 * c0[2];

        sum[0] += kerneld * kernele1 * c1[0];
        sum[1] += kerneld * kernele1 * c1[1];
        sum[2] += kerneld * kernele1 * c1[2];
      }
      sum[0] /= norm;
      sum[1] /= norm;
      sum[2] /= norm;
      target->set(x, y, sum);
    }
  }
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage *fdog(FImage *src, FImage *tfm, 
  int fdog_n, float fdog_sigma_e, float fdog_tau, float fdog_sigma_m, float fdog_phi)
{
  int w = sourceLab->width;
  int h = sourceLab->height;

  FImage *tmp = new FImage(w, h);
  FImage *dst = new FImage(w, h);

  for (int i = 0; i < fdog_n; i++)
  {
    FImage *img = (i == 0)? src : overlay(dst, src);
    //img->toPImage().save("data/"+i+"_fdog_img->jpg");
    run_fdog_0(img, tmp, tfm, fdog_sigma_e, fdog_tau);
    //tmp->toPImage().save("data/"+i+"_fdog_dstp1.jpg");
    run_fdog_1(tmp, dst, tfm, fdog_sigma_m, fdog_phi);
    //dst->toPImage().save("data/"+i+"_fdog_dstp2.jpg");
  } 

  return GaussianBlur(dst, 0.7f);
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void run_fdog_0(FImage *img, FImage *dst, FImage *tfm, 
  float sigma_e, float tau)
{
  float sigma_r = 1.6f * sigma_e;
  float twoSigmaESquared = 2.0 * sigma_e * sigma_e;
  float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;

  int w = img->width;
  int h = img->height;

  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      float uv[3];
      uv[0] = x;
      uv[1] = y;
      uv[2] = 0.f;
      float t[3];
      tfm->get(constrain((int)roundf(uv[0]), 0, w-1), constrain((int)roundf(uv[1]), 0, h-1), t); // nearest neighbor
      float n[3];
      n[0] = t[1]; // along gradient not tangent
      n[1] =-t[0]; // along gradient not tangent
      n[2] =   0.; // along gradient not tangent
      if (fabsf(n[0]) >= fabsf(n[1]))
      {
        n[1] = n[1] / n[0];
        n[0] = 1.f;     
        n[2] = 0.;
      } else
      {
        n[0] = n[0] / n[1];
        n[1] = 1.f;
        n[2] = 0;
      }    
      float ht[3];
      img->get(uv[0], uv[1], ht);
      float sum[3];
      sum[0] = ht[0];
      sum[1] = ht[0];
      sum[2] = 0.f;
      float norm[3];
      norm[0] = 1.0;
      norm[1] = 1.0;
      norm[2] = 0.f;

      float halfWidth = 2.0 * sigma_r / sqrtf(n[0]*n[0]+n[1]*n[1]);
      for (int d = 1; d <= halfWidth; d++) 
      {
        float kernel[3];
        kernel[0] = expf( -d * d / twoSigmaESquared );
        kernel[1] = expf( -d * d / twoSigmaRSquared );
        kernel[2] = 0.f; 
        norm[0] += 2.0 * kernel[0];
        norm[1] += 2.0 * kernel[1];

        float L0[3];
        img->get(uv[0] - d*n[0], uv[1] - d*n[1], L0);
        float L1[3];  
        img->get(uv[0] + d*n[0], uv[1] + d*n[1], L1);  
        L0[1] = L0[0];
        L1[1] = L1[0];

        sum[0] += kernel[0] * ( L0[0] + L1[0]);
        sum[1] += kernel[1] * ( L0[1] + L1[1]);
      }
      sum[0] /= norm[0];
      sum[1] /= norm[1];

      float diff = 100.0 * (sum[0] - tau * sum[1]);
      dst->set(x, y, diff, diff, diff);
    }
  }
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void run_fdog_1(FImage *img, FImage *dst, FImage *tfm, 
  float sigma_m, float phi)
{
  float twoSigmaMSquared = 2.0 * sigma_m * sigma_m;
  float halfWidth = 2.0 * sigma_m;
  int w = img->width;
  int h = img->height;

  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      float uv[3];
      uv[0] = x;
      uv[1] = y;
      uv[2] = 0.f;
      float wg = 1.0;
      float H3[3];
      img->get(x, y, H3);
      float H = H3[0];

      lic_t *a = new lic_t(), *b = new lic_t();
      a->p[0] = b->p[0] = uv[0];
      a->p[1] = b->p[1] = uv[1];
      tfm->get(x, y, a->t);
      tfm->get(x, y, b->t);
      b->t[0] *= -1;
      b->t[1] *= -1;
      b->t[2] *= -1;
      a->w = b->w = 0.0;

      while (a->w < halfWidth) 
      {
        step(tfm, a);
        float k = a->dw * expf(-a->w * a->w / twoSigmaMSquared); // TODO check a->dw
        float vec3_float[3];
        img->get(a->p[0], a->p[1], vec3_float);
        H += k * vec3_float[0];
        wg += k;
      }
      while (b->w < halfWidth) 
      {
        step(tfm, b);
        float k = b->dw * expf(-b->w * b->w / twoSigmaMSquared); // TODO check b->dw
        float vec3_float[3];
        img->get(b->p[0], b->p[1], vec3_float);
        H += k * vec3_float[0];
        wg += k;
      }
      H /= wg;

      float edge;
      if ( H > 0.0 ) {
        edge = 1.0f;
      }
      else {
        edge = 1.0f + tanhf(phi * H);
      }

      dst->set(x, y, edge, edge, edge);
    }
  }
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage *overlay(FImage *edges, FImage *img)
{
  int w = edges->width;
  int h = edges->height;

  FImage *t0 = new FImage(w, h);  
  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      float c[3];
      img->get(x, y, c);
      float e3[3];
      edges->get(x, y, e3);
      float e = e3[0];
      t0->set(x, y, e*c[0], e*c[1], e*c[2]);
    }
  }

  return t0;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

float sign(float x)
{
  return (x <= 0.f) ? -1.f : 1.f;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void step(FImage *tfm, lic_t *s) 
{
  float t[3];
  tfm->get(s->p[0], s->p[1], t);
  float val_float;
  val_float = t[0]*s->t[0]+
              t[1]*s->t[1]+
              t[2]*s->t[2];
  if (val_float < 0.0) {
    t[0] *= -1;
    t[1] *= -1;
    t[2] *= -1;
  }
    
  s->t[0] = t[0];
  s->t[1] = t[1];

  s->dw = (fabsf(t[0]) >= fabsf(t[1]))? 
    fabsf(((s->p[0] - floorf(s->p[0])) - 0.5 - sign(t[0])) / t[0]) : 
    fabsf(((s->p[1] - floorf(s->p[1])) - 0.5 - sign(t[1])) / t[1]);

  s->p[0] += t[0] * s->dw;
  s->p[1] += t[1] * s->dw;
  s->w += s->dw;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage *blendImages(FImage *img, FImage *edges, float ec[3])
{
  int w = img->width;
  int h = img->height;
  FImage *out = new FImage(w, h); 
  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      float b[3];
      float c[3];
      img->get(x, y, c);
      float e3[3];
      edges->get(x, y, e3);
      float e = e3[0];
      b[0] = e * c[0] + (1.f-e) * ec[0];
      b[1] = e * c[1] + (1.f-e) * ec[1];
      b[2] = e * c[2] + (1.f-e) * ec[2];
      out->set(x, y, b);
    }
  }
  return out;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage *quantize(FImage *img, int quant_levels, float phi_q)
{
  int w = img->width;
  int h = img->height;
  FImage *out = new FImage(w, h); 
  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      float uv[3];
      uv[0] = x;
      uv[1] = y;
      uv[2] = 0.f;
      float c[3];
      img->get(uv[0], uv[1], c);

      float dq = 100.f / (float)(quant_levels-1);
      float qn = roundf( (1.f / dq)*c[0] ) * dq;
      float qc = qn + (dq/2.f)*tanhf( phi_q*(c[0]-qn) );
      out->set(x, y, qc, c[1], c[2]);
    }
  }
  return out;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage *lineIntegralConvolution(FImage *tfm, float sigma)
{
  int w = tfm->width;
  int h = tfm->height;
  FImage *out = new FImage(w, h); 

  float step = 1.;

  int l = (int)(2.0f * floorf( sqrtf(-logf(0.1f) * 2 * (sigma*sigma)) ) + 1.0f);

  PImage *noise = new PImage(w, h);
  for (int i = 0; i < w*h*3; i++) 
  {
    int rand_int;
    get_random_int(0, 255, &rand_int);
    noise->pixels[i] = rand_int;
  }
  noise->filter((char *)"THRESHOLD", 0.5);

  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {        
      float c = 0;

      float v0[3];
      tfm->get(x, y, v0);

      float x_ = x;
      float y_ = y;

      float g = 0;

      // forward 
      for (int i = 0; i < l/2; i++)
      {       
        float v1[3];
        tfm->get(x_, y_, v1);

        float val_float;
        val_float = v1[0]*v0[0]+
                    v1[1]*v0[1]+
                    v1[2]*v0[2];
        if (val_float < 0.f) {
          v1[0] *= -1;
          v1[1] *= -1;
          v1[2] *= -1;
        }

        x_ += v1[0]*step;
        y_ += v1[1]*step; 

        if (x_ < 0.f || x_ >= w || y_ < 0.f || y_ >= h) break;        

        v0[0] = v1[0];
        v0[1] = v1[1];
        v0[2] = v1[2];

        float gw = expf(-(i*i) / (2.f*sigma*sigma));
        int vec3_int[3];
        noise->get((int)x_, (int)y_, vec3_int);
        c += gw * vec3_int[0];
        g+=gw;
      }
      tfm->get(x, y, v0);
      v0[0] *= -1;
      v0[1] *= -1;
      v0[2] *= -1;
      x_ = x;
      y_ = y;
      // backward 
      for (int i = 0; i < l/2; i++)
      {       
        float v1[3];
        tfm->get(x_, y_, v1);
        v1[0] *= -1;
        v1[1] *= -1;
        v1[2] *= -1;

        float val_float;
        val_float = v1[0]*v0[0]+
                    v1[1]*v0[1]+
                    v1[2]*v0[2];
        if (val_float < 0.f) {
          v1[0] *= -1;
          v1[1] *= -1;
          v1[2] *= -1;
        }

        x_ += v1[0]*step;
        y_ += v1[1]*step;

        if (x_ < 0.f || x_ >= w || y_ < 0.f || y_ >= h) break;

        v0[0] = v1[0];
        v0[1] = v1[1];
        v0[2] = v1[2];

        float gw = expf(-(i*i) / (2.f*sigma*sigma));
        int vec3_int[3];
        noise->get((int)x_, (int)y_, vec3_int);
        c += gw * vec3_int[0];
        g+=gw;
      }
      out->set(x, y, (c / g) / 255.f, (c / g) / 255.f, (c / g) / 255.f);
    }
  }
  return out;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

float constrain(float amt, float low, float high)
{
  if ( amt < low )
   amt= low;
  if ( amt > high )
   amt= high;
  return amt;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

int constrain(int amt, int low, int high)
{
  if ( amt < low )
   amt= low;
  if ( amt > high )
   amt= high;
  return amt;
}
