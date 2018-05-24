// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include "example.hpp"          // Include short list of convenience functions for rendering
#include <list>
// Capture Example demonstrates how to
// capture depth and color video streams and render them to the screen


void emergency_response() {

}
void fall_detected(std::vector<rs2::depth_frame>& frames_v, int seg_count) {
	//Unpack data
	std::vector<std::vector<float>> depth_data; //switch to numpy compatable array
	for (int k = 0; k < frames_v.size; k++) {
		auto f = frames_v[k];
		auto type = f.get_bytes_per_pixel();
		const short *memory_start = (short*)f.get_data();  // Use short to get single byte array
		//Stride is frame width. 2 bytes per pixel
		int width = f.get_stride_in_bytes() / f.get_bytes_per_pixel(); // gets pixel width
		int height = f.get_height(); //gets pixel height
		//TODO consider just using f.get_distance(i,j)
		const short *memory_end = memory_start + width*height;
		for (int i = 0; i < height; i++) {
			short next_line = *memory_start + i*sizeof(short)*width;
			for (short j = next_line; j < next_line + (int)sizeof(short)*width; i += sizeof(short)) {
				float depth = (float)j;
				depth_data[k].push_back(depth);
			}
		}
	}
	//Pass to fall detction model in python
	bool detected = true;
	if (detected) {
		emergency_response();
	}

	return;
}

int main(int argc, char * argv[]) try
{

    rs2::pipeline pipe;
    pipe.start();
	bool running = true;
	int i = 0;
	int seg_count = 100;
	std::vector<rs2::depth_frame> frames_v;

    while(running) 
    {
		if (i == 0) {
			//TODO lock threads
			std::thread detector{ fall_detected(frames_v, seg_count) };
		}
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		rs2::depth_frame depth_frame = data.get_depth_frame();
		frames_v.push_back(depth_frame); //Copy frames
		i++;
		if (i%seg_count == 0) {
			frames_v.clear();
			i = 0;

		}
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}



