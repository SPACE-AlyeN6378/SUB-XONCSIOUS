inline std::mt19937 rng{std::random_device{}()};

template<size_t NumChannels>
inline std::array<float, NumChannels> generatePolarity()
{
    std::array<float, NumChannels> polarity;

    std::uniform_int_distribution<int> pdist(0, 1);
    for (int i=0; i<NumChannels; ++i)
    {
        polarity[i] = pdist(rng) ? 1.0f : -1.0f;
    }

    return polarity;
}