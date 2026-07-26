template <size_t NumChannels>
inline float join(const std::array<float, NumChannels>& outputs)
{
    float sum = 0.0f;
    for (int i=0; i<NumChannels; ++i)
    {
        sum += outputs[i];
    }
    return sum;
}