#include <iostream>

int main()
{
    double detuneCents = 10.0;
    int num = 6;

    for (int i = 0; i < num; ++i)
    {
        double center = (num - 1) / 2.0;
    
        std::cout << i - center << " ";
    }
    std::cout << std::endl;

    return 0;
}