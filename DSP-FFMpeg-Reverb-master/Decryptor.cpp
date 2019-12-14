/* Perform filtering on audio samples 
 * - reads input from stdin and writes filtered samples to stdout
 * - two channels (front left, front right)
 * - samples in interleaved format (L R L R ...)
 * - samples are 16-bit signed integers (what the Rpi needs)
 *
 * Usage:
 *   ./ffmpeg_decode cool_song.mp3 | ./filter | ./ffmpeg_play 
 */

#include "Filter_Project.h"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <memory>

using std::string;
using std::array;
using std::uint8_t;
using std::int16_t;

// Constructor implementation
FilterProject::FilterProject(float feedbackGain_) : 
 allpass(1,1),
 allpass(1,1),
 allpass(1,1),
 allpass(1,1),
 //Pass in filter coefficients without leading 1 here:
 iirFilter({}),
  // pass in FIR coefficients to the FIR filter class
  firFilter({1})
{
  delay = std::make_unique<deque>(3*2*2940, 0.0);

  feedbackGain = 0.25;
  if (feedbackGain_ > 0 && feedbackGain_ < 1) {
    feedbackGain = feedbackGain_;
  }
}

// function to run on the samples from stdin
uint8_t* FilterProject::get_samples(uint8_t* samples, size_t num_samples) {

  //convert the uint8_t samples to floating point
  auto samples_cast = reinterpret_cast<int16_t*>(samples);
  auto num_samples_cast = num_samples/sizeof(int16_t);

  for (auto i = 0; i < num_samples_cast; i++) {
    //filters on!
    auto left_sample = samples_cast[i];
    samples_cast[i] = do_filtering(left_sample);
    ++i;
    auto right_sample = samples_cast[i];
    samples_cast[i] = do_filtering(right_sample);
  }

  return reinterpret_cast<uint8_t*>(samples);  
}

FilterProject::outType FilterProject::do_filtering(outType new_x) {
  auto &d = *delay.get();

  // the coefficient on the d.back() sets how long the reverb 
  // will sustain: larger = longer 
  auto x = 0.7*new_x + feedbackGain*d.back();

  
  auto y5 = iirFilter.do_filtering(new_x)
  d.pop_back();
  d.push_front(y5);

  //add a bit of an FIR filter here, smooth the output

  return y;
}


// ---------------------------------------------main --------------------------------------------------
int main(int argc, char** argv) {
  if (argc != 1) {
      fprintf(stderr, "usage: ./ffmpeg_decode <input file> | %s | ./ffmpeg_play\n", argv[0]);
      exit(1);
  }

  //some constants
  const int BUFF_SIZE = 4096;
  array<uint8_t, BUFF_SIZE> buffer;
  FilterProject reverb(0.25);

  for (;;) {

    // read input buffer from stdin
    ssize_t ret = read(STDIN_FILENO, buffer.data(), buffer.size());
    if (ret < 0) {
        fprintf(stderr, "read(stdin)\n");
        exit(1);
    }

    //exit if out of data
    if (ret == 0) {
        break;
    }

    // do the filtering
    reverb.get_samples(buffer.data(), buffer.size());

    // write output buffer to stdout
    if (write(STDOUT_FILENO, buffer.data(), buffer.size()) != buffer.size()) {
      fprintf(stderr, "error: write(stdout)\n");
      exit(1);
    }
  }

  return 0;
}
