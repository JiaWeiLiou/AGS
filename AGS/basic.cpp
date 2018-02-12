#include "basic.h"

void DivideArea(InputArray _gray, InputArray _blur, OutputArray _divide)
{
	Mat gray = _gray.getMat();

	Mat blur = _blur.getMat();

	_divide.create(gray.size(), CV_8UC1);
	Mat divide = _divide.getMat();

	for (size_t i = 0; i < gray.rows; ++i) {
		for (size_t j = 0; j < gray.cols; ++j) {
			divide.at<uchar>(i, j) = (double)gray.at<uchar>(i, j) / (double)blur.at<uchar>(i, j) > 1.0f ? 255 : ((double)gray.at<uchar>(i, j) / (double)blur.at<uchar>(i, j)) * 255;
		}
	}
}

void OtsuThreshold(InputArray _gray, OutputArray _binary)
{
	Mat gray = _gray.getMat();

	_binary.create(gray.size(), CV_8UC1);
	Mat binary = _binary.getMat();

	unsigned int *hist = new unsigned int[256]();
	for (size_t i = 0; i < gray.rows; ++i) {
		for (size_t j = 0; j < gray.cols; ++j) {
			++hist[gray.at<uchar>(i, j)];
		}
	}

	unsigned int sumB = 0;
	unsigned int sum1 = 0;
	float wB = 0.0f;
	float wF = 0.0f;
	float mF = 0.0f;
	float max_var = 0.0f;
	float inter_var = 0.0f;
	unsigned char th = 0;

	for (size_t i = 0; i < 256; ++i) {
		sum1 += i * hist[i];
	}

	for (size_t i = 0; i < 256; ++i) {
		wB += hist[i];
		wF = gray.rows * gray.cols - wB;
		if (wB == 0 || wF == 0) {
			continue;
		}
		sumB += i * hist[i];
		mF = (sum1 - sumB) / wF;
		inter_var = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
		if (inter_var >= max_var) {
			th = i;
			max_var = inter_var;
		}
	}

	delete[] hist;

	cv::threshold(gray, binary, th, 255, THRESH_BINARY);
}

void Gradient(InputArray _gray, OutputArray _gradient)
{
	Mat gray = _gray.getMat();

	_gradient.create(gray.size(), CV_8UC1);
	Mat gradient = _gradient.getMat();

	Mat grayRef;
	cv::copyMakeBorder(gray, grayRef, 1, 1, 1, 1, BORDER_REPLICATE);

	float gradx = 0.0f, grady = 0.0f;
	for (size_t i = 0; i < gradient.rows; ++i) {
		for (size_t j = 0; j < gradient.cols; ++j) {
			gradx = (grayRef.at<uchar>(i + 1, j + 2) - grayRef.at<uchar>(i + 1, j)) * 0.5f;
			grady = (grayRef.at<uchar>(i + 2, j + 1) - grayRef.at<uchar>(i, j + 1)) * 0.5f;
			gradient.at<uchar>(i, j) = sqrt(pow(gradx, 2) + pow(grady, 2));
		}
	}
}

void DivideLine(InputArray _gradient, InputArray _blur, OutputArray _divide)
{
	Mat gradient = _gradient.getMat();

	Mat blur = _blur.getMat();

	_divide.create(gradient.size(), CV_8UC1);
	Mat divide = _divide.getMat();

	for (size_t i = 0; i < gradient.rows; ++i) {
		for (size_t j = 0; j < gradient.cols; ++j) {
			divide.at<uchar>(i, j) = ((double)blur.at<uchar>(i, j) / (double)gradient.at<uchar>(i, j) >= 1) ? 0 : (1 - (double)blur.at<uchar>(i, j) / (double)gradient.at<uchar>(i, j)) * 255;
		}
	}
}

void HysteresisCut(InputArray _binary, InputArray _area, OutputArray _line)
{
	Mat binary = _binary.getMat();

	Mat area = _area.getMat();

	_line.create(binary.size(), CV_8UC1);
	Mat line = _line.getMat();

	Mat UT(binary.size(), CV_8UC1, Scalar(0));
	Mat MT(binary.size(), CV_8UC1, Scalar(0));

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (binary.at<uchar>(i, j)) {
				if (area.at<uchar>(i, j)) {
					MT.at<uchar>(i, j) = 255;
				} else {
					UT.at<uchar>(i, j) = 255;
				}
			}
		}
	}

	Mat labelImg;
	int labelNum = bwlabel(MT, labelImg, 4) + 1;	// include label 0
	int* labeltable = new int[labelNum]();		// initialize label table with zero  

	int B, C, D, E, F, G, H, I;
	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			//+ - + - + - +
			//| B | C | D |
			//+ - + - + - +
			//| E | A | F |
			//+ - + - + - +
			//| G | H | I |
			//+ - + - + - +

			B = (i == 0 || j == 0) ? 0 : UT.at<uchar>(i - 1, j - 1);
			C = (i == 0) ? 0 : UT.at<uchar>(i - 1, j);
			D = (i == 0 || j == binary.cols - 1) ? 0 : UT.at<uchar>(i - 1, j + 1);
			E = (j == 0) ? 0 : UT.at<uchar>(i, j - 1);
			F = (j == binary.cols - 1) ? 0 : UT.at<uchar>(i, j + 1);
			G = (i == binary.rows - 1 || j == 0) ? 0 : UT.at<uchar>(i + 1, j - 1);
			H = (i == binary.rows - 1) ? 0 : UT.at<uchar>(i + 1, j);
			I = (i == binary.rows - 1 || j == binary.cols - 1) ? 0 : UT.at<uchar>(i + 1, j + 1);

			// apply 8 connectedness  
			if (B || C || D || E || F || G || H || I) {
				++labeltable[labelImg.at<int>(i, j)];
			}
		}
	}

	labeltable[0] = 0;		//clear 0 label

	for (size_t i = 0; i < line.rows; ++i) {
		for (size_t j = 0; j < line.cols; ++j) {
			if (labeltable[labelImg.at<int>(i, j)] || UT.at<uchar>(i, j)) { 
				line.at<uchar>(i, j) = 0;
			} else {
				line.at<uchar>(i, j) = 255;
			}
		}
	}

	delete[] labeltable;
}

void ReverseBinary(InputArray _binary, OutputArray _rbinary)
{
	Mat binary = _binary.getMat();

	_rbinary.create(binary.size(), CV_8UC1);
	Mat rbinary = _rbinary.getMat();

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (binary.at<uchar>(i, j)) { 
				rbinary.at<uchar>(i, j) = 0; 
			} else { 
				rbinary.at<uchar>(i, j) = 255; 
			}
		}
	}
}

void Combine(InputArray _area, InputArray _line, OutputArray _binary)
{
	Mat area = _area.getMat();

	Mat line = _line.getMat();

	_binary.create(area.size(), CV_8UC1);
	Mat binary = _binary.getMat();

	for (size_t i = 0; i < area.rows; ++i) {
		for (size_t j = 0; j < area.cols; ++j) {
			if (!area.at<uchar>(i, j) || !line.at<uchar>(i, j)) { 
				binary.at<uchar>(i, j) = 0; 
			} else { 
				binary.at<uchar>(i, j) = 255; 
			}
		}
	}
}

void ClearNoise(InputArray _binary, OutputArray _clear)
{
	Mat binary = _binary.getMat();

	_clear.create(binary.size(), CV_8UC1);
	Mat clear = _clear.getMat();

	Mat rbinary(binary.rows + 2, binary.cols + 2, CV_8UC1, 255);
	for (size_t i = 1; i < rbinary.rows - 1; ++i) {
		for (size_t j = 1; j < rbinary.cols - 1; ++j) {
			rbinary.at<uchar>(i, j) = binary.at<uchar>(i - 1, j - 1) ? 0 : 255;
		}
	}

	Mat labelImg;
	int num = bwlabel(rbinary, labelImg, 8);

	int *labeltable = new int[num + 1]();

	// calculate obj size
	for (size_t i = 0; i < labelImg.rows; ++i) {
		for (size_t j = 0; j < labelImg.cols; ++j) {
			++labeltable[labelImg.at<int>(i, j)];
		}
	}

	// find max size
	int maxSize = 0;
	for (size_t i = 1; i < num + 1; ++i) {
		maxSize = labeltable[i] > maxSize ? labeltable[i] : maxSize;
	}

	int tolSize = ceil((double)maxSize * 0.001);

	for (size_t i = 0; i < labelImg.rows; ++i) {
		for (size_t j = 0; j < labelImg.cols; ++j) {
			if (labeltable[labelImg.at<int>(i, j)] < tolSize) {
				rbinary.at<uchar>(i, j) = 0;
			}
		}
	}

	for (size_t i = 0; i < clear.rows; ++i) {
		for (size_t j = 0; j < clear.cols; ++j) {
			clear.at<uchar>(i, j) = rbinary.at<uchar>(i + 1, j + 1) ? 0 : 255;
		}
	}

	delete[] labeltable;
}

void DeleteEdge(InputArray _binary, OutputArray _object)
{
	Mat binary = _binary.getMat();

	_object.create(binary.size(), CV_8UC1);
	Mat object = _object.getMat();
	binary.copyTo(object);

	for (size_t i = 0; i < object.cols; ++i) {
		if (object.at<uchar>(0, i) == 255) { 
			floodFill(object, Point(i, 0), 0);
		}
		if (object.at<uchar>(object.rows - 1, i) == 255) { 
			floodFill(object, Point(i, object.rows - 1), 0);
		}
	}
	for (size_t i = 0; i < object.rows; ++i) {
		if (object.at<uchar>(i, 0) == 255) { 
			floodFill(object, Point(0, i), 0);
		}
		if (object.at<uchar>(i, object.cols - 1) == 255) { 
			floodFill(object, Point(object.cols - 1, i), 0);
		}
	}
}