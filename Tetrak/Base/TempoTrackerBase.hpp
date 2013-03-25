#ifndef TETRAK_TEMPOTRACKERBASE_HPP
#define TETRAK_TEMPOTRACKERBASE_HPP

#include "Tetrak/Base/Dependencies.hpp"

#include "Tetrak/Base/Exception.hpp"
#include "Tetrak/Base/Argument.hpp"
#include "Tetrak/Base/Data.hpp"
#include "Tetrak/Base/Parameters.hpp"

#include "Tetrak/Base/AlgorithmStepBase.hpp"

#include "Tetrak/Base/InputterBase.hpp"
#include "Tetrak/Base/ProcessorBase.hpp"
#include "Tetrak/Base/EventExtractorBase.hpp"
#include "Tetrak/Base/TempoFinderBase.hpp"
#include "Tetrak/Base/OutputterBase.hpp"

#include <iostream>

namespace Tetrak {

class TempoTrackerBase {
protected:
	TetrakParameters params_;
	TetrakData<BUFFERSIZE, MELBANDS> data_;
	
	std::shared_ptr<InputterBase> inputter_;
	std::shared_ptr<ProcessorBase> processor_;
	std::shared_ptr<EventExtractorBase> eventExtractor_;
	std::shared_ptr<TempoFinderBase> tempoFinder_;
	std::shared_ptr<OutputterBase> outputter_;
	
	void executeInput() {inputter_->execute();}
	
	void executeProcess() {processor_->execute();}
	
	void executeEventExtract() {eventExtractor_->execute();}
	
	void executeTempoFind() {tempoFinder_->execute();}
	
	TempoTrackerBase(InputterBase* inputter, ProcessorBase* processor,
					 EventExtractorBase* eventExtractor, TempoFinderBase* tempoFinder) :
					 data_(&params_), inputter_(inputter), processor_(processor),
					 eventExtractor_(eventExtractor), tempoFinder_(tempoFinder),
					 outputter_(nullptr)
	{
		inputter_->init(&data_, &params_);
		processor_->init(&data_, &params_);
		eventExtractor_->init(&data_, &params_);
		tempoFinder_->init(&data_, &params_);
	}
	
public:
	void setupStep(TetrakArg::CommandMarker which, std::vector<TetrakArg>& args);
	
	virtual void executeCycle() {
		throw TetrakException(TetrakErrorCode::NotImplemented);
	}
	
	virtual void executeOutput() {
		if (outputter_->everyCycle() || isComplete())
			outputter_->execute();
	}
	
	void setOutput(OutputterBase* outputter) {
		outputter_.reset(outputter);
		outputter->init(&data_, &params_);
	}
	
	void setComplete(bool complete) {data_.setComplete(complete);}
	bool isComplete() {return data_.isComplete();}
	
	bool isReady() {return data_.isReady();}
	
	void reset() {
		inputter_->reset();
		eventExtractor_->reset();
		tempoFinder_->reset();
		outputter_->reset();
		data_.reset();
	}
};

class TempoTrackerProgramBase {
protected:
	std::vector<TetrakArg> inputArgs_, processArgs_, extractArgs_,
						   findArgs_, outputArgs_;
	
	std::shared_ptr<TempoTrackerBase> tempoTracker_;
	
	TempoTrackerProgramBase(TempoTrackerBase* tempoTracker) :
		tempoTracker_(tempoTracker) {}
	
	void setupStep(TetrakArg::CommandMarker which, std::vector<TetrakArg>& args) {
		tempoTracker_->setupStep(which, args);
	}
	
public:
	void setOutput(OutputterBase* outputter) {
		tempoTracker_->setOutput(outputter);
	}
	
	void executeOutput(std::vector<TetrakArg>& args) {
		tempoTracker_->executeOutput();
	}
	
	virtual void execute(std::vector<TetrakArg>& args);
	
	virtual void exit(std::vector<TetrakArg>& args) {
		throw TetrakException(TetrakErrorCode::Exit);
	}
	
	virtual void main(int argc, const char *argv[]) {
		throw TetrakException(TetrakErrorCode::NotImplemented);
	}
};

}

#endif /* TETRAK_TEMPOTRACKERBASE_HPP */
