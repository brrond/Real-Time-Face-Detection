#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

// Class of Real-Time-Face-Detection application
class RTFDApplication {
public:
    RTFDApplication(int argc, const char** argv) : argc(argc), argv(argv) {}

    int run();

private:
    const cv::String getKeys() { return "{help h usage ? | | Print this help message. After application start you can use several keys to manipulate the program:\n\t\t'G' - go to grayscale mode\n\t\t\t'T' - go to threshold mode\n\t\t\t\t'+'/'-' - increase/decrease threshold value}"; }

private:
    const int argc;
    const char** argv;
};

int RTFDApplication::run() {
    cv::CommandLineParser parser(argc, argv, getKeys()); // init arg parser
    parser.about("Real-time face detecction application by greentech72"); // add about msg

    // print help
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // start application

    return 0;
}
