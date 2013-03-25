#include "Tetrak/Implementation/Input/LiveInputter.hpp"

namespace Tetrak {

    void LiveInputter::readData() {
        std::copy(inputBuffer_,
                inputBuffer_ + data_->size(),
                data_->window_buffer.begin() + data_->size());
    }

    void LiveInputter::execute() {
        data_->currentPos_Frames += data_->size();
        data_->currentPos_Seconds = Sample(data_->currentPos_Frames) / data_->sampleRate();
        data_->currentPos_Chunks = data_->currentPos_Frames / data_->size();

        std::copy(data_->full_buffer.begin() + data_->size(),
                data_->full_buffer.begin() + data_->size() * 2,
                data_->full_buffer.begin());

        readData();
        data_->melInput = data_->window_buffer;
    }

}
