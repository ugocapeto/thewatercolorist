// input image //<>// //<>//
final String imageFile = "data/Butterfly.jpg";
//final String imageFile = "data/test.png";
//final String imageFile = "data/test_512x512.png";
//final String imageFile = "data/miami.jpg";

// params
// tensor
final float tensor_sigma = 3.f;
// orientation aligned bilateral filter
final float sigma_d = 3.f;
final float sigma_r = 4.25f;
final int nb = 1;
// flow based differences of Gaussian
final int fdog_n = 2;
final float fdog_sigma_e = 1.f;
final float fdog_sigma_r = 1.6;
final float fdog_tau = 0.99;
final float fdog_sigma_m = 3.f;
final float fdog_phi = 2.f;
// quantization
final float phi_q = 3.4f;
final int nbins = 6;

// variables
PImage input;
FImage tfm;
FImage sourceLab;
PImage toShow;
String dispText = "showing input";
int iteration = 0;

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void settings()
{
  toShow = input = loadImage(imageFile);
  size(input.width, input.height);
}

void setup()
{
  FImage sourceRGB = new FImage(input);

  sourceLab = convert_srgb2Lab(sourceRGB);

  tfm = tangentFlow(sourceRGB, tensor_sigma); 

  noLoop();
}

void draw()
{  
  image(toShow, 0, 0, width, height);
  //drawArrows(tfm);

  fill(255, 200);
  rect(10, 10, 10+textWidth(dispText)+6, 10+textAscent()+2);
  fill(0, 200);
  text(dispText, 10+4, 10+textAscent()+4);
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void keyPressed()
{
  if (key == '1')
  {
    dispText = "input"; 
    toShow = input;
  } else if (key == '2')
  {
    dispText = "tangent flow";   
    toShow = lineIntegralConvolution(tfm, 10.f).toPImage();
  } else if (key == '3')
  {
    dispText = "orientation aligned bilateral filter iteration: " + iteration;  
    toShow = convert_Lab2srgb(sourceLab).toPImage();
    //toShow.save("data/oabf.jpg");
  } else if (key == '4')
  {
    dispText = "f dog";   
    toShow = fdog(sourceLab, tfm, fdog_n, fdog_sigma_e, fdog_sigma_r, fdog_tau, fdog_sigma_m, fdog_phi).toPImage();
    //toShow.save("data/fdog.jpg");
  } else if (key == '5')
  {
    dispText = "oabf, quantization and fdog combined";   
    FImage edges = fdog(sourceLab, tfm, fdog_n, fdog_sigma_e, fdog_sigma_r, fdog_tau, fdog_sigma_m, fdog_phi);
    FImage qu = quantize(sourceLab, nbins, phi_q);
    FImage rgb = convert_Lab2srgb(qu);

    toShow = blendImages(rgb, edges, new PVector(0, 0, 0)).toPImage();
    //toShow.save("data/result.jpg");
  } else if (key == ' ')
  {
    sourceLab = orientationAlignedBilateralFilter(sourceLab, tfm, sigma_d, sigma_r, nb);
    iteration++;
    dispText = "orientation aligned bilateral filter iteration: " + iteration;  
    //toShow = convert_Lab2srgb(sourceLab).toPImage();
  } 

  redraw();
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage tangentFlow(final FImage input, final float tensor_sigma)
{
  // compute structure tensors from rgb image
  FImage tensors = computeStructureTensors(input);

  // smooth tensors with Gaussian blur
  if (tensor_sigma > 0.5f)
  {
    tensors = GaussianBlur(tensors, tensor_sigma);
  }

  FImage tfm = computeTangentFlowMap(tensors);

  return tfm;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage orientationAlignedBilateralFilter(final FImage sourceLab, final FImage tfm, final float sigma_d, final float sigma_r, int n)
{
  final int w = sourceLab.width;
  final int h = sourceLab.height;

  FImage t0 = new FImage(w, h);  
  FImage t1 = new FImage(sourceLab);  

  for (int i = 0; i < n; i++)
  {
    run_oabf(0, t1, t0, tfm, sigma_d, sigma_r);
    run_oabf(1, t0, t1, tfm, sigma_d, sigma_r);
  }  

  return t1;
}

void run_oabf(int pass, final FImage sourceLab, FImage target, final FImage tfm, final float sigma_d, final float sigma_r)
{
  final int w = sourceLab.width;
  final int h = sourceLab.height;

  for (int y = 0; y < h; y++) 
  {
    for (int x = 0; x < w; x++) 
    {        
      final PVector uv = new PVector(x, y, 0.f);
      final PVector tangent = tfm.get(uv.x, uv.y);
      PVector t = (pass == 0) ? new PVector(tangent.y, -tangent.x, 0.f) : tangent;    

      if (abs(t.x) >= abs(t.y))
      {
        t.y = t.y / t.x;
        t.x = 1.f;     
        t.z = 0.;
      } else
      {
        t.x = t.x / t.y;
        t.y = 1.f;
        t.z = 0;
      }      

      final PVector center = sourceLab.get(uv.x, uv.y);
      PVector sum = new PVector();
      sum.x = center.x;
      sum.y = center.y;
      sum.z = center.z;

      float norm = 1;
      float halfWidth = (2.0 * sigma_d) / sqrt(t.x*t.x+t.y*t.y);

      //println("t: " + t + "  h: " + halfWidth);

      for (int d = 1; d <= halfWidth; d++) 
      {
        final float uxn = uv.x + d * t.x;
        final float uyn = uv.y + d * t.y;
        final float uxp = uv.x - d * t.x;
        final float uyp = uv.y - d * t.y;

        PVector c0 = sourceLab.get(uxn, uyn);
        PVector c1 = sourceLab.get(uxp, uyp);

        float e0 = sqrt(pow(c0.x-center.x, 2.f) + pow(c0.y-center.y, 2.f) + pow(c0.z-center.z, 2.f));
        float e1 = sqrt(pow(c1.x-center.x, 2.f) + pow(c1.y-center.y, 2.f) + pow(c1.z-center.z, 2.f));

        float kerneld = exp(-(d*d) / (2.f*sigma_d*sigma_d));
        float kernele0 = exp(-(e0*e0) / (2.f*sigma_r*sigma_r));
        float kernele1 = exp(-(e1*e1) / (2.f*sigma_r*sigma_r));      

        norm += kerneld * kernele0;
        norm += kerneld * kernele1;

        sum.x += kerneld * kernele0 * c0.x;
        sum.y += kerneld * kernele0 * c0.y;
        sum.z += kerneld * kernele0 * c0.z;

        sum.x += kerneld * kernele1 * c1.x;
        sum.y += kerneld * kernele1 * c1.y;
        sum.z += kerneld * kernele1 * c1.z;
      }
      sum.x /= norm;
      sum.y /= norm;
      sum.z /= norm;
      target.set(x, y, sum);
    }
  }
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage fdog(FImage src, FImage tfm, 
  final int fdog_n, final float fdog_sigma_e, final float fdog_sigma_r, final float fdog_tau, final float fdog_sigma_m, final float fdog_phi)
{
  final int w = sourceLab.width;
  final int h = sourceLab.height;

  FImage tmp = new FImage(w, h);  
  FImage dst = new FImage(w, h);  

  for (int i = 0; i < fdog_n; i++)
  {
    FImage img = (i == 0)? src : overlay(dst, src);
    //img.toPImage().save("data/"+i+"_fdog_img.jpg");
    run_fdog_0(img, tmp, tfm, fdog_sigma_e, fdog_sigma_r, fdog_tau);
    //tmp.toPImage().save("data/"+i+"_fdog_dstp1.jpg");
    run_fdog_1(tmp, dst, tfm, fdog_sigma_m, fdog_phi);
    //dst.toPImage().save("data/"+i+"_fdog_dstp2.jpg");
  } 

  return GaussianBlur(dst, 0.7f);
}

void run_fdog_0(final FImage img, FImage dst, FImage tfm, 
  final float sigma_e, final float sigma_r, final float tau)
{
  final float twoSigmaESquared = 2.0 * sigma_e * sigma_e;
  final float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;

  final int w = img.width;
  final int h = img.height;

  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      final PVector uv = new PVector(x, y, 0.f);
      final PVector t = tfm.get(constrain((int)round(uv.x), 0, w-1), constrain((int)round(uv.y), 0, h-1)); // nearest neighbor
      PVector n = new PVector(t.y, -t.x, 0.); // along gradient not tangent
      if (abs(n.x) >= abs(n.y))
      {
        n.y = n.y / n.x;
        n.x = 1.f;     
        n.z = 0.;
      } else
      {
        n.x = n.x / n.y;
        n.y = 1.f;
        n.z = 0;
      }    
      final PVector ht = img.get(uv.x, uv.y);
      PVector sum = new PVector(ht.x, ht.x, 0.f);
      PVector norm = new PVector(1.0, 1.0, 0.f);

      float halfWidth = 2.0 * sigma_r / sqrt(n.x*n.x+n.y*n.y);
      for (int d = 1; d <= halfWidth; d++) 
      {
        PVector kernel = new PVector( exp( -d * d / twoSigmaESquared), 
          exp( -d * d / twoSigmaRSquared ), 0.f);
        norm.x += 2.0 * kernel.x;
        norm.y += 2.0 * kernel.y;

        PVector L0 =  img.get(uv.x - d*n.x, uv.y - d*n.y);
        PVector L1 =  img.get(uv.x + d*n.x, uv.y + d*n.y);  
        L0.y = L0.x;
        L1.y = L1.x;

        sum.x += kernel.x * ( L0.x + L1.x);
        sum.y += kernel.y * ( L0.y + L1.y);
      }
      sum.x /= norm.x;
      sum.y /= norm.y;

      float diff = 100.0 * (sum.x - tau * sum.y);
      dst.set(x, y, diff, diff, diff);
    }
  }
}

void run_fdog_1(final FImage img, FImage dst, FImage tfm, 
  final float sigma_m, final float phi)
{
  final float twoSigmaMSquared = 2.0 * sigma_m * sigma_m;
  final float halfWidth = 2.0 * sigma_m;
  final int w = img.width;
  final int h = img.height;

  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      final PVector uv = new PVector(x, y, 0.f);
      float wg = 1.0;
      float H = img.get(x, y).x;

      lic_t a = new lic_t(), b = new lic_t();
      a.p.x = b.p.x = uv.x;
      a.p.y = b.p.y = uv.y;
      a.t = tfm.get(x, y);
      b.t = tfm.get(x, y);
      b.t.mult(-1);
      a.w = b.w = 0.0;

      while (a.w < halfWidth) 
      {
        step(tfm, a);
        float k = a.dw * exp(-a.w * a.w / twoSigmaMSquared); // TODO check a.dw
        H += k * img.get(a.p.x, a.p.y).x;
        wg += k;
      }
      while (b.w < halfWidth) 
      {
        step(tfm, b);
        float k = b.dw * exp(-b.w * b.w / twoSigmaMSquared); // TODO check b.dw
        H += k * img.get(b.p.x, b.p.y).x;
        wg += k;
      }
      H /= wg;

      float edge = ( H > 0.0 )? 1.0 : 2.0 * smoothstep(-2.0, 2.0, phi * H );
      dst.set(x, y, edge, edge, edge);
    }
  }
}

FImage overlay(final FImage edges, final FImage img)
{
  final int w = edges.width;
  final int h = edges.height;

  FImage t0 = new FImage(w, h);  
  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      PVector c = img.get(x, y);
      float e = edges.get(x, y).x;
      t0.set(x, y, e*c.x, e*c.y, e*c.z);
    }
  }

  return t0;
}

class lic_t 
{ 
  PVector p = new PVector(); 
  PVector t = new PVector();
  float w;
  float dw;
};

float sign(float x)
{
  return (x <= 0.f) ? -1.f : 1.f;
}

float smoothstep(final float edge0, final float edge1, final float x)
{
  float t = constrain((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
}

void step(final FImage tfm, lic_t s) 
{
  PVector t = tfm.get(s.p.x, s.p.y);
  if (t.dot(s.t) < 0.0) t.mult(-1);
  s.t.x = t.x;
  s.t.y = t.y;

  s.dw = (abs(t.x) >= abs(t.y))? 
    abs(((s.p.x - floor(s.p.x)) - 0.5 - sign(t.x)) / t.x) : 
    abs(((s.p.y - floor(s.p.y)) - 0.5 - sign(t.y)) / t.y);

  s.p.x += t.x * s.dw;
  s.p.y += t.y * s.dw;
  s.w += s.dw;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage blendImages(final FImage img, final FImage edges, final PVector ec)
{
  final int w = img.width;
  final int h = img.height;
  FImage out = new FImage(w, h); 
  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      PVector b = new PVector();
      PVector c = img.get(x, y);
      float e = edges.get(x, y).x;
      b.x = e * c.x + (1.f-e) * ec.x;
      b.y = e * c.y + (1.f-e) * ec.y;
      b.z = e * c.z + (1.f-e) * ec.z;
      out.set(x, y, b);
    }
  }
  return out;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage quantize(final FImage img, final int nbins, final float phi_q)
{
  final int w = img.width;
  final int h = img.height;
  FImage out = new FImage(w, h); 
  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {
      final PVector uv = new PVector(x, y, 0.f);
      PVector d = new PVector(1.f, 1.f, 0.f);
      PVector c = img.get(uv.x, uv.y);

      float qn = floor(c.x * (float)nbins + 0.5) / (float)nbins;
      float qs = smoothstep(-2.0, 2.0, phi_q * (c.x - qn) * 100.0) - 0.5;
      float qc = qn + qs / (float)nbins;
      out.set(x, y, qc, c.y, c.z);
    }
  }
  return out;
}



/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

FImage lineIntegralConvolution(final FImage tfm, final float sigma)
{
  final int w = tfm.width;
  final int h = tfm.height;
  FImage out = new FImage(w, h); 

  final float step = 1.;

  final int l = (int)(2.0f * floor( sqrt(-log(0.1f) * 2 * (sigma*sigma)) ) + 1.0f);

  PImage noise = createImage(w, h, RGB);
  for (int i = 0; i < noise.pixels.length; i++) 
  {
    noise.pixels[i] = color(random(0, 255));
  }
  noise.filter(THRESHOLD, 0.5);

  for (int y = 0; y < h; y++) 
  {    
    for (int x = 0; x < w; x++) 
    {        
      float c = 0;

      PVector v0 = tfm.get(x, y);

      float x_ = x;
      float y_ = y;

      float g = 0;

      // forward 
      for (int i = 0; i < l/2; i++)
      {       
        PVector v1 = tfm.get(x_, y_);

        if (v1.dot(v0) < 0.f) v1.mult(-1);

        x_ += v1.x*step;
        y_ += v1.y*step; 

        if (x_ < 0.f || x_ >= w || y_ < 0.f || y_ >= h) break;        

        v0 = v1;

        float gw = exp(-(i*i) / (2.f*sigma*sigma));
        c += gw * red(noise.get((int)x_, (int)y_));
        g+=gw;
      }
      v0 = tfm.get(x, y).mult(-1);
      x_ = x;
      y_ = y;
      // backward 
      for (int i = 0; i < l/2; i++)
      {       
        PVector v1 = tfm.get(x_, y_).mult(-1);

        if (v1.dot(v0) < 0.f) v1.mult(-1);

        x_ += v1.x*step;
        y_ += v1.y*step;

        if (x_ < 0.f || x_ >= w || y_ < 0.f || y_ >= h) break;

        v0 = v1;

        float gw = exp(-(i*i) / (2.f*sigma*sigma));
        c += gw * red(noise.get((int)x_, (int)y_));
        g+=gw;
      }
      final float lambda = tfm.get(x, y).z;
      color blend = lerpColor(color(c / g), color(255, 0, 0), lambda);
      out.set(x, y, red(blend) / 255., green(blend) / 255, blue(blend) / 255);
    }
  }

  println("finished lic");
  return out;
}

void drawArrows(final FImage tfm)
{
  final int w = tfm.width;
  final int h = tfm.height;

  final int cs = 15;
  final int o = 5;

  stroke(0, 255, 0);
  for (int y = 0; y < h; y+=cs) 
  {    
    for (int x = 0; x < w; x+=cs) 
    {        
      PVector v = tfm.get(x, y);
      final int r = (cs-1)/2;
      final int cx = x + r;
      final int cy = y + r;

      line(cx - o*v.x, cy - o*v.y, cx + o*v.x, cy + o*v.y);
    }
  }
}