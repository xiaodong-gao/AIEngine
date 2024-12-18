#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "Common.h"
#include <opencv2/opencv.hpp>


extern __global__ void extractRoiKernel(const unsigned char* d_image, unsigned char* d_output, roi_s* d_rois, int roi_count, int width, int height, int channels);

class ExtractROIs {
public:
	explicit ExtractROIs(int img_width, int img_height, int img_channel, int roi_num, int roi_width, int roi_height, int device_id);
	~ExtractROIs();
	
	ExtractROIs(const ExtractROIs& rhs) = delete;
	ExtractROIs& operator=(const ExtractROIs& rhs) = delete;

	std::vector<roi_s> get_split_overlap_rois(int imgWidth, int imgHeight, int startX, int startY, int roiWidth, int roiHeight, int stepX, int stepY);
	void extract_rois_cpu(const unsigned char* buffer, std::vector<roi_s> roi_region,std::vector<cv::Mat> &vec_result_rois_);
	void extract_rois_gpu(const unsigned char* buffer, std::vector<roi_s> roi_region, std::vector<cv::Mat>& vec_result_rois_);

private:
	unsigned char* d_inputImage;
	unsigned char* d_outputImage;
	roi_s* d_rois;

	int img_width_;
	int img_height_;
	int img_channel_;
	int roi_num_;
	int roi_width_;
	int roi_height_;
	int device_id_;



};