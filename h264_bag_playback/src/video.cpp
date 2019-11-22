#include <video.hpp>



void Video::InitialiseCameraInfo(sensor_msgs::CameraInfo &camera_info) {

  camera_info_msg = camera_info;

  uint32_t image_width = camera_info_msg.width;
  uint32_t image_height = camera_info_msg.height;

  cv::Size image_size = cv::Size(image_width, image_height);

  if (camera_info_msg.distortion_model == "rational_polynomial") {
    camera_matrix = cv::Mat(3, 3, CV_64F, &camera_info_msg.K[0]);
    distance_coeffs = cv::Mat(4, 1, CV_64F, &camera_info_msg.D[0]);
  }
  else if (camera_info_msg.distortion_model == "equidistant") {
    camera_matrix = cv::Mat(3, 3, CV_64F, &camera_info_msg.K[0]);
    distance_coeffs = cv::Mat(4, 1, CV_64F, &camera_info_msg.D[0]);

    //cv::Mat scaled_camera_matrix = camera_matrix *
    camera_matrix.at<double>(2, 2) = 1.;

    //cv::Mat output_image;
    cv::Mat identity_mat = cv::Mat::eye(3, 3, CV_64F);

    cv::Mat modified_camera_matrix;
    cv::fisheye::estimateNewCameraMatrixForUndistortRectify(camera_matrix, distance_coeffs, image_size,
                                                            identity_mat, modified_camera_matrix);

    cv::fisheye::initUndistortRectifyMap(camera_matrix,
                                         distance_coeffs,
                                         identity_mat,
                                         modified_camera_matrix,
                                         image_size,
                                         CV_16SC2,
                                         map1, map2);
  }

  valid_camera_info = true;
}


bool Video::InitialiseVideo(std::string camera_name, std::string video_filename) {

  ros::NodeHandle private_nh("~");
  ros::NodeHandle public_nh;

  image_transport::ImageTransport image_transport(public_nh);

  valid_camera_info = false;
  file_name = video_filename;
  video_device = cv::VideoCapture(file_name);
  frame_counter = 0;
  std::string topic_prefix = "/gmsl/";
  topic_prefix += camera_name;

  std::string uncorrected_topic_name = topic_prefix;
  std::string corrected_topic_name = topic_prefix;

  uncorrected_topic_name += "/image_color";
  corrected_topic_name += "/image_rect_color";

  uncorrected_publisher = image_transport.advertise(uncorrected_topic_name, 1);
  corrected_publisher = image_transport.advertise(corrected_topic_name, 1);

  if(!video_device.isOpened()) { // check if we succeeded
    //ROS_INFO_STREAM("could not open video file: " << file_name << " called " << camera_name);
    return false;
  }

  return true;
}

