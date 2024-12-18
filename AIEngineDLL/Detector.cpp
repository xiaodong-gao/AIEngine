#include "Detector.h"

//static Detector* detector = NULL;
static std::unique_ptr<Detector> detector;
static std::unique_ptr<ExtractROIs> extractor;

int init_extract_rois(int img_width, int img_height, int img_channel, int roi_num, int roi_width, int roi_height, int device_id) {
	extractor.reset(new ExtractROIs(img_width, img_height, img_channel, roi_num, roi_width, roi_height, device_id));
	return 1;
}
int get_split_overlap_rois(int imgWidth, int imgHeight, int startX, int startY, int roiWidth, int roiHeight, int stepX, int stepY, rois_container& container) {
	std::vector<roi_s> rois = extractor->get_split_overlap_rois(imgWidth, imgHeight, startX, startY, roiWidth, roiHeight, stepX, stepY);
	for (size_t i = 0; i < rois.size() && i < C_SHARP_MAX_OBJECTS; ++i)
		container.candidates[i] = rois[i];
	return rois.size();
}

int extract_rois(int device_id, const unsigned char* buffer, rois_container roi_region, int roi_count, rois_region_container& vec_result_rois) {
	std::vector<roi_s> roi_region_;
	for (int i = 0; i < roi_count; i++) {
		roi_region_.push_back(roi_region.candidates[i]);
	}
	std::vector<cv::Mat> vec_result_rois_;
	if (device_id == -1) {
		extractor->extract_rois_cpu(buffer, roi_region_, vec_result_rois_);
		for (int i = 0; i < vec_result_rois_.size(); i++) {
			vec_result_rois.candidates[i].data = (vec_result_rois_[i]).data;
			vec_result_rois.candidates[i].w = (vec_result_rois_[i]).cols;
			vec_result_rois.candidates[i].h = (vec_result_rois_[i]).rows;
			vec_result_rois.candidates[i].c = (vec_result_rois_[i]).channels();
			roi_region_.push_back(roi_region.candidates[i]);
		}
		return vec_result_rois_.size();
	}
	else {
		extractor->extract_rois_gpu(buffer, roi_region_, vec_result_rois_);
		for (int i = 0; i < vec_result_rois_.size(); i++) {
			vec_result_rois.candidates[i].data = (vec_result_rois_[i]).data;
			vec_result_rois.candidates[i].w = (vec_result_rois_[i]).cols;
			vec_result_rois.candidates[i].h = (vec_result_rois_[i]).rows;
			vec_result_rois.candidates[i].c = (vec_result_rois_[i]).channels();
			roi_region_.push_back(roi_region.candidates[i]);
		}
		return vec_result_rois_.size();
	}
}


int dispose_rois() {
	extractor.reset();
	return 1;
}

int init(std::string type, std::string weight_filename, int gpu_id, int batch_size, float confidence_threshold, float nms_threshold) {
	detector.reset(new Detector(type, weight_filename, gpu_id, batch_size, confidence_threshold, nms_threshold));
	return 1;
}

int detect_mat(const unsigned char* data, const size_t data_length, bbox_t_container& container) {
#ifdef OPENCV
	std::vector<char> vdata(data, data + data_length);
	cv::Mat image = imdecode(cv::Mat(vdata), 1);

	std::vector<bbox_t> detection = detector->detect(image);
	for (size_t i = 0; i < detection.size() && i < C_SHARP_MAX_OBJECTS; ++i)
		container.candidates[i] = detection[i];
	return detection.size();
#else
	return -1;
#endif    // OPENCV
}

int detect_batch(const unsigned char* data, const size_t data_length, bbox_t_container& container) {
#ifdef OPENCV
	std::vector<char> vdata(data, data + data_length);
	cv::Mat image = imdecode(cv::Mat(vdata), 1);

	std::vector<bbox_t> detection = detector->detect(image);
	for (size_t i = 0; i < detection.size() && i < C_SHARP_MAX_OBJECTS; ++i)
		container.candidates[i] = detection[i];
	return detection.size();
#else
	return -1;
#endif    // OPENCV
}

int dispose() {
	//if (detector != NULL) delete detector;
   //detector = NULL;
	detector.reset();
	return 1;
}


Detector::Detector(std::string type, std::string weight_filename, int device_id , int batch_size, float confidence_threshold, float nms_threshold)
	:type_{ type }, 
	 weight_filename_{ weight_filename }, 
	 gpu_id_{ device_id }, 
	 batch_size_{ batch_size } ,
	 confidence_threshold_{ confidence_threshold} ,
	 nms_threshold_{ nms_threshold}
{
	yolo::Type yolo_type;
	if (type == "v5") {
		yolo_type = yolo::Type::V5;
	}else if (type == "x") {
		yolo_type = yolo::Type::X;
	}else if (type == "v3") {
		yolo_type = yolo::Type::V3;
	}else if (type == "v7") {
		yolo_type = yolo::Type::V7;
	}else if (type == "v8" || type == "v11") {
		yolo_type = yolo::Type::V8;
	}else if (type == "v8seg" || type == "v11seg") {
		yolo_type = yolo::Type::V8Seg;
	}
	yolo_ = yolo::load(weight_filename.c_str(), yolo_type, confidence_threshold, nms_threshold);
	if (yolo_ == nullptr)
		return;
}

Detector::~Detector() {
	yolo_.reset();
}


std::vector<bbox_t> Detector::detect(cv::Mat img) {
	yolo::BoxArray objs = yolo_->forward(yolo::Image(img.data, img.cols, img.rows) );
	std::vector<bbox_t> vec_bbox;
	for (auto& obj : objs) {
		bbox_t box;
		box.x = obj.left;
		box.y = obj.top;
		box.w = obj.right - obj.left;
		box.h = obj.bottom - obj.top;
		vec_bbox.push_back(box);
	}
	return vec_bbox;
}

std::vector<std::vector<bbox_t>> Detector::detect_batch(std::vector<cv::Mat> images) {
	std::vector<yolo::Image> yoloimages(images.size());
	std::transform(images.begin(), images.end(), yoloimages.begin(), [](const cv::Mat& image) -> yolo::Image {
		return yolo::Image(image.data, image.cols, image.rows);
		});
	auto batched_result = yolo_->forwards(yoloimages);

	std::vector<std::vector<bbox_t>> vec_vec_bbox;
	for (int ib = 0; ib < (int)batched_result.size(); ++ib) {
		auto& objs = batched_result[ib];
		std::vector<bbox_t> vec_bbox;
		for (auto& obj : objs) {
			bbox_t box;
			box.x = obj.left;
			box.y = obj.top;
			box.w = obj.right - obj.left;
			box.h = obj.bottom - obj.top;
			vec_bbox.push_back(box);
		}
		vec_vec_bbox.push_back(vec_bbox);
	}
	return   vec_vec_bbox;
}

