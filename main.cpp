#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "Tetrak/FileTempoTracker.hpp"
#include "Tetrak/LiveTempoTracker.hpp"

using namespace Tetrak;

template <typename TTP>
class TempoTrackerProgram_CommandLine : public TTP {
private:
	/** @class CliOption
	 * @brief Command-line interface invoke options
	 */
	class CliOption {
	private:
		typedef void (*Function)(TempoTrackerProgram_CommandLine*, std::string&);
		Function function_;
		
	public:
		bool needsArgument;
		
		// Need argument
		static void set(TempoTrackerProgram_CommandLine* program, std::string& arg) {
			program->command_ = "set " + arg;
			std::cout << program->command_ << std::endl;
			program->parseCommandLine();
			program->handleSetup(program->args_);
		}
		
		// Does not need argument
		static void autoexec(TempoTrackerProgram_CommandLine* program, std::string&) {
			program->triggeredOptions_.autoexec = true;
		}
		// Implies -auto
		static void autoexit(TempoTrackerProgram_CommandLine* program, std::string&) {
			program->triggeredOptions_.autoexec = true;
			program->triggeredOptions_.nointeract = true;
		}
		// Implies -autoexit
		static void autobatch(TempoTrackerProgram_CommandLine* program, std::string&) {
			program->triggeredOptions_.autoexec = true;
			program->triggeredOptions_.nointeract = true;
			program->triggeredOptions_.nowait = true;
		}
		
		void function(TempoTrackerProgram_CommandLine* program, std::string& arg) {
			program->args_.clear();
			(*function_)(program, arg);
		}
		
		CliOption() : function_(nullptr), needsArgument(false) {}
		CliOption(Function function, bool needsArgument) :
				  function_(function), needsArgument(needsArgument) {}
	};
	
	/** @class CliOption
	 * @brief Command-line interface invoke options
	 */
	class InvokeOptions {
	public:
		bool autoexec = false;
		bool nointeract = false;
		bool nowait = false;
	} triggeredOptions_;
	
	typedef void (TempoTrackerProgram_CommandLine::*Function)(std::vector<TetrakArg>&);
	static constexpr const char* queryString_ = "tetrak> ";
	std::unordered_map<std::string, TempoTrackerProgram_CommandLine::Function> functionHash_;
	std::unordered_map<std::string, CliOption> invokeOptionHash_;
	
	Function function_;
	std::vector<TetrakArg> args_;
	std::string command_;
	
	void parseInvokeOptions(int argc, const char *argv[]);
	void handleTriggeredOptions();
	
	void parseCommandLine();
	
	void handleSetup(std::vector<TetrakArg>& args);
	
public:
	TempoTrackerProgram_CommandLine();
	
	void main(int argc, const char *argv[]);
};

template <typename TTP>
TempoTrackerProgram_CommandLine<TTP>::TempoTrackerProgram_CommandLine() {
	// Commands that can be called via CLI
	functionHash_["exec"] = functionHash_["execute"] = &TempoTrackerProgram_CommandLine::execute;
	functionHash_["set"] = &TempoTrackerProgram_CommandLine::handleSetup;
	functionHash_["output"] = &TempoTrackerProgram_CommandLine::executeOutput;
	functionHash_["exit"] = &TempoTrackerProgram_CommandLine::exit;
	
	// Invoke options
	invokeOptionHash_["-param"] = CliOption(&CliOption::set, true);
	invokeOptionHash_["-input"] = invokeOptionHash_["-in"] = CliOption(&CliOption::set, true);
	invokeOptionHash_["-process"] = CliOption(&CliOption::set, true);
	invokeOptionHash_["-extract"] = CliOption(&CliOption::set, true);
	invokeOptionHash_["-find"] = CliOption(&CliOption::set, true);
	invokeOptionHash_["-output"] = invokeOptionHash_["-out"] = CliOption(&CliOption::set, true);
	
	invokeOptionHash_["-auto"] = CliOption(&CliOption::autoexec, false);
	invokeOptionHash_["-autoexit"] = CliOption(&CliOption::autoexit, false);
	invokeOptionHash_["-autobatch"] = CliOption(&CliOption::autobatch, false);
	
	this->tempoTracker_->setOutput(new GnuplotOutputter);
	args_.reserve(10);
}

template <typename TTP>
void TempoTrackerProgram_CommandLine<TTP>::parseInvokeOptions(int argc, const char *argv[]) {
	bool needArgument = false;
	std::string argument, option;
	
	// For each argument
	for (int i = 1; i < argc; ++i) {
		argument = argv[i];
		
		// If the argument is an option denoted by a hyphen (e.g. -foo)
		if (argument.at(0) == '-') {
			option = argument;
			
			// If an argument for an option was expected, throw
			if (needArgument)
				throw TetrakException(TetrakErrorCode::Parsing,
									  "Expected argument, got another option");
			
			// Check whether the option exists and whether it requires
			// an argument
			if (invokeOptionHash_.count(option) != 0)
				needArgument = invokeOptionHash_[option].needsArgument;
			else
				throw TetrakException(TetrakErrorCode::Option, option);
		
		// If an argument for an option is needed
		} else if (needArgument) {
			needArgument = false;
		} else {
			throw TetrakException(TetrakErrorCode::Parsing,
								  "Argument without option");
		}
		
		// Execute the desired function
		if (!needArgument) {
			if (invokeOptionHash_.count(option) != 0) {
				argument = option.substr(1) + ' ' + argument;
				invokeOptionHash_[option].function(this, argument);
			}
		}
	}
	// If the loop was exited before finding an argument
	if (needArgument)
		throw TetrakException(TetrakErrorCode::Parsing,
							  "Expected argument");
}

template <typename TTP>
void TempoTrackerProgram_CommandLine<TTP>::handleTriggeredOptions() {
	if (triggeredOptions_.autoexec) {
		std::cout << "Automatically executing..." << std::endl;
		this->execute(args_);
	}
	
	if (triggeredOptions_.nointeract) {
		if (!triggeredOptions_.nowait) {
			std::cout << "Press enter to exit...";
			std::cin.get();
		}
		throw TetrakException(TetrakErrorCode::Exit);
	}
}

template <typename TTP>
void TempoTrackerProgram_CommandLine<TTP>::parseCommandLine() {
	std::string command = command_;
	
	// While the string has not been exhausted
	while (command.size() > 0) {
		// Strip whitespace
		if (command.at(0) == ' ') {
			command.erase(command.begin());
			
		// Parse based on whitespace if there are no quotes
		} else if (command.at(0) != '"' && command.at(0) != '\'') {
			size_t pos = command.find(' ');
			if (pos == std::string::npos) {
				args_.push_back(TetrakArg(command));
				break;
			}
			
			if (command.substr(0, pos).back() != ',') {
				args_.push_back(TetrakArg(command.substr(0, pos)));
				command = command.substr(pos + 1);
			} else {
				args_.push_back(TetrakArg(command.substr(0, pos - 1)));
				args_.push_back(TetrakArg(TetrakArg::CommandMarker::Separator));
				command = command.substr(pos + 1);
			}
			
		// Otherwise, parse based on quotes
		} else { // i.e., if (command.at(0) == '"' || command.at(0) == '\'')
			char delimiter = command.at(0);
			if (command.size() == 1)
				throw TetrakException(TetrakErrorCode::Parsing,
									  "No matching closing delimiter was found.");
			
			size_t pos = command.find(delimiter, 1);
			if (pos == std::string::npos)
				throw TetrakException(TetrakErrorCode::Parsing,
									  "No matching closing delimiter was found.");
			
			if (command.substr(0, pos).back() != ',') {
				args_.push_back(TetrakArg(command.substr(1, pos - 1)));
				command = command.substr(pos + 1);
			} else {
				args_.push_back(TetrakArg(command.substr(1, pos - 2)));
				args_.push_back(TetrakArg(TetrakArg::CommandMarker::Separator));
				command = command.substr(pos + 1);
			}
		}
	}
}

template <typename TTP>
void TempoTrackerProgram_CommandLine<TTP>::handleSetup(std::vector<TetrakArg>& args) {
	args.erase(args.begin());
	args.front().convert<TetrakArg::CommandMarker>();
	TetrakArg::CommandMarker which = args.front().get<TetrakArg::CommandMarker>();
	args.erase(args.begin());
	if (args.size() == 0)
		throw TetrakException(TetrakErrorCode::Argument, "Must specify options to set");
	std::vector<TetrakArg> temp(3);
	temp.clear();
	for (auto i : args) {
		if (i.getType() == TetrakArg::Type::Marker) {
			this->setupStep(which, temp);
			temp.clear();
		} else {
			temp.push_back(i);
		}
	}
	if (temp.size() > 0)
		this->setupStep(which, temp);
}

template <typename TTP>
void TempoTrackerProgram_CommandLine<TTP>::main(int argc, const char *argv[]) {
	bool exit = false;
	try {
		parseInvokeOptions(argc, argv);
		handleTriggeredOptions();
	} catch (const TetrakException& e) {
		exit = true;
		std::cout << e.what() << std::endl;
	}
	
	while (!exit) {
		try {
			std::cout << queryString_;
			getline(std::cin, command_);
			args_.clear();
			parseCommandLine();
			
			// If the user didn't input anything
			if (args_.size() == 0)
				continue;
			
			// Check for valid command
			if (functionHash_.count(args_.at(0).get<std::string>()) != 0) {
				function_ = functionHash_[args_.at(0).get<std::string>()];
				(*this.*function_)(args_);
			} else {
				throw TetrakException(TetrakErrorCode::Command, args_.at(0).get<const char*>());
			}
		} catch (const TetrakException& e) {
			std::cout << e.what() << std::endl;
			if (e.code() == TetrakErrorCode::Exit)
				exit = true;
			else if (e.code() >= TetrakErrorCode::Fatal_Generic)
				exit = true;
		}
	}
}

int main(int argc, const char *argv[]) {
	// Default mode is file
	bool modeFile = true;
	
	if (argc > 1) {
		if (strcmp(argv[1], "-Mlive") == 0) {
			modeFile = false;
			// Cut out the mode option
			if (argc > 2)
				std::copy_backward(argv + 2, argv + argc, argv + (argc - 1));
			--argc;
		} else if (strcmp(argv[1], "-Mfile") == 0) {
			modeFile = true;
			// Cut out the mode option
			if (argc > 2)
				std::copy_backward(argv + 2, argv + argc, argv + (argc - 1));
			--argc;
		}
	}
	
	if (modeFile) {
		TempoTrackerProgram_CommandLine<FileTempoTrackerProgram> program;
		program.main(argc, argv);
	} else {
		TempoTrackerProgram_CommandLine<LiveTempoTrackerProgram> program;
		program.main(argc, argv);
	}
}
