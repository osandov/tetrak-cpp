#ifndef TETRAK_OUTPUTTERBASE_HPP
#define TETRAK_OUTPUTTERBASE_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {

class OutputterBase : public AlgorithmStepBase {
protected:
	bool everyCycle_ = false;

public:
	bool everyCycle() {return everyCycle_;}
};

}

#endif /* TETRAK_OUTPUTTERBASE_HPP */
