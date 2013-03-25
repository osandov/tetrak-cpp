#include "Tetrak/Implementation/TempoFinder.hpp" 

namespace Tetrak {

void TempoFinder::execute() {
	if ((data_->eventFound && data_->events.size() == params_->eventCount)
		|| data_->isComplete())
	{
		auto begin = data_->events.begin(), end = data_->events.end();
		
		Tempo tempo = 0;
		Sample maxResonance = 0;
		Sample interval = data_->events.back().pos - data_->events.front().pos;
		
		for (Tempo t = params_->minTempo; t <= params_->maxTempo; ++t) {
			Resonator f(params_->resonatorImpulseWidth, 60.0 / t, params_->resonatorPenalty);
			
			Sample resonance = 0;
			
			auto offset = begin;
			while (offset != end) {
				f.setOffset(offset->pos);
				resonance = std::max(resonance, f.sum(begin, end) * f.scoreFactor(interval));
				++offset;
			}
			
			if (resonance > maxResonance) {
				tempo = t;
				maxResonance = resonance;
			}
			
			//~ data_->tempo_buffer[t] = resonance * f.scoreFactor(interval);
			data_->tempo_buffer[t] = resonance;
		}
		
		data_->rangeSmooth.exec(data_->tempo_buffer.begin() + params_->minTempo,
									data_->tempo_buffer.begin() + params_->maxTempo + 1);
		
		// Add the tempo to the list, but there's no need to be
		// redundant if the previous tempo was the same
		//~ if (data_->tempi.size() == 0) {
			//~ data_->tempi.push_back(TempoResult{data_->events.front().pos, tempo});
		//~ } else if (tempo != data_->tempi.back().tempo) {
			//~ data_->tempi.push_back(TempoResult{data_->events.front().pos - 1.0 / 100.0,
								   //~ data_->tempi.back().tempo});
			//~ data_->tempi.push_back(TempoResult{data_->events.front().pos, tempo});
		//~ }
		data_->tempi.push_back(TempoResult{data_->events.front().pos, tempo});		
	}
}

}
