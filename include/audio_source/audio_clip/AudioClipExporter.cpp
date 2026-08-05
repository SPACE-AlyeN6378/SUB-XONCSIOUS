#include "audio_source/audio_clip/AudioClipExporter.hpp"
#include <sndfile.h>
#include <stdexcept>

void AudioExporter::exportWav(
    const std::string& filename,
    const std::vector<float>& samples,
    unsigned int channels,
    double sampleRate)
{
    SF_INFO info{};
    info.channels = channels;
    info.samplerate = static_cast<int>(sampleRate);
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* file = sf_open(filename.c_str(), SFM_WRITE, &info);

    if (!file)
        throw std::runtime_error("Failed to create WAV file");

    sf_count_t written = sf_writef_float(
        file,
        samples.data(),
        samples.size() / channels
    );

    if (written <= 0)
    {
        sf_close(file);
        throw std::runtime_error("Failed to write WAV data");
    }

    sf_close(file);
}