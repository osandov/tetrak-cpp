#include "Tetrak/Implementation/Processor.hpp" 

namespace Tetrak {

void Processor::execute() {
	data_->executeMel();
	
	auto spectBegin = data_->melSpect.begin(),
		 spectEnd = data_->melSpect.end(),
		 prevBegin = data_->prevMelSpect.begin();
	
	Sample shift = 0.0;
	while (spectBegin != spectEnd) {
		Sample delta = *spectBegin++ - *prevBegin++;
		if (delta > 0.0)
			shift += delta;
	}
	
	data_->setShift(data_->avgSmooth(data_->avgSmooth(shift)));
	
	data_->prevMelSpect = data_->melSpect;
}

}
