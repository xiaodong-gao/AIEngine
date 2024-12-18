#ifndef COMMON_H_
#define COMMON_H_

#define C_SHARP_MAX_OBJECTS 1000

struct bbox_t {
    unsigned int x, y, w, h;       // (x,y) - top-left corner, (w, h) - width & height of bounded box
    float prob;                    // confidence - probability that the object was found correctly
    unsigned int obj_id;           // class of object - from range [0, classes-1]
    //unsigned int track_id;         // tracking id for video (0 - untracked, 1 - inf - tracked object)
    //unsigned int frames_counter;   // counter of frames on which the object was detected
    //float x_3d, y_3d, z_3d;        // center of object (in Meters) if ZED 3D Camera is used
};

struct roi_s {
    int x, y, width, height;       // (x,y) - top-left corner, (w, h) - width & height of bounded box
    roi_s(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height) {
    }
};

struct image_t {
    int h;                        // height
    int w;                        // width
    int c;                        // number of chanels (3 - for RGB)
    unsigned char* data;                  // pointer to the image data
};

struct bbox_t_container {
    bbox_t candidates[C_SHARP_MAX_OBJECTS];
};

struct rois_container {
    roi_s candidates[C_SHARP_MAX_OBJECTS];
};

struct rois_region_container {
    image_t candidates[C_SHARP_MAX_OBJECTS];
};



#endif
