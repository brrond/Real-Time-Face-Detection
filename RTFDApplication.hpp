#pragma once

#include "RTFD.h"

// Class of Real-Time-Face-Detection application
class RTFDApplication {
public:
    RTFDApplication(int argc, const char** argv) : argc(argc), argv(argv) {}

    int run();

private:
    const cv::String getKeys() { return "{help h usage ? | | Print this help message. After application start you can use several keys to manipulate the program:\n\t\t'G' - go to grayscale mode\n\t\t\t'T' - go to threshold mode\n\t\t\t\t'+'/'-' - increase/decrease threshold value}"; }
    int mainloop();

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
    return mainloop();
}

int RTFDApplication::mainloop() {
    cv::Mat frame; // init frame
    cv::VideoCapture camera(1); // capture camera
    cv::CascadeClassifier face_cascade("haarcascade_frontalface_default.xml"); // load face cascade

    camera.open(1, cv::CAP_ANY); // open camera

    // if problem with camera
    if (!camera.isOpened()) {
        std::cerr << "Error: unable to open camera\n";
        return -1;
    }

    std::cout << "Start real-time face detection" << std::endl
        << "Press 'Q' to exit" << std::endl;

    // first time point
    auto f = std::chrono::high_resolution_clock::now();

    do {
        camera.read(frame); // read next frame

        if (frame.empty()) {
            std::cerr << "Error: blank frame grabbed\n";
            break;
        }

        std::vector<cv::Rect> faces; // array of faces
        try {
            face_cascade.detectMultiScale(frame, faces, 1.5, 7, 0, cv::Size(30, 30)); // detect
        }
        catch (std::exception e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }

        std::for_each(faces.begin(), faces.end(), [&frame](cv::Rect r) {
            cv::ellipse(frame, cv::Point(r.x + r.width / 2, r.y + r.height / 2),
                cv::Size(r.width / 2, r.height / 2),
                0, 0, 360,
                cv::Scalar(255, 0, 255, 255), 2);
            cv::rectangle(frame, r, cv::Scalar(0, 255, 0, 255));
            }); // print all faces with box and ellipse around face

        // calculate fps
        auto s = std::chrono::high_resolution_clock::now();
        auto time_per_frame = std::chrono::duration_cast<std::chrono::milliseconds>((s - f)).count();
        auto fps = 1. / time_per_frame * 1000.; // fps
        f = s;

        // showw fps
        cv::putText(frame, "TPF : " + std::to_string(time_per_frame) + "ms. FPS : " + std::to_string((int)fps), cv::Point(10, 30), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 0, 0));
        imshow("output", frame); // show frame

        int ch = cv::waitKey(5);
        if (ch == 'q') {
            std::cout << "Exiting..." << std::endl;
            break;
        }
            
    } while (true);

    return 0;
}
