#ifndef TETRAK_LIVEINPUTTER_HPP
#define TETRAK_LIVEINPUTTER_HPP

#include "Tetrak/Base/TempoTrackerBase.hpp"
#include <portaudiocpp/PortAudioCpp.hxx>

namespace Tetrak {

    class LiveInputter : public InputterBase {
        protected:
            const float* inputBuffer_;
            const PaStreamCallbackTimeInfo* timeInfo_;

            void readData();

        public:
            void setPaPointers(const float* buffer, const PaStreamCallbackTimeInfo* timeInfo) {
                inputBuffer_ = buffer;
                timeInfo_ = timeInfo;
            }

            void execute();
    };

}

#endif /* TETRAK_LIVEINPUTTER_HPP */
