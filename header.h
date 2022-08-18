#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define MAXLINE 256
#include <limits.h>

#include <iostream>
#include <chrono>
#include <random>

#include "io/proto.h"
#include "math/proto.h"
#include "util/proto.h"

#include "PVector.h"
#include "PImage.h"
#include "FImage.h"
#include "Color.h"
#include "Filtering.h"
#include "image_abstraction_filter.h"
#include "Tensor.h"

#include "FastNoise-master/FastNoise.h"
#include "RGBConverter-master/RGBConverter.h"

#include "defs.h"
#include "proto.h"
