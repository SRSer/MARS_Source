#include "cuveconv.h"

#include "math.h"
#include "fastmath.h"

tCuveConv::tCuveConv (const float linstart, const float linend, const float expstart, const float expend)
{
  _b = log(expend/expstart) / (linend-linstart);
  _a = expend / fastexp( _b * linend );
}

float tCuveConv::linToExp (float input)
{
  if (input)
    return _a * fastexp( _b * input );
  else
    return 0;
}