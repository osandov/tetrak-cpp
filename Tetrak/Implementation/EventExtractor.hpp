#ifndef TETRAK_EVENTEXTRACTOR_HPP
#define TETRAK_EVENTEXTRACTOR_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {

class EventMemoryDecay : public tetra::MathFunction<OnsetEvent, Sample> {
	Sample decay_, min_, start_;
public:
	EventMemoryDecay(Sample decay, Sample min, Sample start) :
		decay_(decay), min_(min), start_(start) {}
	
	Sample f(const OnsetEvent& x) {
		return x.energy * ((1.0 - min_) * exp(decay_ * (x.pos - start_)) + min_);
		//~ return x.energy * exp(decay_ * (x.pos - start_));
	}
};

class EventExtractor : public EventExtractorBase {
	Sample shiftDiff_ = 0, prevShiftDiff_ = 0;
	Sample shiftTroughEnergy_ = 0, shiftTroughPos_ = 0;
	
	Sample eventThreshold();
	
public:
	void execute();
	
	void reset();
};

}

#endif /* TETRAK_EVENTEXTRACTOR_HPP */
