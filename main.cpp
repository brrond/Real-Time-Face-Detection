#include "RTFDApplication.hpp"

int main(int argc, char** argv) {
    RTFDApplication app(argc, const_cast<const char**>(argv));
    return app.run();
}
