#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {

void TempoTrackerBase::setupStep(TetrakArg::CommandMarker which,
								 std::vector<TetrakArg>& args)
{
	switch (which) {
		case TetrakArg::CommandMarker::Parameters:
			params_.processSetupArgs(args);
			params_.setup(args);
		break;
		
		case TetrakArg::CommandMarker::Input:
			inputter_->processSetupArgs(args);
			inputter_->setup(args);
		break;
		
		case TetrakArg::CommandMarker::Process:
			processor_->processSetupArgs(args);
			processor_->setup(args);
		break;
		
		case TetrakArg::CommandMarker::EventExtract:
			eventExtractor_->processSetupArgs(args);
			eventExtractor_->setup(args);
		break;
		
		case TetrakArg::CommandMarker::TempoFind:
			tempoFinder_->processSetupArgs(args);
			tempoFinder_->setup(args);
		break;
		
		case TetrakArg::CommandMarker::Output:
			outputter_->processSetupArgs(args);
			outputter_->setup(args);
		break;
		default:
			throw TetrakException(TetrakErrorCode::Command,
								  "Attempting to set invalid option");
		break;
	}
}

void TempoTrackerProgramBase::execute(std::vector<TetrakArg>& args) {
	if (!tempoTracker_->isReady())
		 throw TetrakException(TetrakErrorCode::Run,
							   "System has not been set up");
	
	tempoTracker_->reset();
	
	do {
		tempoTracker_->executeCycle();
	} while (!tempoTracker_->isComplete());
}

}
