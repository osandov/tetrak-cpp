#ifndef TETRAK_TEMPOTRACKER_HPP 
#define TETRAK_TEMPOTRACKER_HPP 

#include "Tetrak/Base/TempoTrackerBase.hpp"

#include "Tetrak/Implementation/Processor.hpp"
#include "Tetrak/Implementation/EventExtractor.hpp"
#include "Tetrak/Implementation/TempoFinder.hpp"

namespace Tetrak {

class TempoTracker : public TempoTrackerBase {
public:
	TempoTracker(InputterBase* inputter) :
		TempoTrackerBase(inputter, new Processor, new EventExtractor, new TempoFinder) {}
	
	virtual void executeCycle()
	{
		executeInput();
		executeProcess();
		executeEventExtract();
		executeTempoFind();
		executeOutput();
	}
};

}

#endif /* TETRAK_IMPLEMENTEDTEMPOTRACKER_HPP */
