#ifndef CMDSTAN_CALLBACKS_WORDLE_INTERRUPT_HPP
#define CMDSTAN_CALLBACKS_WORDLE_INTERRUPT_HPP

#include <stan/callbacks/interrupt.hpp>
#include <cstdlib>
#include <cmath>
#include <random>
#include <ctime>

namespace cmdstan {
namespace callbacks {

  class wordle_interrupt : public stan::callbacks::interrupt {
  public:
    wordle_interrupt(const int N_warmup, const int N_draws)
      : N_warmup_(N_warmup), N_draws_(N_draws), N_total_(N_warmup + N_draws),
	iteration_(0),
	start_ms_(static_cast<long int>(std::time(nullptr))),
	g_(start_ms_), flip_(0.2),
	wordle_num_(std::uniform_int_distribution<int>(1,10000)(g_)),
	N_(-1) {
      stops_ = create_stops(N_warmup_, N_draws_, 6);
      for (int n = 0; n < 6; n++)
	wordle_colors_[n] = &gry_[0];
    }

    void operator()() {
      if (iteration_ == 0) {
	std::cout << "CmdStan Wordle " << wordle_num_ << " " << std::flush;
      }
      iteration_++;

      if (N_ == -1 && iteration_ == stops_[0]) {
	long int current_ms = static_cast<long int>(std::time(nullptr));
	if (current_ms - start_ms_ < 1000) {
	  int p = std::uniform_int_distribution<int>(1,60)(g_);
	  if (p < 5)
	    N_ = 1;
	  else if (p < 15)
	    N_ = 2;
	  else if (p < 25)
	    N_ = 3;
	  else if (p < 35)
	    N_ = 4;
	  else if (p < 45)
	    N_ = 5;
	  else
	    N_ = 6;
	} else {
	  N_ = 6;
	}
	std::cout << N_ << "/6" << std::endl << std::endl;

	stops_ = create_stops(N_warmup_, N_draws_, N_);
      }

      int* stop_location = std::find(&stops_[0], &stops_[0] + 5, iteration_);
      if (stop_location != (&stops_[0] + 5)) {
	int stop_number = stop_location - &stops_[0] + 1;
	//std::cout << "stopping at iteration_ = " << iteration_ << std::endl;
	create_row_colors(stop_number, N_, wordle_colors_);
	for (int n = 0; n < 6; n++) {
	  std::cout << wordle_colors_[n];
	}
	std::cout << std::endl;
      }

      if (iteration_ == N_total_) {
	for (int n = 0; n < 6; n++) {
	  std::cout << grn_;
	}
	std::cout << std::endl;
      }
    }

  private:
    const char grn_[5] { '\xF0', '\x9F', '\x9F', '\xA9', 0};
    const char yel_[5] { '\xF0', '\x9F', '\x9F', '\xA8', 0};
    const char gry_[4] { '\xE2', '\xAC', '\x9C', 0};

    const int N_warmup_;
    const int N_draws_;
    const int N_total_;
    int iteration_;
    const char *wordle_colors_[6];

    const long int start_ms_;
    std::default_random_engine g_;
    std::bernoulli_distribution flip_;
    const int wordle_num_;

    int N_;
    std::vector<int> stops_;

    std::vector<int> create_stops(int N_warmup, int N_draws, int N_stops) {
      int N_total = N_warmup + N_draws;
      std::vector<int> stops{ -1, -1, -1, -1, -1 };

      for (int n = 0; n < (N_stops - 1); n++) {
	stops[n] = ceil((n + 1.0) * N_total / N_stops);
      }
      return stops;
    }

    void create_row_colors(const int stop_number, const int N, const char *wordle_colors[6]) {
      // init to all gray
      for (int n = 0; n < 6; n++)
	wordle_colors[n] = &gry_[0];

      for (int n = 0; n < 6; ++n) {
	// color green or yellow
	if (n < 6 * stop_number / N) {
	  if (flip_(g_)) {
	    wordle_colors[n] = &yel_[0];
	  } else {
	    wordle_colors[n] = &grn_[0];
	  }
	} else {
	  if (flip_(g_)) {
	    wordle_colors[n] = &yel_[0];
	  } else {
	    wordle_colors[n] = &gry_[0];
	  }
	}
      }
      
      std::shuffle(&wordle_colors[0], &wordle_colors[5], g_);
    }
  };

}  // namespace callbacks
}  // namespace cmdstan
#endif
