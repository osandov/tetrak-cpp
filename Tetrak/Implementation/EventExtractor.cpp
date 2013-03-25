#include "Tetrak/Implementation/EventExtractor.hpp" 

namespace Tetrak {

Sample EventExtractor::eventThreshold() {
	EventMemoryDecay f(params_->eventMemoryDecay, params_->minEventMemory, data_->currentPos_Seconds);
	return f.sum(data_->events.begin(), data_->events.end()) /
		params_->eventCount * params_->eventThreshold;
}

void EventExtractor::execute() {
	prevShiftDiff_ = shiftDiff_;
	shiftDiff_ = data_->shift - data_->prevShift;
	data_->eventFound = false;
	
	if (shiftDiff_ > 0 && prevShiftDiff_ <= 0) {
		shiftTroughPos_ = data_->currentPos_Seconds;
		shiftTroughEnergy_ = data_->prevShift;
	} else if (shiftDiff_ <= 0 && prevShiftDiff_ > 0) {
		if (data_->prevShift - shiftTroughEnergy_ > eventThreshold()) {
			if (data_->events.size() == params_->eventCount)
				data_->events.pop_front();
			data_->events.push_back(OnsetEvent{shiftTroughPos_, data_->prevShift - shiftTroughEnergy_});
			data_->eventFound = true;
		}
	}
}

void EventExtractor::reset() {
	shiftDiff_ = 0;
	prevShiftDiff_ = 0;
	shiftTroughEnergy_ = 0;
	shiftTroughPos_ = 0;
}

}
