#include "Tetrak/LiveTempoTracker.hpp"

namespace Tetrak {

LiveTempoTracker::PortAudio::PortAudio(LiveTempoTracker* tempoTracker) :
	tempoTracker_(tempoTracker),
	autoSys_(), sys_(portaudio::System::instance()),
	inParams_(sys_.defaultInputDevice(), 2, portaudio::FLOAT32, false,
			  sys_.defaultInputDevice().defaultLowInputLatency(), nullptr),
	params_(inParams_, portaudio::DirectionSpecificStreamParameters::null(),
			sampleRate_, BUFFERSIZE, paClipOff),
	stream_(params_, *this, &PortAudio::callback)
{
	tempoTracker_->data_.sampleRate(sampleRate_);
	tempoTracker_->data_.setReady(true);
}

int LiveTempoTracker::PortAudio::callback(const void* inputBuffer, void* outputBuffer,
										  unsigned long framesPerBuffer,
										  const PaStreamCallbackTimeInfo *timeInfo,
										  PaStreamCallbackFlags statusFlags) 
{
	dynamic_cast<LiveInputter*>(tempoTracker_->inputter_.get())
		->setPaPointers(*static_cast<float* const*>(inputBuffer), timeInfo);
	tempoTracker_->executeCycle();
	
	return paContinue;
}

void LiveTempoTrackerProgram::execute(std::vector<TetrakArg>& args) {
	if (!tempoTracker_->isReady())
		 throw TetrakException(TetrakErrorCode::Run,
							   "System has not been set up");
	
	tempoTracker_->reset();
	dynamic_cast<LiveTempoTracker*>(tempoTracker_.get())->startStream();
	waitForUser();
	dynamic_cast<LiveTempoTracker*>(tempoTracker_.get())->stopStream();
}
		
}
