#ifndef DETECTOR_H_
#define DETECTOR_H_

#define  LIB_EXPORTS 
#define  OPENCV 

#ifndef LIB_API
#ifdef LIB_EXPORTS
#if defined(_MSC_VER)
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __attribute__((visibility("default")))
#endif
#else
#if defined(_MSC_VER)
#define LIB_API
#else
#define LIB_API
#endif
#endif
#endif

#include <memory>
#include <vector>
#include <deque>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include "Common.h"
#include "ErrorCode.h"
#include "infer.hpp"
#include "yolo.hpp"
#include "ExtractROIs.cuh"

#ifdef OPENCV
#include <opencv2/opencv.hpp>            // C++
#include <opencv2/highgui/highgui_c.h>   // C
#include <opencv2/imgproc/imgproc_c.h>   // C
#endif

extern "C" LIB_API int init(std::string type, std::string weight_filename, int gpu_id, int batch_size , float confidence_threshold, float nms_threshold);
extern "C" LIB_API int detect_mat(const unsigned char* data, const size_t data_length, bbox_t_container& container);
extern "C" LIB_API int detect_batch(const unsigned char* data, const size_t data_length, bbox_t_container& container);
extern "C" LIB_API int dispose();

extern "C" LIB_API int init_extract_rois(int img_width, int img_height, int img_channel, int roi_num, int roi_width, int roi_height, int device_id);
extern "C" LIB_API int get_split_overlap_rois(int imgWidth, int imgHeight, int startX, int startY, int roiWidth, int roiHeight, int stepX, int stepY, rois_container& container);
extern "C" LIB_API int extract_rois(int device_id,const unsigned char* buffer, int img_width, int img_height, int img_channel, rois_container roi_region,int roi_count, rois_region_container& vec_result_rois_);
extern "C" LIB_API int dispose_rois();

class Detector{
public:
	explicit Detector(std::string type, std::string weight_filename, int gpu_id = 0, int batch_size = 1, float confidence_threshold = 0.2, float nms_threshold = 0.4);
	~Detector();
    Detector(const Detector& rhs) = delete;
    Detector& operator=(const Detector& rhs) = delete;

	std::vector<bbox_t> detect(cv::Mat img);
	std::vector<std::vector<bbox_t>> detect_batch(std::vector<cv::Mat> images);

private:
	std::string type_;
	std::string weight_filename_;
	int gpu_id_;
	int batch_size_;
	float confidence_threshold_;
	float nms_threshold_;
	std::shared_ptr<yolo::Infer> yolo_;

};

#endif

