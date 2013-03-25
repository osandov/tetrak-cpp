#include <iostream>
#include <portaudiocpp/PortAudioCpp.hxx>
#include <tetra/math_utils.hpp>

const int NUM_SECONDS = 5;
const double SAMPLE_RATE = 44100.0;
const int FRAMES_PER_BUFFER = 1024;
const double clickLength = 0.1;
const double clickFreq = 50.0;

class Metronome {
	double tempo_;
	bool needRampDown_ = false;
	double t = 0.0, x = 0.0;
	float last_ = 0.f;
	
public:
	Metronome(double tempo) : tempo_(tempo) {};
	
	int callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, 
			   const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
	{
		float* out = *static_cast<float**>(outputBuffer);
		bool needSilence = true;
		
		t += FRAMES_PER_BUFFER / SAMPLE_RATE;
		
		if (t < clickLength) {
			for (int i = 0; i < FRAMES_PER_BUFFER; ++i) {
				out[i] = 0.5f * sin(2 * M_PI * x);
				x += 1.0 / clickFreq;
				if (x > clickFreq)
					x -= clickFreq;
			}
			last_ = out[FRAMES_PER_BUFFER - 1];
			needSilence = false;
			needRampDown_ = true;
		} else if (t > 60 / tempo_) {
			t -= 60 / tempo_;
		}
		
		if (needSilence) {
			int i = 0;
			double sine;
			
			if (needRampDown_) {
				for (; i < FRAMES_PER_BUFFER; ++i) {
					sine = 0.5f * sin(2 * M_PI * x);
					if ((sine >= 0.0 && last_ < 0.0) || (sine <= 0.0 && last_ > 0.0))
						needRampDown_ = false;
					out[i] = sine;
					x += 1.0 / clickFreq;
					if (x > clickFreq)
						x -= clickFreq;
				}
			}
			
			x = 0.0;
			for (; i < FRAMES_PER_BUFFER; ++i) {
				sine = 0.5f * sin(2 * M_PI * x);
				out[i] = 0.0;
			}
		}
		
		return paContinue;
	}
	
	void restart() {
		t = x = 0.0;
		last_ = 0.f;
		needRampDown_ = false;
	}
};

int main(int argc, char *argv[]) {
	try {
		if (argc < 2) {
			std::cout << "Must specify tempo" << std::endl;
			return 1;
		}
		
		double tempo = atof(argv[1]);
		
		if (tempo <= 0.0) {
			std::cout << "Invalid tempo" << std::endl;
			return 1;
		}
		
		Metronome test(tempo);
		
		// PortAudio setup
		std::cout << "Setting up PortAudio..." << std::endl;
		portaudio::AutoSystem autoSys;
		portaudio::System &sys = portaudio::System::instance();
		
		// Stream setup
		std::cout << "Setting up and starting output stream..." << std::endl;
		
		portaudio::DirectionSpecificStreamParameters outParams(sys.defaultOutputDevice(), 1, portaudio::FLOAT32, false,
															  sys.defaultOutputDevice().defaultLowOutputLatency(), nullptr);
		
		portaudio::StreamParameters params(portaudio::DirectionSpecificStreamParameters::null(), outParams, SAMPLE_RATE,
										   FRAMES_PER_BUFFER, paClipOff);
		
		portaudio::MemFunCallbackStream<Metronome> stream(params, test, &Metronome::callback);
		
		// Playback
		std::cout << "Running..." << std::endl;
		stream.start();
		
		std::cout << "Press enter to finish..." << std::endl;
		std::cin.get();
		
		//~ do {
			//~ int read = std::cin.get();
			//~ if (read != '\n')
				//~ break;
			//~ test.restart();
		//~ } while(true);
	
	} catch (const portaudio::PaException &e) {
		std::cout << "A PortAudio error occured: " << e.paErrorText() << std::endl;
	} catch (const portaudio::PaCppException &e) {
		std::cout << "A PortAudioCpp error occured: " << e.what() << std::endl;
	} catch (const std::exception &e) {
		std::cout << "A generic error occured: " << e.what() << std::endl;
	} catch (...) {
		std::cout << "An unknown exception occured." << std::endl;
	}
}
