inline std::mt19937 rng{std::random_device{}()};

template<size_t NumChannels>
inline std::array<size_t, NumChannels> generatePermutation()
{
    std::array<size_t, NumChannels> permutation{};
    std::iota(permutation.begin(), permutation.end(), 0u);
    std::shuffle(permutation.begin(), permutation.end(), rng);
    return permutation;
}