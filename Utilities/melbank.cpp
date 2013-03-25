#include <tetra/Fft.hpp>
#include <Gnuplot.hpp>
#include <iostream>

int main(int argc, const char *argv[]) {
	const std::size_t BUFFERSIZE = 2048, NUMBANDS = 20;
	tetra::RealFft<BUFFERSIZE> rfft;
	tetra::MelBands<BUFFERSIZE, NUMBANDS> melFil(rfft);
	melFil.setSampleRate(44100.0);
	
	Gnuplot gp;
	gp << "set xrange [0:" << BUFFERSIZE / 2 << ']' << std::endl;
	gp << "plot ";
	for (std::size_t i = 0; i < NUMBANDS - 1; ++i)
		gp << "'-' with lines title 'FFT bins "
		   << melFil.getFilterStart(i) << " - "
		   << melFil.getFilterStop(i) << "' linewidth 2, ";
	
	gp << "'-' with lines title 'FFT bins "
	   << melFil.getFilterStart(NUMBANDS - 1) << " - "
	   << melFil.getFilterStop(NUMBANDS - 1) << "' linewidth 2" << std::endl;
	
	for (std::size_t i = 0; i < NUMBANDS; ++i) {
		for (std::size_t j = 0; j < BUFFERSIZE / 2; ++j)
			gp << melFil.getFilterBankAt(i, j) << '\n';
		gp << 'e' << std::endl;
	}
	
	std::cin.get();
}
