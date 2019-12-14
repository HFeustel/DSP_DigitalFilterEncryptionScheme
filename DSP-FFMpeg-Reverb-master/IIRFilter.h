#ifndef _IIR_FILTER_H_
#define _IIR_FILTER_H_

#include <memory>
#include <deque>
#include <vector>

using std::size_t;
using std::int32_t;

using std::array;


class IIRFilter {
using outType = int32_t;
using deque = std::deque<outType>;
using buffPtr = std::unique_ptr<deque>;

public:
  IIRFilter(std::vector<float> taps_);
  ~IIRFilter() = default;

  outType do_filtering(outType new_x);

private:

  size_t delay;
  std::vector<float> taps;
  buffPtr input_samples;
};

#endif // _REVERB_UNIT_H_
