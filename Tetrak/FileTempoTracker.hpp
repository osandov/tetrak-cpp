#ifndef TETRAK_FILETEMPOTRACKER_HPP
#define TETRAK_FILETEMPOTRACKER_HPP

#include "Tetrak/Implementation/TempoTracker.hpp"

#include "Tetrak/Implementation/Input/FileInputter.hpp"
#include "Tetrak/Implementation/Output/GnuplotOutputter.hpp"

namespace Tetrak {

class FileTempoTracker : public TempoTracker {
public:
	FileTempoTracker() : TempoTracker(new FileInputter) {}
};

class FileTempoTrackerProgram : public TempoTrackerProgramBase {
public:
	FileTempoTrackerProgram() :
		TempoTrackerProgramBase(new FileTempoTracker) {}
};

}

#endif /* TETRAK_FILETEMPOTRACKER_HPP */
