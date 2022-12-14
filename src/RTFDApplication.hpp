/*****************************************************************//**
 * \file   RTFDApplication.hpp
 * \brief  Real-time face detection application
 * 
 * \author greentech72
 * \date   August 2022
 *********************************************************************/

#pragma once

#include "../include/RTFD.h"

class RTFDApplication {
public:
    RTFDApplication(int argc, const char** argv) : argc(argc), argv(argv) {}

    /**
     * Runs Real-time face detection application.
     * 
     * \return execution result code
     */
    int run();

private:
    const cv::String getKeys() { return "{help h usage ? | | Print this help message. After application start you can use several keys to manipulate the program:\n\t\t'G' - go to grayscale mode\n\t\t\t'T' - go to threshold mode\n\t\t\t\t'+'/'-' - increase/decrease threshold value\n\t\t'M' - enable/disable motion filter}"; }
    int mainloop();

    void print_help_before_loop();

    void perform_additional_operations(cv::Mat& frame);
    void handle_input(int key);

private:

    const int argc;
    const char** argv;

    /**
     * Flags for per-frame operations.
     */
    bool mGrayscale = false;
    bool mThreshold = false;
    bool mMotion = false;
    cv::Mat* mPrevFrame = nullptr;
    int mThresholdValue = 128;
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

void RTFDApplication::print_help_before_loop() {
    std::cout << "Start real-time face detection" << std::endl
        << "Press 'Q' to exit" << std::endl
        << "Press 'G' to enable/disable grayscale mode" << std::endl
        << "\tPress 'T' to enable/disable threshold" << std::endl
        << "\t\tPres '+'/'-' to increase/decrease threshold value" << std::endl
        << "Press 'M' to enable/disable motion" << std::endl;
}

int RTFDApplication::mainloop() {
    cv::Mat frame; // init frame
    cv::VideoCapture camera; // capture camera
    cv::CascadeClassifier face_cascade("resources/haarcascade_frontalface_default.xml"); // load face cascade

    for (int i = 0; i < CAMERA_INDICES_TO_TRY; i++) {
        camera.open(i, cv::CAP_ANY); // try to open camera

        // if problem with camera
        if (!camera.isOpened()) {
            std::cerr << "Error: unable to open camera #" << i << std::endl;
            if (i == CAMERA_INDICES_TO_TRY - 1) {
                std::cout << "Exiting..." << std::endl;
                exit(-1);
            }
        }
        else break;
    }

    print_help_before_loop();

    // first time point
    auto f = std::chrono::high_resolution_clock::now();

    do {
        camera.read(frame); // read next frame

        if (frame.empty()) {
            std::cerr << "Error: blank frame grabbed\n";
            break;
        }

        perform_additional_operations(frame);

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

        handle_input(cv::waitKey(5));
    } while (true);

    return 0;
}

void RTFDApplication::perform_additional_operations(cv::Mat& frame) {
    if (mGrayscale) { 
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        if (mThreshold) {
            for (int i = 0; i < frame.rows; i++) {
                for (int j = 0; j < frame.cols; j++) {
                    uint8_t val = frame.at<uint8_t>(i, j);
                    frame.at<uint8_t>(i, j) = (val > mThresholdValue) ? 255 : 0;
                }
            }
        }
        else if (mMotion) {
            cv::Mat* tmp = new cv::Mat(frame.clone());
            if (mPrevFrame != nullptr) {
                for (int i = 0; i < frame.rows; i++) {
                    for (int j = 0; j < frame.cols; j++) {
                        // current frame - prev frame
                        frame.at<uint8_t>(i, j) = abs(frame.at<uint8_t>(i, j) - mPrevFrame->at<uint8_t>(i, j));
                    }
                }
                delete mPrevFrame;
            }
            mPrevFrame = tmp;
        }
        else if (!mMotion) {
            delete mPrevFrame;
            mPrevFrame = nullptr;
        }
    }
}

void RTFDApplication::handle_input(int key) {
    if (key == 'q') {
        std::cout << "Exiting..." << std::endl;
        exit(0);
    }
    else if (key == 'g') {
        mGrayscale = !mGrayscale;
        std::cout << "Grayscale mode is: " << (mGrayscale ? "enabled" : "disabled") << std::endl;
    }
    else if (key == 't' && mGrayscale && !mMotion) {
        mThreshold = !mThreshold;
        std::cout << "Threshold mode is: " << (mThreshold? "enabled" : "disabled") << std::endl;
    }
    else if (key == 'm' && mGrayscale && !mThreshold) {
        mMotion = !mMotion;
        std::cout << "Motion mode is: " << (mMotion ? "enabled" : "disabled") << std::endl;
    }
    else if (key == '+') {
        mThresholdValue += 5;
        if (mThresholdValue > 255) mThresholdValue = 255;
        std::cout << "Threshold value is: " << mThresholdValue << std::endl;
    }
    else if (key == '-') {
        mThresholdValue -= 5;
        if (mThresholdValue < 0) mThresholdValue = 0;
        std::cout << "Threshold value is: " << mThresholdValue << std::endl;
    }
}
