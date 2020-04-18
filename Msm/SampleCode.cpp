#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>

int main()
{
    std::vector<int> vi{1,2,3,4,5};
    int i = vi[6];
    std::cout << "i = " << i << std::endl;
    return 0;
}