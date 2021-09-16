#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include "Queue/queue.h"
#include "Convertor/convertor.h"


Convertor c(3);

void readXmlFile(std::string path){
    for (int i = 0; i < 100; i++){
        c.readXml(path);
        sleep(1);
    }
}

void writeJsonFile(){
    for (int i = 0; i < 100; i++){
        c.writeJson();
        sleep(2);
    }
}

int main() {
    std::cout << "start" << std::endl;
    std::string path = ".//input.xml";
    std::thread readThr(readXmlFile, path);
    std::thread writeThr(writeJsonFile);
    readThr.join();
    writeThr.join();
    return 0;
}
