#include "Tetrak/Implementation/Output/GnuplotOutputter.hpp"

namespace Tetrak {

GnuplotOutputter::GnuplotOutputter() : gp_("gnuplot 2> /dev/null") {}

void GnuplotOutputter::processSetupArgs(std::vector<TetrakArg>& args) {
	if (args.size() != 2)
		throw TetrakException(TetrakErrorCode::Argument,
							  "Invalid number of arguments");
}

void GnuplotOutputter::setup(std::vector<TetrakArg>& args) {	
	if (args.at(0).get<std::string>() == "yrange")
		gp_ << "set yrange " << args.at(1).get<std::string>() << std::endl;
		
	else if (args.at(0).get<std::string>() == "xrange")
		gp_ << "set xrange " << args.at(1).get<std::string>() << std::endl;
		
	else if (args.at(0).get<std::string>() == "with")
		setWith(args.at(1).get<std::string>());
	
	else if (args.at(0).get<std::string>() == "title")
		gp_ << "set title '" << args.at(1).get<std::string>() << "'" << std::endl;
		
	else if (args.at(0).get<std::string>() == "data")
		setWhichData(args.at(1).get<std::string>());
	
	else if (args.at(0).get<std::string>() == "everycycle")
		if (args.at(1).get<std::string>() == "true")
			everyCycle_ = true;
		else if (args.at(1).get<std::string>() == "false")
			everyCycle_ = false;
		else
			throw TetrakException(TetrakErrorCode::Argument, "everycycle must be true or false");
	
	else
		throw TetrakException(TetrakErrorCode::Argument, args.at(0).get<std::string>());
	
	gp_ << std::endl;
}

void GnuplotOutputter::setWith(const std::string& with) {
	static std::string styles[] = {"lines", "points", "linespoints", "dots", "impulses", "vectors",
								   "yerrorbars", "xerrorbars", "xyerrorbars", "steps", "fs"};
	
	for (const std::string& s : styles) {
		if (s == with) {
			gp_ << "set style data " << with << std::endl;
			return;
		}
	}
	throw TetrakException(TetrakErrorCode::Argument, with);
}

void GnuplotOutputter::setWhichData(const std::string& whichData) {
	if (whichData == "buffer") {
		gp_ << "set xrange [*:*]" << std::endl;
		gp_ << "set yrange [*:*]" << std::endl;
		setWith("lines");
	} else if (whichData == "events") {
		gp_ << "set xrange [*:*]" << std::endl;
		gp_ << "set yrange [*:*]" << std::endl;
		setWith("vectors");
	} else if (whichData == "tempi") {
		gp_ << "set xrange [*:*]" << std::endl;
		gp_ << "set yrange [" << params_->minTempo << ':' << params_->maxTempo << ']' << std::endl;
		setWith("lines");
	} else if (whichData == "freq") {
		gp_ << "set xrange [*:*]" << std::endl;
		gp_ << "set yrange [*:*]" << std::endl;
		setWith("lines");
	} else if (whichData == "temporesults") {
		gp_ << "set xrange [" << params_->minTempo << ':' << params_->maxTempo << ']' << std::endl;
		gp_ << "set yrange [*:*]" << std::endl;
		setWith("lines");
	} else {
		throw TetrakException(TetrakErrorCode::Argument, whichData);
	}
	whichData_ = whichData;
}

template <typename Iterator>
void GnuplotOutputter::plot(Iterator first, Iterator last, const std::string& dataTitle) {
	if (last - first > 0) {
		gp_ << "plot '-' title '" << dataTitle << "'" << std::endl;
		gp_.send(first, last);
		
		gp_ << std::endl;
	}
}

void GnuplotOutputter::execute() {
	if (whichData_.empty())
		setWhichData("tempi");
	
	if (whichData_ == "buffer") {
		gp_ << "set xlabel" << std::endl;
		gp_ << "set ylabel" << std::endl;
		plot(data_->window_buffer.begin(), data_->window_buffer.end(), "Buffer");
		
	} else if (whichData_ == "events") {
		gp_ << "set xlabel 'Time (sec)'" << std::endl;
		gp_ << "set ylabel 'Energy'" << std::endl;
		plot(data_->events.begin(), data_->events.end(), "Onset Events");
		
	} else if (whichData_ == "tempi") {
		gp_ << "set xlabel 'Time (sec)'" << std::endl;
		gp_ << "set ylabel 'Tempo (BPM)'" << std::endl;
		plot(data_->tempi.begin(), data_->tempi.end(), "Tempo");
		
	} else if (whichData_ == "freq") {
		gp_ << "set xlabel 'Frequency (Mel bands)'" << std::endl;
		gp_ << "set ylabel 'Power'" << std::endl;
		plot(data_->melSpect.begin(), data_->melSpect.end(), "Frequency-Domain");
		
	} else if (whichData_ == "temporesults") {
		gp_ << "set xlabel 'Tempo (BPM)'" << std::endl;
		gp_ << "set ylabel 'Resonance'" << std::endl;
		plot(data_->tempo_buffer.begin(), data_->tempo_buffer.end(), "Tempo Results");
	}
}

}
