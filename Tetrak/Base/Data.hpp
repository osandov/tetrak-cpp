#ifndef TETRAK_DATA_HPP
#define TETRAK_DATA_HPP

namespace Tetrak {

typedef double Sample;
typedef std::complex<Sample> Complex;
typedef uint Tempo;

}

#include "Tetrak/Base/Parameters.hpp"

namespace Tetrak {

class OnsetEvent {
public:
	Sample pos, energy;
};

template <class charT, class traits>
std::basic_ostream<charT,traits>& operator<<(std::basic_ostream<charT, traits>& ostr,
											 const OnsetEvent& rhs) {
	ostr << rhs.pos << ", " << rhs.energy;
	return ostr;
}

class TempoResult {
public:
	Sample pos;
	Tempo tempo;
};

template <class charT, class traits>
std::basic_ostream<charT,traits>& operator<<(std::basic_ostream<charT, traits>& ostr,
											 const TempoResult& rhs) {
	ostr << rhs.pos << ", " << rhs.tempo;
	return ostr;
}

template <std::size_t N, std::size_t M>
class TetrakData {
	TetrakParameters* params_;
	bool complete_ = false;
	bool ready_ = false;
	tetra::RealFft<N * 2> fft_;
	tetra::MelBands<N * 2, M> mel_;
	
	Sample* buffer_;
	Sample* spectBuffer_;
	
	std::size_t sampleRate_ = 0;
	
public:
	tetra::MovingAverageSmoother<Sample> avgSmooth;
	tetra::RangeSmoother<Sample> rangeSmooth;
	
	std::size_t currentPos_Frames = 0;
	std::size_t currentPos_Chunks = 0;
	Sample currentPos_Seconds = 0;
	
	Sample shift = 0, prevShift = 0;
	
	bool eventFound;
	
	// All of the following buffers use the same underlying memory block
	tetra::numarray_wrapper<Sample, N * 3> full_buffer;
	tetra::numarray_wrapper<Sample, N * 2> window_buffer;
	tetra::numarray_wrapper<Sample, N> tempo_buffer;
	
	// Buffers for FFT
	tetra::numarray_wrapper<Sample, N * 2> melInput;
	tetra::numarray_wrapper<Sample, M> melSpect;
	tetra::numarray_wrapper<Sample, M> prevMelSpect;
	
	/** A list of the events extracted from the audio data. A std::deque
	 * is used rather than a std::vector because elements are often
	 * popped from the front, for which deques perform better
	 */ 
	std::deque<OnsetEvent> events;
	
	std::vector<TempoResult> tempi;
	
	TetrakData(TetrakParameters* params) :
		params_(params), mel_(fft_), rangeSmooth(300)
	{
		try {
			buffer_ = new Sample[N * 3];
			spectBuffer_ = new Sample[M];
			
			full_buffer.elems() = buffer_;
			window_buffer.elems() = buffer_;
			tempo_buffer.elems() = buffer_ + N * 2;
			
			melInput.elems() = mel_.getIn();
			melSpect.elems() = mel_.getOut();
			prevMelSpect.elems() = spectBuffer_;
		} catch (const std::bad_alloc& e) {
			throw TetrakException(TetrakErrorCode::Fatal_OutOfMemory,
								  "Not enough memory for tempo tracker data");
		}
	}
	
	~TetrakData() {
		delete[] buffer_;
		delete[] spectBuffer_;
	}
	
	static constexpr std::size_t size() {return N;}
	
	void setComplete(bool complete) {complete_ = complete;}
	bool isComplete() const {return complete_;}
	
	void setReady(bool ready) {ready_ = ready;}
	bool isReady() const {return ready_;}
	
	void executeMel() {mel_.execute();}
	
	void setShift(Sample val) {
		prevShift = shift;
		shift = val;
	}
	
	std::size_t sampleRate() {
		return sampleRate_;
	}
	
	void sampleRate(std::size_t sampleRate) {
		sampleRate_ = sampleRate;
		mel_.setSampleRate(sampleRate_);
	}
	
	void reset() {
		complete_ = false;
		eventFound = false;
		
		currentPos_Frames = 0;
		currentPos_Chunks = 0;
		currentPos_Seconds = 0;
		// Don't reset the sampleRate
		
		avgSmooth.reset();
		avgSmooth.setWindowSize(params_->onsetSmoothWindow);
		
		rangeSmooth.setWindowSize(params_->tempoSmoothWindow);
		
		shift = 0;
		prevShift = 0;
		
		memset(full_buffer.c_array(), 0, sizeof(Sample) * N * 3);
		memset(melInput.c_array(), 0, sizeof(Sample) * N * 2);
		memset(melSpect.c_array(), 0, sizeof(Sample) * M);
		memset(prevMelSpect.c_array(), 0, sizeof(Sample) * M);
		
		events.clear();
		tempi.clear();
	}
};

}

#endif /* TETRAK_DATA_HPP */ 
