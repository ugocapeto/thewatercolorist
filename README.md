# thewatercolorist

Written and tested on Ubuntu 22.04.

Bousseau et al. pioneered the process of watercolorizing a photograph in "Interactive watercolor rendering with temporal coherence and abstraction" by Adrien Bousseau, Matthew Kaplan, Joelle Tholot, Francois X. Sillion. The idea of darkening/lightening a color image given a grayscale texture image enables to simulate a bunch of watercolor effects including paper texture, turbulent flow, pigment dispersion, and edge darkening. There is another paper that I found quite useful in implementing my own version of Bousseau's watercolorizer, "Expressive Rendering with Watercolor" by Patrick J. Doran and John Hughes, as it discusses Bousseau's algorithm.

To create the executable:

compile the code in directory "thewatercolorist/FastNoise-master" using "make -f Makefile_g/Makefile_O"

compile the code in directory "thewatercolorist/RGBConverter-master" using "make -f Makefile_g/Makefile_O"

compile the code in directory "thewatercolorist" using "make -f Makefile_g/Makefile_O"

compile the code in directory "thewatercolorist/main" using "make"

Test cases are given in the "test" directory.

What you would typically do is run thecartoonist (see my other repos) on a given picture to get an abstracted quantized version of the picture (file name should be "abstracted_image_after_quantize.png") and use that as the input for thewatercolorist.

Info about thewatercolorist (theory behind it and how to use it) can be found here:

[Non Photorealistic Rendering Software - The Watercolorist](https://3dstereophoto.blogspot.com/2018/07/non-photorealistic-rendering-software_41.html)

[Non Photorealistic Rendering - Watercolor rendering (watercolorization)](https://3dstereophoto.blogspot.com/2018/05/non-photorealistic-rendering-watercolor.html)

Dependencies (check the Makefiles):

"common" repo

# Notes

FastNoise-master comes from github user Auburns. MIT license.

RGBConverter-master comes from github user ratkins. "Do whatever you like with it" license.
