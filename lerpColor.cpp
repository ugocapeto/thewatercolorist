#include "header.h"

void lerpColor(
 int rgb1[3],
 int rgb2[3],
 float amt,
 int rgb3[3]
)

{

 if ( amt < 0.f )
  amt= 0.f;
 if ( amt > 1.f )
  amt= 1.f;

 for ( int cind= 0 ; cind< 3 ; cind++ ) {
    float val_float = (1.f-amt)*(float)rgb1[cind]+
                           amt *(float)rgb2[cind];
    int val_int= (int)roundf(val_float);
    if ( !(val_int >= 0) )
     val_int= 0;
    if ( !(val_int <= 255) )
     val_int= 255;
    rgb3[cind]= val_int;
 }

}
