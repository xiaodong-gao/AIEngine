#include "ExtractROIs.cuh"

// CUDA内核：基于多个ROI裁剪图像 (2D Kernel)
__global__ void extractRoiKernel(const unsigned char* d_image, unsigned char* d_output, roi_s* d_rois, int roi_count, int width, int height, int channels) {
    // 计算线程的全局索引
    int tx = threadIdx.x + blockIdx.x * blockDim.x; // 横向索引
    int ty = threadIdx.y + blockIdx.y * blockDim.y; // 纵向索引
    // 确保索引在有效范围内
    if (tx < roi_count) {
        // 获取当前 ROI
        roi_s roi = d_rois[tx];
        // 计算该 ROI 的位置和大小
        int roi_x = roi.x;
        int roi_y = roi.y;
        int roi_width = roi.width;
        int roi_height = roi.height;
        // 遍历 ROI 区域并提取图像数据
        for (int i = 0; i < roi_height; i++) {
            for (int j = 0; j < roi_width; j++) {
                int global_x = roi_x + j;  // 当前像素的全局 x 坐标
                int global_y = roi_y + i;  // 当前像素的全局 y 坐标
                // 检查该坐标是否在图像范围内
                if (global_x < width && global_y < height) {
                    int image_index = (global_y * width + global_x) * channels;  // 图像的索引
                    int output_index = (tx * roi_width * roi_height + i * roi_width + j) * channels;  // 输出的索引
                    // 提取每个通道的像素值
                    for (int c = 0; c < channels; c++) {
                        d_output[output_index + c] = d_image[image_index + c];
                    }
                }
            }
        }
    }
}

ExtractROIs::ExtractROIs(int img_width, int img_height, int img_channel, int roi_num,int roi_width, int roi_height, int device_id)
    : img_width_{ img_width }, 
      img_height_{ img_height },
      img_channel_{ img_channel },
      roi_num_{ roi_num },
      roi_width_{ roi_width },
      roi_height_{ roi_height },
      device_id_{device_id} {
    // 分配图像数据内存
    cudaMalloc(&d_inputImage, img_width * img_height * img_channel * sizeof(unsigned char));
    cudaMalloc(&d_outputImage, roi_num * roi_width * roi_height * img_channel * sizeof(unsigned char));  // 每个 ROI 区域大小均分
    cudaMalloc(&d_rois, roi_num * sizeof(roi_s));
}

ExtractROIs::~ExtractROIs() {
    // 释放 GPU 内存
    cudaFree(d_inputImage);
    cudaFree(d_outputImage);
    cudaFree(d_rois);
}

std::vector<roi_s> ExtractROIs::get_split_overlap_rois(int imgWidth, int imgHeight, int startX, int startY, int roiWidth, int roiHeight, int stepX, int stepY) {
    std::vector<roi_s> rois;
    // 计算 ROI 的数量
    for (int y = startY; y + roiHeight <= imgHeight; y += stepY) {
        for (int x = startX; x + roiWidth <= imgWidth; x += stepX) {
            rois.push_back(roi_s{ x,y,roiWidth ,roiHeight });
        }
    }
    return rois;
}

void ExtractROIs::extract_rois_cpu(const unsigned char* buffer, std::vector<roi_s> roi_region, std::vector<cv::Mat>& vec_result_rois_) {
    std::vector<char> vdata(buffer, buffer + img_width_ * img_height_ * img_channel_ * sizeof(unsigned char));
    cv::Mat img = imdecode(cv::Mat(vdata), cv::IMREAD_UNCHANGED);
    vec_result_rois_.clear();
    for (size_t i = 0; i < roi_num_; ++i) {
        vec_result_rois_.push_back(img(cv::Rect(roi_region[i].x, roi_region[i].y, roi_region[i].width, roi_region[i].height)));
    }
}

void ExtractROIs::extract_rois_gpu(const unsigned char* buffer, std::vector<roi_s> roi_region, std::vector<cv::Mat>& vec_result_rois_) {
    vec_result_rois_.clear();
    // 将图像数据和 ROI 数据从主机传输到 GPU
    cudaMemcpy(d_inputImage, buffer, img_width_ * img_height_ * img_channel_ * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_rois, roi_region.data(), roi_region.size() * sizeof(roi_s), cudaMemcpyHostToDevice);

    // 调用 CUDA 内核进行裁剪
    // 确保每个线程块处理一个ROI
    dim3 blockSize(32, 16);
    dim3 gridSize((roi_region.size() + blockSize.x - 1) / blockSize.x, 1);
    // 启动 CUDA 核函数
    extractRoiKernel <<<gridSize, blockSize>>> (d_inputImage, d_outputImage, d_rois, roi_region.size(), img_width_, img_height_, img_channel_);
    // 等待 CUDA 核函数执行完毕
    cudaDeviceSynchronize();

    // 检查 CUDA 内核执行是否成功
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        //std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
        //return -1;
    }

    // 从 GPU 拷贝裁剪结果回 CPU
    std::vector<uchar> outputImage(roi_region.size() * roi_width_ * roi_height_);       // 假设最大尺寸是 100x100
    cudaMemcpy(outputImage.data(), d_outputImage, roi_region.size() * roi_width_ * roi_height_ * img_channel_ * sizeof(unsigned char), cudaMemcpyDeviceToHost);

    for (int i = 0; i < roi_region.size(); ++i) {
        if(img_channel_ ==1)
            vec_result_rois_.push_back(cv::Mat(roi_region[i].height, roi_region[i].width, CV_8UC1, outputImage.data() + i * roi_region[i].width * roi_region[i].height));
        else if (img_channel_ == 3) {
            vec_result_rois_.push_back(cv::Mat(roi_region[i].height, roi_region[i].width, CV_8UC3, outputImage.data() + i * roi_region[i].width * roi_region[i].height));
        }
    }
}


/*
void ExtractROIs::output_result() {

}
*/

