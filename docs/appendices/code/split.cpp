template <size_t NumChannels>
inline std::array<float, NumChannels> split(float input)
{
    std::array<float, NumChannels> output{};
    output.fill(input);

    return output;
}