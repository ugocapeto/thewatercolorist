# thecartoonist

Written and tested on Ubuntu 22.04.

thecartoonist is software to abstract and stylize an input photograph in such a way that the output photograph kinda looks like a cartoon with thick lines and cel shading (toon shading). This is basically a cartoon filter.

thecartoonist is an implementation of:

"Image Abstraction by Structure Adaptive Filtering" by Jan Eric Kyprianidis and Jürgen Döllner.

"Real-Time Video Abstraction" by Holger Winnemöller, Sven C. Olsen, and Bruce Gooch.

To create the executable:

compile the code in directory "thecartoonist/FastNoise-master" using "make -f Makefile_g/Makefile_O"

compile the code in directory "thecartoonist/RGBConverter-master" using "make -f Makefile_g/Makefile_O"

compile the code in directory "thecartoonist" using "make -f Makefile_g/Makefile_O"

compile the code in directory "thecartoonist/main" using "make"

Test cases are given in the "test" directory.

Info about thecartoonist (theory behind it and how to use it) can be found here:

[Non Photorealistic Rendering Software - The Cartoonist](https://3dstereophoto.blogspot.com/2018/07/non-photorealistic-rendering-software_9.html)

[Non Photorealistic Rendering - Image Abstraction by Structure Adaptive Filtering](https://3dstereophoto.blogspot.com/2018/05/non-photorealistic-rendering-image.html)

Dependencies (check the Makefiles):

"common" repo

# Disclaimer

The code comes from a "Processing" implementation by Thomas Lindemeier which I ported to C++ and to which I added support for basic image formats: jpeg, png, and tiff.

FastNoise-master comes from github user Auburns. MIT license.

RGBConverter-master comes from github user ratkins. "Do whatever you like with it" license.
