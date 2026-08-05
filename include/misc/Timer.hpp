#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

namespace TestUtils {
class Timer
{
public:
    Timer(std::vector<double>& vec)
        : measurements(vec),
            start(std::chrono::high_resolution_clock::now())
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed = 
            std::chrono::duration<double, std::micro>(end - start).count();

        measurements.push_back(elapsed);
    }


private:
    std::vector<double>& measurements;
    std::chrono::high_resolution_clock::time_point start;
};

}   // namespace TestUtils

#endif     // TIMER_HPP