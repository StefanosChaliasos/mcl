#include <emscripten/bind.h>
#include <iostream>

using namespace emscripten;

class MyClass {
private:
    std::string arr[3] {"foo", "bar", "foobar"};
    int n {3};
    
public:
    MyClass() {}

    std::string getString(int n) const {
        return arr[n];
    }

};


std::string example_array[3] = {"foo", "bar", "foobar"};

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}


EMSCRIPTEN_BINDINGS(my_module) {
    class_<MyClass>("MyClass")
        .constructor()
        .function("getString", &MyClass::getString)
        ;
}


