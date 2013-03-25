#ifndef TETRAK_GNUPLOTOUTPUTTER_HPP
#define TETRAK_GNUPLOTOUTPUTTER_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"
#define GNUPLOT_ENABLE_PTY
#include "Tetrak/Implementation/Output/Gnuplot.hpp"

namespace Tetrak {

class GnuplotOutputter : public OutputterBase {
protected:
	Gnuplot gp_;
	std::string whichData_;
	
	template <typename Iterator>
	void plot(Iterator first, Iterator last, const std::string& dataTitle = "");
	
	void setWith(const std::string& whichData);
	void setWhichData(const std::string& whichData);
	
public:
	GnuplotOutputter();
	
	void processSetupArgs(std::vector<TetrakArg>& args);
	void setup(std::vector<TetrakArg>& args);
	void execute();
};

}

#endif /* TETRAK_GNUPLOTOUTPUTTER_HPP */
