#include <algorithm>

struct Stereo
{
    float left;
    float right;

    Stereo operator+(const Stereo& other) const
    {
        return {
            left + other.left,
            right + other.right
        };
    }

    Stereo operator-(const Stereo& other) const
    {
        return {
            left - other.left,
            right - other.right
        };
    }

    Stereo operator*(float gain) const
    {
        return {
            left * gain,
            right * gain
        };
    }

    Stereo operator/(float gain) const
    {
        return {
            left / gain,
            right / gain
        };
    }

    // Assignment
    Stereo& operator=(const Stereo& other)
    {
        left = other.left;
        right = other.right;
        return *this;
    }

    // Equality
    bool operator==(const Stereo& other) const
    {
        return left == other.left &&
               right == other.right;
    }

    bool operator!=(const Stereo& other) const
    {
        return !(*this == other);
    }

    // For approximation
    bool approximatelyEqual(const Stereo& other, float tolerance = 1e-6f) const
    {
        return std::abs(left - other.left) < tolerance &&
            std::abs(right - other.right) < tolerance;
    }

    float peak() const
    {
        return std::max(std::abs(left), std::abs(right));
    }

    float energy() const
    {
        return left * left + right * right;
    }

    bool operator<(const Stereo& other) const
    {
        return peak() < other.peak();
    }

    bool operator>(const Stereo& other) const
    {
        return peak() > other.peak();
    }

    bool operator<=(const Stereo& other) const
    {
        return peak() <= other.peak();
    }

    bool operator>=(const Stereo& other) const
    {
        return peak() >= other.peak();
    }
};
// TODO (Future consideration): Replace all the floats with Stereo in the audio effects