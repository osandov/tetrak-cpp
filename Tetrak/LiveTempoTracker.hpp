#ifndef TETRAK_LIVETEMPOTRACKER_HPP
#define TETRAK_LIVETEMPOTRACKER_HPP

#include "Tetrak/Implementation/TempoTracker.hpp"

#include "Tetrak/Implementation/Input/LiveInputter.hpp"
#include "Tetrak/Implementation/Output/GnuplotOutputter.hpp"

namespace Tetrak {

class LiveTempoTracker : public TempoTracker {
	class PortAudio {
		LiveTempoTracker* tempoTracker_;
		static constexpr double sampleRate_ = 44100.0;
		
		portaudio::AutoSystem autoSys_;
		portaudio::System &sys_;
		portaudio::DirectionSpecificStreamParameters inParams_;
		portaudio::StreamParameters params_;
		portaudio::MemFunCallbackStream<PortAudio> stream_;
		
		const float* inputBuffer_;
		
		int callback(const void* inputBuffer, void* outputBuffer,
					 unsigned long framesPerBuffer,
					 const PaStreamCallbackTimeInfo *timeInfo,
					 PaStreamCallbackFlags statusFlags);
	public:
		PortAudio(LiveTempoTracker* tempoTracker);
		
		void start() {stream_.start();}
		void stop() {stream_.stop();}
	} pa_;
	
public:
	void startStream() {pa_.start();}
	void stopStream() {pa_.stop();}
	
	LiveTempoTracker() : TempoTracker(new LiveInputter), pa_(this) {}
};

class LiveTempoTrackerProgram : public TempoTrackerProgramBase {
	virtual void waitForUser() {
		std::cout << "Press enter to finish...";
		std::cin.get();
		tempoTracker_->setComplete(true);
	}
	
public:
	LiveTempoTrackerProgram() :
		TempoTrackerProgramBase(new LiveTempoTracker) {}
	
	void execute(std::vector<TetrakArg>& args);
};

}

#endif /* TETRAK_LIVETEMPOTRACKER_HPP */
