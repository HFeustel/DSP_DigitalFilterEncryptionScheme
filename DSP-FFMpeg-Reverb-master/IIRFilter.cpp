#include "IIRFilter.h"

IIRFilter::IIRFilter(std::vector<float> taps_) :
  taps(taps_)
{
  // multiply by 2 to make room for the L/R audio channels
  delay = taps_.size();

  // initilize buffer to zeros
  input_samples = std::make_unique<deque>(delay, 0.0);
}

FIRFilter::outType FIRFilter::do_filtering(outType new_x) {
  auto &y = *input_samples.get();

  // replace the oldest value for x
  
  double new_y = 0;
  // implement the filter. 
  for (auto i = 0; i < delay; ++i) {
    //multiply by 2 to account for LR channels
    new_y -= taps[i]*y[i];
  }
  y.pop_back()
  y.push_front(new_y);
  //return the newest value for y
  return new_y;
}
