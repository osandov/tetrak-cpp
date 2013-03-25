#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {

void TetrakParameters::processSetupArgs(std::vector<TetrakArg>& args) {
	if (args.size() != 2)
		throw TetrakException(TetrakErrorCode::Argument,
							  "Invalid number of arguments");
	
	args.at(1).convert<Sample>();
	
	if (args.at(1).get<Sample>() <= 0)
		throw TetrakException(TetrakErrorCode::Argument,
							  "Invalid parameter value");
}

void TetrakParameters::setup(std::vector<TetrakArg>& args) {
	if (args.at(0).get<std::string>() == "onsetsmooth")
		onsetSmoothWindow = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "eventthresh")
		eventThreshold = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "eventdecay")
		eventMemoryDecay = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "mineventmem")
		minEventMemory = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "eventcount")
		eventCount = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "mintempo")
		minTempo = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "maxtempo")
		maxTempo = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "impulsewidth")
		resonatorImpulseWidth = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "penalty")
		resonatorPenalty = args.at(1).get<Sample>();
	
	else if (args.at(0).get<std::string>() == "temposmooth")
		tempoSmoothWindow = args.at(1).get<Sample>();
	
	else
		throw TetrakException(TetrakErrorCode::Argument,
							  args.at(0).get<std::string>());
}

}
