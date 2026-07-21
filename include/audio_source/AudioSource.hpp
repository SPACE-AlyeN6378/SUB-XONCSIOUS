#ifndef AUDIO_SOURCE_HPP
#define AUDIO_SOURCE_HPP

class AudioSource {

public:

    virtual ~AudioSource() = default;
    virtual float generate(std::size_t channel) = 0;

protected:
    unsigned long framesPerBuffer;
    double sampleRate;
};
    
#endif // AUDIO_SOURCE_HPP
