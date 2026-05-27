/*
2026——05——27
LOC：WSL-Ubuntu2204,
::作为WSL内测试OpenCV连接摄像头的程序。

*/

#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::VideoCapture cap;

    cap.open(0,cv::CAP_V4L2);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Open Camera failed." << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y','U','Y','V'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    std::cout << "Opening Camera Succesfully,press 'Q' exits." << std::endl;

    cv:: Mat frame;
    


    while (true)
    {
        cap >> frame;
        if (frame.empty()){
            std::cerr << "错误：读取到空帧" << std::endl;
            break;
        }
        cv::imshow("Camera:",frame);

        int key = cv:: waitKey(1);
        if (key == 'q' || key == 27){break;}
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
