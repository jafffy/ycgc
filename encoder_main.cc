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

	char buf[4096];

	int w = 59;
	int h = 33;

	Mat high_frame = Mat::zeros(Size(w*64, h*64), CV_32F);
	Mat moderate_frame = Mat::zeros(Size(w*64, h*64), CV_32F);

	const char* base_path = "../data/frame777";

	struct stat output_file;

	for (int wi = 0; wi < w; ++wi) {
		for (int hi = 0; hi < h; ++hi) {
			sprintf(buf, "%s/%d_%d.png", base_path, wi+1, hi+1);

			Mat image = imread(buf, IMREAD_GRAYSCALE);

			if (! image.data) {
				fprintf(stderr, "Could not open or find the image\n");

				return -1;
			}

			float mean_intensity = 0.0f;
			float pe = get_pseudo_entropy(mean_intensity, image.data, 64 * 64);

			if (pe > 7.0f) { // Hard. Lossless compression.
				high_frame(Range(hi*64, (hi+1)*64), Range(wi*64, (wi+1)*64)) = image;
			} else if (pe > 3.0) { // Moderate. Lossy compression with backend encoder.
				moderate_frame(Range(hi*64, (hi+1)*64), Range(wi*64, (wi+1)*64)) = image;
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

		}
	}

	{
		struct stat output_file;

		vector<int> compression_params;
		compression_params.push_back(IMWRITE_JPEG_QUALITY);
		compression_params.push_back(75);

		imwrite("/tmp/output.jpg", moderate_frame, compression_params);

		lstat("/tmp/output.jpg", &output_file);
		printf("%d\n", output_file.st_size);
	}

	{
		struct stat output_file;

		vector<int> compression_params;
		compression_params.push_back(IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(3);

		imwrite("/tmp/output.png", high_frame, compression_params);

		lstat("/tmp/output.png", &output_file);
		printf("%d\n", output_file.st_size);
	}

	return 0;
}
