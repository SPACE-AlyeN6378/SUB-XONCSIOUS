#ifndef AUDIO_SOURCE_HPP
#define AUDIO_SOURCE_HPP

class AudioSource {

public:

    virtual ~AudioSource() = default;
    virtual float generate() = 0;
    virtual void reset() = 0;

protected:
    // unsigned long framesPerBuffer;
    double engineSampleRate;
};
    
#endif // AUDIO_SOURCE_HPP
