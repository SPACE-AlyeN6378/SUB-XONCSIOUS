#ifndef HADAMARD_HPP
#define HADAMARD_HPP

#include <array>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

using ArraySignal4 = std::array<float, 4>;
using ArraySignal8 = std::array<float, 8>;

// <*hadamard>
namespace Hadamard {

    inline ArraySignal4 transform4(ArraySignal4 input)
    {
        return
        {
            0.5f * (input[0] + input[1] + input[2] + input[3]),
            0.5f * (input[0] - input[1] + input[2] - input[3]),
            0.5f * (input[0] + input[1] - input[2] - input[3]),
            0.5f * (input[0] - input[1] - input[2] + input[3])
        };
    }

    inline ArraySignal8 transform8(ArraySignal8 input)
    {
        return
        {
            0.353553f * (input[0] + input[1] + input[2] + input[3] + input[4] + input[5] + input[6] + input[7]),
            0.353553f * (input[0] - input[1] + input[2] - input[3] + input[4] - input[5] + input[6] - input[7]),
            0.353553f * (input[0] + input[1] - input[2] - input[3] + input[4] + input[5] - input[6] - input[7]),
            0.353553f * (input[0] - input[1] - input[2] + input[3] + input[4] - input[5] - input[6] + input[7]),
            0.353553f * (input[0] + input[1] + input[2] + input[3] - input[4] - input[5] - input[6] - input[7]),
            0.353553f * (input[0] - input[1] + input[2] - input[3] - input[4] + input[5] - input[6] + input[7]),
            0.353553f * (input[0] + input[1] - input[2] - input[3] - input[4] - input[5] + input[6] + input[7]),
            0.353553f * (input[0] - input[1] - input[2] + input[3] - input[4] + input[5] + input[6] - input[7])
        };
    }

    template<typename T>
    T transform(T input)
    {
        if constexpr (std::is_same_v<T, ArraySignal4>)
            return transform4(input);
        else if constexpr (std::is_same_v<T, ArraySignal8>)
            return transform8(input);
        else
            static_assert(std::is_same_v<T, ArraySignal4> || std::is_same_v<T, ArraySignal8>, "Unsupported signal size");
    }
}
// </hadamard>

// <*householder>
namespace Householder {

    inline ArraySignal4 transform4(ArraySignal4 input)
    {
        float sum =
            input[0] + input[1] + input[2] + input[3];

        constexpr float scale = 0.5f; // 2/N

        float correction = scale * sum;

        return
        {
            input[0] - correction,
            input[1] - correction,
            input[2] - correction,
            input[3] - correction
        };
    }

    inline ArraySignal8 transform8(ArraySignal8 input)
    {
        float sum =
            input[0] + input[1] + input[2] + input[3] +
            input[4] + input[5] + input[6] + input[7];

        constexpr float scale = 0.25f; // 2/N

        float correction = scale * sum;

        return
        {
            input[0] - correction,
            input[1] - correction,
            input[2] - correction,
            input[3] - correction,
            input[4] - correction,
            input[5] - correction,
            input[6] - correction,
            input[7] - correction
        };
    }

    template<typename T>
    inline T transform(T input)
    {
        if constexpr (std::is_same_v<T, ArraySignal4>)
            return transform4(input);
        else if constexpr (std::is_same_v<T, ArraySignal8>)
            return transform8(input);
        else
            static_assert(std::is_same_v<T, ArraySignal4> || std::is_same_v<T, ArraySignal8>, "Unsupported signal size");
    }    
}
// </householder>

#endif  // HADAMARD_HPP