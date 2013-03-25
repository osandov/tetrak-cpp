#ifndef TETRAK_INPUTTERBASE_HPP
#define TETRAK_INPUTTERBASE_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {

class InputterBase : public AlgorithmStepBase {
protected:
	Sample len_ = 0.0;
	
	virtual void readData() = 0;
};

}

#endif /* TETRAK_INPUTTERBASE_HPP */
