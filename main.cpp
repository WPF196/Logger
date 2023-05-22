#include <iostream>
#include "logger.h"

using namespace std;
using namespace melon::utility;

int main(){
    Logger::instance()->open("./test.log");
    Logger::instance()->set_Level(DEBUG);
    Logger::instance()->set_Max(1024);

    Logger::instance()->log(DEBUG, __FILE__, __LINE__, "name: %s, age: %d", "jack", 19);
    debug("hello world");
    debug("name: %s,  age: %d", "jack", 18);
    error("error!");
    info("info!");
    warn("warn!");


    Logger::instance()->close();
    return 0;
}