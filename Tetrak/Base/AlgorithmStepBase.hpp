#ifndef TETRAK_ALGORITHMSTEPBASE_HPP
#define TETRAK_ALGORITHMSTEPBASE_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {

class AlgorithmStepBase {
protected:
	TetrakData<BUFFERSIZE, MELBANDS>* data_ = nullptr;
	TetrakParameters* params_ = nullptr;
	
public:
	virtual ~AlgorithmStepBase() {};
	virtual void init(TetrakData<BUFFERSIZE, MELBANDS>* data, TetrakParameters* params) {
		data_ = data;
		params_ = params;
	}
	virtual void reset() {}
	
	// Used to convert any arguments to the desired type if so needed
	virtual void processSetupArgs(std::vector<TetrakArg>& args) {}
	
	virtual void setup(std::vector<TetrakArg>& args) {
		throw TetrakException(TetrakErrorCode::NotImplemented);
	}
	
	virtual void execute() = 0;
};

}

#endif /* TETRAK_ALGORITHMSTEPBASE_HPP */
 
