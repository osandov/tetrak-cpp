#include "tetra/Fft.hpp"
#include <Gnuplot.hpp>
#include <sndfile.h>
#include <stdexcept>

template <std::size_t N>
class BaseInputter {
protected:
	double currentTime_ = 0.0;
	bool done_ = false;
	double* buffer_ = nullptr;
	
public:
	BaseInputter() : buffer_(new double[N * 3])
	{
		memset(buffer_, 0, sizeof(double) * N);
	}
	~BaseInputter() {
		delete[] buffer_;
	}
	
	virtual void setup(std::string& arg) = 0;
	virtual void setup(const char* arg) = 0;
	virtual void execute() = 0;
	
	double getCurrentTime() {return currentTime_;}
	bool isDone() {return done_;}
	double* getBuffer() {return buffer_;}
	
	virtual double sampleRate() = 0;
};

template <std::size_t N>
class FileInputter : public BaseInputter<N> {
	SF_INFO fileInfo_;
	SNDFILE* file_ = nullptr;
	
public:
	~FileInputter() {
		sf_close(file_);
	}
	
	void setup(std::string& fileName) {
		setup(fileName.c_str());
	}
	
	void setup(const char* fileName) {
		file_ = sf_open(fileName, SFM_READ, &fileInfo_);
		if (file_ == nullptr)
			throw std::runtime_error("Could not open file");
	}
	
	void execute() {
		memcpy(BaseInputter<N>::buffer_, BaseInputter<N>::buffer_ + N, N);
		sf_count_t framesread = sf_readf_double(file_, BaseInputter<N>::buffer_ + N, N);
		if (fileInfo_.channels == 2)
			tetra::stereoToMonoInterleaved(BaseInputter<N>::buffer_ + N,
					BaseInputter<N>::buffer_ + 3 * N, BaseInputter<N>::buffer_ + N);
		BaseInputter<N>::currentTime_ = double(sf_seek(file_, 0, SEEK_CUR)) / sampleRate();
		
		if (std::size_t(framesread) < N)
			BaseInputter<N>::done_ = true;
	}
	
	double sampleRate() {return fileInfo_.samplerate;}
};

template <std::size_t N>
class LiveInputter : public BaseInputter<N> {
	
};

template <std::size_t N, typename Inputter>
class Spectrograph {
protected:
	tetra::RealFft<N * 2> fft_;
	Gnuplot gp_;
	Inputter inputter_;
	
public:
	Spectrograph() {
		gp_ << "set pm3d map" << std::endl;
		gp_ << "set xlabel 'Time (sec)'" << std::endl;
		gp_ << "set ylabel 'Frequency (Hz)'" << std::endl;
		gp_ << "set cblabel 'Power'" << std::endl;
		//~ gp_ << "set cbrange [0:0.0007]" << std::endl;
	}
	
	Spectrograph(std::string& fileName) : Spectrograph()
	{
		openFile(fileName);
	}
	
	Spectrograph(const char* fileName) : Spectrograph()
	{
		openFile(fileName);
	}
	
	void sendToGnuplot(const std::string& str) {
		gp_ << str << std::endl;
	}
	
	virtual void openFile(std::string& fileName) {
		inputter_.setup(fileName);
	}
	
	virtual void openFile(const char* fileName) {
		inputter_.setup(fileName);
	}
	
	virtual void execute() = 0;
};

template <std::size_t N, typename Inputter>
class FftSpectrograph : public Spectrograph<N, Inputter> {
public:
	FftSpectrograph() : Spectrograph<N, Inputter>()	{}
	
	FftSpectrograph(std::string& fileName) : Spectrograph<N, Inputter>(fileName) {}
	
	FftSpectrograph(const char* fileName) : Spectrograph<N, Inputter>(fileName) {}
	
	void execute() {
		this->gp_ << "splot '-' using 1:2:3 with pm3d" << std::endl;
		while (!this->inputter_.isDone()) {
			this->inputter_.execute();
			memcpy(this->fft_.getIn(),
				this->inputter_.getBuffer(), sizeof(double) * N * 2);
			this->fft_.applyHann();
			this->fft_.executeNormalizedPowerSpectrum();
			double* spectrum = this->fft_.getPowerSpectrum();
			
			double x = this->inputter_.getCurrentTime();
			
			for (std::size_t i = 0; i < this->fft_.hzToBin(5000.0, this->inputter_.sampleRate()); ++i)
				this->gp_ << x << ", "
					<< this->fft_.binToHz(i, this->inputter_.sampleRate()) << ", "
					<< *spectrum++ << '\n';
			this->gp_ << '\n';
		}
		this->gp_ << 'e' << std::endl;
	}
};

template <std::size_t N, std::size_t M, typename Inputter>
class MelSpectrograph : public Spectrograph<N, Inputter> {
	tetra::MelBands<N * 2, M> mel_;
	
public:
	MelSpectrograph() : Spectrograph<N, Inputter>(), mel_(this->fft_)
	{
		this->gp_ << "set ylabel 'Mel bands'" << std::endl;
	}
	
	MelSpectrograph(std::string& fileName) : Spectrograph<N, Inputter>(fileName),
		mel_(this->fft_)
	{
		this->gp_ << "set ylabel 'Mel bands'" << std::endl;
		mel_.setSampleRate(this->inputter_.sampleRate());
	}
	
	MelSpectrograph(const char* fileName) : Spectrograph<N, Inputter>(fileName),
		mel_(this->fft_)
	{
		this->gp_ << "set ylabel 'Mel bands'" << std::endl;
		mel_.setSampleRate(this->inputter_.sampleRate());
	}
	
	void execute() {
		this->gp_ << "splot '-' using 1:2:3 with pm3d" << std::endl;
		while (!this->inputter_.isDone()) {
			this->inputter_.execute();
			memcpy(this->mel_.getIn(),
				this->inputter_.getBuffer(), sizeof(double) * N * 2);
			this->mel_.execute();
			double* mfcc = this->mel_.getOut();
			
			double x = this->inputter_.getCurrentTime();
			
			for (std::size_t i = 0; i < M; ++i)
				this->gp_ << x << ", "
					<< i << ", "
					<< *mfcc++ << '\n';
			this->gp_ << '\n';
		}
		this->gp_ << 'e' << std::endl;
	}
	
	void openFile(std::string& fileName) {
		this->inputter_.setup(fileName);
		this->mel_.setSampleRate(this->inputter_.sampleRate());
	}
	
	void openFile(const char* fileName) {
		this->inputter_.setup(fileName);
		this->mel_.setSampleRate(this->inputter_.sampleRate());
	}
};

int main(int argc, const char *argv[]) {
	try {
		if (argc <= 1)
			throw std::runtime_error("Must specify spectrogram mode or input");
		
		std::string first = argv[1], fileName;
		int spectMode = 0;
		
		if (first.at(0) == '-') {
			if (first == "-fft")
				spectMode = 1;
			else if (first != "-mel")
				throw std::runtime_error("Spectrogram mode must be either -mel (default) or -fft");
			
			if (argc < 3)
				throw std::runtime_error("Must specify spectrogram mode or input");
			fileName = argv[2];
		} else {
			fileName = argv[1];
		}
		
		if (spectMode == 0) {
			MelSpectrograph<1024, 40, FileInputter<1024>> spect(fileName);
			spect.execute();
			
			std::cin.get();
		} else {
			FftSpectrograph<1024, FileInputter<1024>> spect(fileName);
			spect.execute();
			
			std::cin.get();
		}
	} catch (const std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
}
