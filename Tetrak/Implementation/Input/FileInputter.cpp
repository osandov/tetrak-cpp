#include "Tetrak/Implementation/Input/FileInputter.hpp"

namespace Tetrak {

void FileInputter::readData() {
	sf_count_t count = sf_readf_double(file_,
						data_->full_buffer.c_array() + data_->size(),
						data_->size());
	if (fileInfo_.channels == 2)
		tetra::stereoToMonoInterleaved(data_->full_buffer.begin() + data_->size(),
									   data_->full_buffer.end(),
									   data_->full_buffer.begin() + data_->size());
	
	if (std::size_t(count) < data_->size()) {
		if (count > 0)
			data_->full_buffer[tetra::slice(data_->size() + count)] = 0;
		data_->setComplete(true);
	}
}

void FileInputter::reset() {
	sf_seek(file_, 0, SEEK_SET);
}

void FileInputter::processSetupArgs(std::vector<TetrakArg>& args) {
	if (args.size() == 2) {
		args.at(1).convert<Sample>();
		
		if (args.at(1).get<Sample>() < 0)
			throw TetrakException(TetrakErrorCode::Argument,
								  "Invalid parameter value");
	}
}

void FileInputter::setup(std::vector<TetrakArg>& args) {
	// Set the input file
	if (args.size() == 1) {
		sf_close(file_);
		
		const char* fileName_ = args.at(0).get<const char*>();
		file_ = sf_open(fileName_, SFM_READ, &fileInfo_);
		
		if (file_ == nullptr)
			throw TetrakException(TetrakErrorCode::FileIO, sf_strerror(file_));
		
		if (fileInfo_.channels < 1 || fileInfo_.channels > 2)
			throw TetrakException(TetrakErrorCode::InvalidFormat,
								  "File input data is not stereo or mono.");
		
		data_->sampleRate(fileInfo_.samplerate);
		
		data_->setReady(true);
		
	// Set the input length
	} else if (args.size() == 2) {
		if (args.at(0).get<std::string>() == "len")
			len_ = args.at(1).get<Sample>();
		else
			throw TetrakException(TetrakErrorCode::Argument,
								  args.at(0).get<std::string>());
	} else {
		throw TetrakException(TetrakErrorCode::Argument,
							  "Invalid number of arguments");
	}
}

void FileInputter::execute() {
	data_->currentPos_Frames = sf_seek(file_, 0, SEEK_CUR);
	data_->currentPos_Chunks = data_->currentPos_Frames / data_->size();
	data_->currentPos_Seconds = Sample(data_->currentPos_Frames) / data_->sampleRate();
	
	std::copy(data_->full_buffer.begin() + data_->size(),
			  data_->full_buffer.begin() + data_->size() * 2,
			  data_->full_buffer.begin());
	
	readData();
	data_->melInput = data_->window_buffer;
	if ((len_ != 0.0) && (data_->currentPos_Seconds > len_))
		data_->setComplete(true);
}

FileInputter::~FileInputter() {
	if (file_)
		sf_close(file_);
}

}
