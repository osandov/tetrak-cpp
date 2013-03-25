#ifndef TETRAK_FILEINPUTTER_HPP
#define TETRAK_FILEINPUTTER_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"
#include <sndfile.h>

namespace Tetrak {

class FileInputter : public InputterBase {
protected:
	const char* fileName_ = nullptr;
	SNDFILE* file_ = nullptr;
	SF_INFO fileInfo_;
	void readData();
	
public:
	void reset();
	void processSetupArgs(std::vector<TetrakArg>& args);
	void setup(std::vector<TetrakArg>& args);
	void execute();
	
	~FileInputter();
};

}

#endif /* TETRAK_FILEINPUTTER_HPP */
