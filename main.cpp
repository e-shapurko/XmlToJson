#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include "Convertor/convertor.h"


int main() {
    Convertor c(3);
    std::vector<std::string> v{".//input.xml", ".//input.xml", ".//input.xml", ".//input.xml", ".//input.xml", ".//input.xml", ".//input.xml", ".//input.xml", ".//input.xml"};
    c.run(v);
//    std::cout << "start" << std::endl;
//    std::string path = ".//input.xml";
    return 0;
}
