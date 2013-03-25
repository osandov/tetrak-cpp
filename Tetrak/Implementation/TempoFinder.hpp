#ifndef TEMPOFINDER_HPP
#define TEMPOFINDER_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {

class Resonator : public tetra::MathFunction<OnsetEvent, Sample> {
	Sample a_, k_, b_, penalty_;
	Sample modifiedRamp(Sample x) {
		return (x >= 0) ? x : -penalty_;
	}
	
public:
	Resonator(Sample w, Sample k, Sample penalty) :
		a_(4.0 / (w * w)), k_(k), penalty_(penalty) {}
	
	void setOffset(const Sample& b) {
		b_ = b;
	}
	
	Sample scoreFactor(Sample interval) {
		k_ / interval;
	}
	
	Sample f(const OnsetEvent& x) {
		Sample x_ = fabs(fmod(x.pos - b_, k_));
		
		if (x_ > k_ * 0.5)
			x_ -= k_;
		
		return x.energy * modifiedRamp(1 - a_ * x_ * x_);
	}
};

class TempoFinder : public TempoFinderBase {
public:
	void execute();
};

}

#endif /* TEMPOFINDER_HPP */
