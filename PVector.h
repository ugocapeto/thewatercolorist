#ifndef __PVECTOR_H__
#define __PVECTOR_H__
class PVector
{

  public: float x;

  public: float y;

  public: float z;

  PVector();

  PVector(float x, float y, float z);

  float dot(PVector other); 

  void mult(float multiplier); 

};
#endif
