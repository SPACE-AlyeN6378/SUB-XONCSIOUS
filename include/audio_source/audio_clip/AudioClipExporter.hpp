#include <string>
#include <vector>

class AudioExporter
{
public:
    static void exportWav(
        const std::string& filename,
        const std::vector<float>& samples,
        unsigned int channels,
        double sampleRate
    );
};