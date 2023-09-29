#ifndef CUVECONV_H_
#define CUVECONV_H_

#include "stdint.h"

class tCuveConv
{
  public:
    tCuveConv(const float linstart, const float linend, const float expstart, const float expend); //x1, y1 - точки входящей линейной прямой; x2, y2 - точки преобразованной экспоненциальной
    //~tCuveConv();
    float linToExp (float input);

  private:
    float _a;
    float _b;
};

#endif /* CUVECONV_H_ */