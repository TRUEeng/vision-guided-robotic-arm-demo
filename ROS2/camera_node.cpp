#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

class CameraNode : public rclcpp::Node
{
public:
    CameraNode() : Node("camera_node")
    {
        // 1. 创建发布者
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>(
            "/camera/image_raw", 10
        );

        // 2. 打开摄像头
        cap_.open("/dev/video0", cv::CAP_V4L2);
        cap_.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
        cap_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

        if (!cap_.isOpened()) {
            RCLCPP_ERROR(this->get_logger(), "无法打开摄像头");
            return;
        }

        RCLCPP_INFO(this->get_logger(), "摄像头已打开，开始发布图像");

        // 3. 创建定时器，每 33ms 触发一次（约 30fps）
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(33),
            std::bind(&CameraNode::timer_callback, this)
        );
    }

private:
    void timer_callback()
    {
        cv::Mat frame;
        cap_ >> frame;

        if (frame.empty()) {
            RCLCPP_WARN(this->get_logger(), "读取到空帧，跳过");
            return;
        }

        // 4. cv::Mat → sensor_msgs/Image
        auto msg = cv_bridge::CvImage(
            std_msgs::msg::Header(),   // 时间戳（暂时留空）
            "bgr8",                    // 像素格式：BGR 每通道 8bit
            frame
        ).toImageMsg();

        // 5. 发布
        publisher_->publish(*msg);
    }

    // 成员变量
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    cv::VideoCapture cap_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CameraNode>());
    rclcpp::shutdown();
    return 0;
}
