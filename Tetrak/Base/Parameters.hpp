#ifndef TETRAK_PARAMETERS_HPP
#define TETRAK_PARAMETERS_HPP

namespace Tetrak {

constexpr std::size_t BUFFERSIZE = 1024;
constexpr std::size_t MELBANDS = 40;

class TetrakParameters {
public:
	// Input settings
	
	// Processing settings
	/** The size of the smoothing window applied to onset-strength 
	 * envelope */
	std::size_t onsetSmoothWindow = 5;
	
	// Event extraction settings
	/** The factor for the weighted average threshold for adding new
	 * events */
	Sample eventThreshold = 0.5;
	
	/** The decay factor by which old events are forgotten for threshold
	 * purposes. Higher value = more forgetful. */
	Sample eventMemoryDecay = 0.5;
	
	/** The minimum possible influence an event can have */
	Sample minEventMemory = 0.2;
	
	/** The number of events which the tempo finder tests */
	std::size_t eventCount = 16;
	
	// Tempo finding settings
	/** The minimum tested tempo, inclusive */
	Tempo minTempo = 80;
	
	/** The maximum tested tempo, inclusive */
	Tempo maxTempo = 200;
	
	/** The width of a resonator impulse */
	Sample resonatorImpulseWidth = 0.05;
	
	/** Penalty for missing a note */
	Sample resonatorPenalty = 0.2;
	
	/** The smoothing window for tempo results */
	std::size_t tempoSmoothWindow = 3;
	
	// Setup
	void processSetupArgs(std::vector<TetrakArg>& args);
	void setup(std::vector<TetrakArg>& args);
};

}

#endif /* TETRAK_PARAMETERS_HPP */
