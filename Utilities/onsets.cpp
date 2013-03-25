#include <tetra/numarray_wrapper.hpp>
#include <tetra/Fft.hpp>
#include <tetra/Smoother.hpp>
#include <sndfile.h>
#include <Gnuplot.hpp>
#include <iostream>

const std::size_t BUFFERSIZE = 1024, NUMBANDS = 40;

int main(int argc, const char *argv[]) {
	if (argc < 2) {
		std::cout << "Must specify input" << std::endl;
		return 1;
	}
	
	const char* filename = argv[1];
	
	SF_INFO fileInfo;
	SNDFILE* file = sf_open(filename, SFM_READ, &fileInfo);
	if (file == nullptr) {
		std::cout << "Could not open file" << std::endl;
		return 1;
	}
	
	tetra::RealFft<BUFFERSIZE * 2> rfft;
	tetra::MelBands<BUFFERSIZE * 2, NUMBANDS> melBands(rfft);
	tetra::MovingAverageSmoother<double> smoother(5);
	
	melBands.setSampleRate(fileInfo.samplerate);
	double buffer[BUFFERSIZE * 3];
	memset(buffer, 0, sizeof(double) * BUFFERSIZE * 3);
	
	double prev[NUMBANDS];
	memset(prev, 0, sizeof(double) * NUMBANDS);
	
	Gnuplot gp;
	
	gp << "set xlabel 'Time (sec)'" << std::endl;
	gp << "set ylabel 'Onset strength'" << std::endl;
	
	gp << "plot '-' with lines title 'Onset-strength envelope'" << std::endl;
	
	double curSeconds = 0.0;
	while (true) {
		memcpy(buffer, buffer + BUFFERSIZE, sizeof(double) * BUFFERSIZE);
		sf_count_t framesread = sf_readf_double(file, buffer + BUFFERSIZE, BUFFERSIZE);
		curSeconds = double(sf_seek(file, 0, SEEK_CUR)) / fileInfo.samplerate;
		
		if (std::size_t(framesread) < BUFFERSIZE)
			break;
			
		if (fileInfo.channels == 2)
			tetra::stereoToMonoInterleaved(buffer + BUFFERSIZE, buffer + 3 * BUFFERSIZE, buffer + BUFFERSIZE);
		
		memcpy(melBands.getIn(), buffer, sizeof(double) * BUFFERSIZE * 2);
		
		melBands.execute();
		
		double* result = melBands.getOut();
		
		double r = 0.0;
		for (std::size_t i = 0; i < NUMBANDS; ++i) 
			r += tetra::ramp(result[i] - prev[i]);
		gp << curSeconds
		   << ", " << smoother(smoother(r)) << '\n';
		
		memcpy(prev, result, sizeof(double) * NUMBANDS);
	}
	gp << 'e' << std::endl;
	
	std::cin.get();
	
	sf_close(file);
}
