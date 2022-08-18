FImage *GaussianBlur(FImage *f, float sigma);

FImage *convolveVertical1D(FImage *f, float *kernel, int k_len);

FImage *convolveHorizontal1D(FImage *f, float *kerneli, int k_len);
