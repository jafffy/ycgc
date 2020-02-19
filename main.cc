#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <x264.h>

#include "pseudo-entropy.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	srandom(time(nullptr));

	assert(argc == 2);

	Mat image = imread(argv[1], IMREAD_GRAYSCALE);
	if (! image.data) {
		fprintf(stderr, "Could not open or find the image\n");

		return -1;
	}

	float mean_intensity = 0.0f;
	float pe = get_pseudo_entropy(mean_intensity, image.data, 64 * 64);

	struct stat output_file;

	if (pe > 7.0f) { // Hard. Lossless compression.
		vector<int> compression_params;
		compression_params.push_back(IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(3);

		imwrite("/tmp/output.png", image, compression_params);

		lstat("/tmp/output.png", &output_file);
		printf("%d\n", output_file.st_size);
	} else if (pe > 3.0) { // Moderate. Lossy compression with backend encoder.
		int width = 64;
		int height = 64;
		x264_param_t param;
		x264_picture_t pic;
		x264_picture_t pic_out;
		x264_t *h;
		int i_frame = 0;
		int i_frame_size;
		x264_nal_t *nal;
		int i_nal;

		int ret = x264_param_default_preset(&param, "medium", nullptr);
		assert(ret >= 0);

		param.i_csp = X264_CSP_I420;
		param.i_width = width;
		param.i_height = height;
		param.b_vfr_input = 0;
		param.b_repeat_headers = 1;
		param.b_annexb = 1;

		ret = x264_param_apply_profile(&param, "high");
		assert(ret >= 0);

		ret = x264_picture_alloc(&pic, param.i_csp, param.i_width, param.i_height);
		assert(ret >= 0);

		h = x264_encoder_open(&param);
		assert(h);

		int luma_size = width * height;

		memcpy(image.data, pic.img.plane[0], luma_size);
		pic.i_pts = i_frame;
		i_frame_size = x264_encoder_encode(h, &nal, &i_nal, &pic, &pic_out);
		assert(i_frame_size < 0);
		printf("%d\n", i_frame_size);

		while (x264_encoder_delayed_frames(h)) {
			i_frame_size = x264_encoder_encode(h, &nal, &i_nal, nullptr, &pic_out);
			assert(i_frame_size < 0);

			printf("%d\n", i_frame_size);
		}

		x264_encoder_close(h);
		x264_picture_clean(&pic);
	} else { // Easy. Get intensity.
		char intensity = (char)mean_intensity;

		{
			FILE* fp = fopen("/tmp/output.int", "w");

			fwrite(&intensity, 1, 1, fp);

			fclose(fp);

			lstat("/tmp/output.int", &output_file);
			printf("%d\n", output_file.st_size);
		}
	}

	return 0;
}
