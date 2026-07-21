// TODO: Use the RAII approach to measure time
// Create a function which takes a function as an argument and retrieves the execution time
#include <iostream>
#include <vector>
#include <chrono>

class Timer
{
public:
    explicit Timer(std::vector<std::chrono::duration<double, std::milli>>& durations)
        : m_start(std::chrono::steady_clock::now()), m_durations(durations)
    {
    }

    ~Timer()
    {
        const auto end = std::chrono::steady_clock::now();
        const auto elapsed =
            std::chrono::duration<double, std::milli>(end - m_start);
        
        m_durations.push_back(elapsed);
    }

private:
    std::chrono::steady_clock::time_point m_start;
    std::vector<std::chrono::duration<double, std::milli>>& m_durations;
};

int main() {


    
}