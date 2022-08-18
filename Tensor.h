FImage *computeStructureTensors(FImage *img);

float getLambda1(FImage *tensors, int x, int y);

float getLambda2(FImage *tensors, int x, int y);

void getV1(FImage *tensors, int x, int y, float v[3]);

void getV2(FImage *tensors, int x, int y, float v[3]);

FImage *computeTangentFlowMap(FImage *tensors);
