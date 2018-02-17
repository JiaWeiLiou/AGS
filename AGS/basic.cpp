/******************************************************************************
FileName     [ basic.h ]
PackageName  [ AGS ]
Synopsis     [ Automatic Grain Sizing ]
Author       [ Jia-Wei (Jimmy) Liou ]
Copyright    [ Copyleft(c) 2018-present LaDF, CE-Hydrolic, NTU, Taiwan ]
******************************************************************************/

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
				line.at<uchar>(i, j) = 255;
			} else {
				line.at<uchar>(i, j) = 0;
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

//void Reconstruct(InputArray _marker, InputArray _mask, OutputArray _hdistance)
//{
//	Mat marker = _marker.getMat();
//
//	Mat mask = _mask.getMat();
//
//	_hdistance.create(mask.size(), CV_32FC1);
//	Mat hdistance = _hdistance.getMat();
//
//	min(marker, mask, hdistance);
//	dilate(hdistance, hdistance, Mat());
//	min(hdistance, mask, hdistance);
//	Mat temp1 = Mat(marker.size(), CV_8UC1);
//	Mat temp2 = Mat(marker.size(), CV_8UC1);
//	do {
//		hdistance.copyTo(temp1);
//		dilate(hdistance, hdistance, Mat());
//		min(hdistance, mask, hdistance);
//		compare(temp1, hdistance, temp2, CV_CMP_NE);
//	} while (sum(temp2).val[0]);
//}

void Reconstruct(InputArray _marker, InputArray _mask, OutputArray _hdistance)
{
	Mat marker = _marker.getMat();

	Mat mask = _mask.getMat();

	_hdistance.create(mask.size(), CV_32FC1);
	Mat hdistance = _hdistance.getMat();

	// neighbourhood group forward scan
	Point2i  m[4] = { Point2i(-1, -1), Point2i(-1, 0), Point2i(-1, 1), Point2i(0, -1) };
	// neighbourhood group backward scan
	Point2i  n[4] = { Point2i(1, 1), Point2i(1, 0), Point2i(1, -1), Point2i(0, 1) };
	// complete neighbourhood 
	Point2i  nc[8] = { Point2i(-1, -1), Point2i(-1, 0), Point2i(-1, 1), Point2i(0, -1),
					   Point2i(0, 1), Point2i(1, -1), Point2i(1, 0), Point2i(1, 1) };


	// forward reconstruction
	for (size_t i = 0; i < marker.rows; ++i) {
		for (size_t j = 0; j < marker.cols; ++j) {
			// find regional maximum of neighbourhood 
			float d = marker.at<float>(i, j);
			for (size_t k = 0; k < 4; ++k) {
				if (i + m[k].x >= 0 && j + m[k].y >= 0 && i + m[k].x < marker.rows && j + m[k].y < marker.cols) {
					d = std::max(marker.at<float>(i + m[k].x, j + m[k].y), d);
				}
			}

			// compare mask and maximum value of neighbourhood
			// write this value on position i,j
			d = std::min(mask.at<float>(i, j), d);
			hdistance.at<float>(i, j) = d;
		}
	}

	std::queue<Point2i> fifo;

	// backward reconstruction
	for (int i = marker.rows - 1; i >= 0; --i) {
		for (int j = marker.cols - 1; j >= 0; --j) {
			// find regional maximum of neighbourhood 
			float d = hdistance.at<float>(i, j);
			for (size_t k = 0; k < 4; ++k) {
				if (i + n[k].x >= 0 && j + n[k].y >= 0 && i + n[k].x < marker.rows && j + n[k].y < marker.cols) {
					d = std::max(hdistance.at<float>(i + n[k].x, j + n[k].y), d);
				}
			}

			// compare mask and maximum value of neighbourhood
			// write this value on position i,j
			d = std::min(mask.at<float>(i, j), d);
			hdistance.at<float>(i, j) = d;

			/* check if the current position might be changed in a forward scan */
			for (size_t k = 0; k < 4; ++k) {
				if (i + m[k].x >= 0 && j + m[k].y >= 0 && i + m[k].x < marker.rows && j + m[k].y < marker.cols) {
					float q = hdistance.at<float>(i + m[k].x, j + m[k].y);
					if ((q < d) && (q < mask.at<float>(i + m[k].x, j + m[k].y))) {
						fifo.push(Point2i(i, j));
						break;
					}
				}
			}
		}
	}

	while (!fifo.empty()) {
		Point2i base = fifo.front();
		fifo.pop();

		// complete neighbourhood
		for (size_t k = 0; k < 8; ++k) {
			Point2i base_new = base + nc[k];
			if (base_new.x >= 0 && base_new.y >= 0 && base_new.x < marker.rows && base_new.y < marker.cols) {
				if ((hdistance.at<float>(base_new.x, base_new.y) < hdistance.at<float>(base.x, base.y)) && (hdistance.at<float>(base_new.x, base_new.y) < mask.at<float>(base_new.x, base_new.y))) {
					hdistance.at<float>(base_new.x, base_new.y) = std::min(hdistance.at<float>(base.x, base.y), mask.at<float>(base_new.x, base_new.y));
					fifo.push(base_new);
				}
			}
		}
	}
}

void HMinimaTransform(InputArray _distance, OutputArray _hdistance, float h)
{
	Mat distance = _distance.getMat();

	_hdistance.create(distance.size(), CV_32FC1);
	Mat hdistance = _hdistance.getMat();

	Mat marker(distance.size(), CV_32FC1);
	for (size_t i = 0; i < distance.rows; ++i) {
		for (size_t j = 0; j < distance.cols; ++j) {
			marker.at<float>(i, j) = distance.at<float>(i, j) - h;
		}
	}

	Reconstruct(marker, distance, hdistance);
}

void DetectRegionalMinima(InputArray _distance, OutputArray _seed)
{
	Mat distance = _distance.getMat();

	_seed.create(distance.size(), CV_8UC1);
	Mat seed = _seed.getMat();
	seed = Scalar(255);

	queue<Point2i> *mpFifo = new queue<Point2i>();

	for (size_t y = 0; y < seed.rows; ++y) {
		for (size_t x = 0; x < seed.cols; ++x) {
			if (seed.at<uchar>(y, x)) {
				for (int dy = -1; dy <= 1; ++dy) {
					for (int dx = -1; dx <= 1; ++dx) {
						if ((x + dx >= 0) && (x + dx < distance.cols) && (y + dy >= 0) && (y + dy < distance.rows)) {
							// If pe2.value < pe1.value, pe1 is not a local minimum
							if (floor(distance.at<float>(y, x)) > floor(distance.at<float>(y + dy, x + dx))) {
								seed.at<uchar>(y, x) = 0;
								mpFifo->push(Point2i(x, y));

								while (!mpFifo->empty()) {
									Point2i pe3 = mpFifo->front();
									mpFifo->pop();

									int xh = pe3.x;
									int yh = pe3.y;

									for (int dyh = -1; dyh <= 1; ++dyh) {
										for (int dxh = -1; dxh <= 1; ++dxh) {
											if ((xh + dxh >= 0) && (xh + dxh < distance.cols) && (yh + dyh >= 0) && (yh + dyh < distance.rows)) {
												if (seed.at<uchar>(yh + dyh, xh + dxh)) {
													if (floor(distance.at<float>(yh + dyh, xh + dxh)) == floor(distance.at<float>(y, x))) {
														seed.at<uchar>(yh + dyh, xh + dxh) = 0;
														mpFifo->push(Point2i(xh + dxh, yh + dyh));
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void ExtendRegionalMinima(InputArray _distance, OutputArray _seed, float h)
{
	Mat distance = _distance.getMat();

	_seed.create(distance.size(), CV_8UC1);
	Mat seed = _seed.getMat();

	float imageMinLength = distance.rows < distance.cols ? distance.rows : distance.cols;

	for (size_t i = 0; i < distance.rows; ++i) {
		for (size_t j = 0; j < distance.cols; ++j) {
			distance.at<float>(i, j) = distance.at<float>(i, j) > imageMinLength * 0.025 ? imageMinLength * 0.025 : distance.at<float>(i, j);
		}
	}

	Mat objectHMT;
	HMinimaTransform(distance, objectHMT, h);

	for (size_t i = 0; i < objectHMT.rows; ++i) {
		for (size_t j = 0; j < objectHMT.cols; ++j) {
			objectHMT.at<float>(i, j) = -objectHMT.at<float>(i, j);
		}
	}

	DetectRegionalMinima(objectHMT, seed);
}

void DistanceCut(InputArray _distance, OutputArray _seed)
{
	Mat distance = _distance.getMat();

	_seed.create(distance.size(), CV_8UC1);
	Mat seed = _seed.getMat();

	float imageMinLength = distance.rows < distance.cols ? distance.rows : distance.cols;

	for (size_t i = 0; i < distance.rows; ++i) {
		for (size_t j = 0; j < distance.cols; ++j) {
			seed.at<uchar>(i, j) = distance.at<float>(i, j) > imageMinLength * 0.025 ? 255 : 0;
		}
	}
}

void AddSeed(InputArray _binary, InputArray _seed, OutputArray _fseed)
{
	Mat binary = _binary.getMat();

	Mat seed = _seed.getMat();

	_fseed.create(binary.size(), CV_8UC1);
	Mat fseed = _fseed.getMat();
	seed.copyTo(fseed);

	Mat labels;
	int num = bwlabel(binary, labels, 4) + 1;	//include label 0

	int *labeltable = new int[num]();

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (fseed.at<uchar>(i, j)) {
				++labeltable[labels.at<int>(i, j)];
			}
		}
	}

	labeltable[0] = 1;	// add background

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (!labeltable[labels.at<int>(i, j)]) {
				fseed.at<uchar>(i, j) = 255;
			}
		}
	}

	delete[] labeltable;
	labeltable = nullptr;
}

void DetectMinima(InputArray _distance, InputArray _seed, OutputArray _labelImg, priority_queue<PixelElement, vector<PixelElement>, mycomparison> &sortedQueue)
{
	Mat distance = _distance.getMat();

	Mat seed = _seed.getMat();

	_labelImg.create(distance.size(), CV_32SC1);
	Mat labelImg = _labelImg.getMat();

	int num = bwlabel(seed, labelImg, 8);

	Mat element = (Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	Mat seedDilate;
	morphologyEx(seed, seedDilate, MORPH_DILATE, element);

	for (size_t i = 0; i < seed.rows; ++i) {
		for (size_t j = 0; j < seed.cols; ++j) {
			if (seedDilate.at<uchar>(i, j) && !seed.at<uchar>(i, j)) {
				labelImg.at<int>(i, j) = LABEL_PROCESSING;
				sortedQueue.push(PixelElement(distance.at<float>(i, j), j, i));
			} else {
				labelImg.at<int>(i, j) = seed.at<uchar>(i, j) ? labelImg.at<int>(i, j) : LABEL_NOLOCALMINIMUM;
			}
		}
	}
}

bool CheckForAlreadyLabeledNeighbours(int x, int y, Mat &label, Point2i &outLabeledNeighbour, int &outLabel)
{
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if ((x + dx >= 0) && (x + dx < label.cols) && (y + dy >= 0) && (y + dy < label.rows)) {
				if (label.at<int>(y + dy, x + dx) > LABEL_RIDGE) {
					outLabeledNeighbour = Point2i(x + dx, y + dy);
					outLabel = label.at<int>(y + dy, x + dx);
					return true;
				}
			}
		}
	}
	return false;
}

bool CheckIfPixelIsWatershed(int x, int y, Mat &label, Point2i &inLabeledNeighbour)
{
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if ((x + dx >= 0) && (x + dx < label.cols) && (y + dy >= 0) && (y + dy < label.rows)) {
				if ((label.at<int>(y + dy, x + dx) >= LABEL_MINIMUM) && (label.at<int>(y + dy, x + dx) != label.at<int>(inLabeledNeighbour.y, inLabeledNeighbour.x)) && ((inLabeledNeighbour.x != x + dx) || (inLabeledNeighbour.y != y + dy))) {
					label.at<int>(y, x) = LABEL_RIDGE;
					return true;
				}
			}
		}
	}
	return false;
}

void WatershedTransform(InputArray _binary, InputArray _seed, InputArray _distance, OutputArray _object)
{
	Mat binary = _binary.getMat();

	Mat seed = _seed.getMat();

	Mat distance = _distance.getMat();

	_object.create(distance.size(), CV_8UC1);
	Mat object = _object.getMat();

	for (size_t i = 0; i < distance.rows; ++i) {
		for (size_t j = 0; j < distance.cols; ++j) {
			distance.at<float>(i, j) = -distance.at<float>(i, j);
		}
	}

	Mat label;
	priority_queue<PixelElement, vector<PixelElement>, mycomparison> mvSortedQueue;
	DetectMinima(distance, seed, label, mvSortedQueue);

	while (!mvSortedQueue.empty()) {
		PixelElement lItemA = mvSortedQueue.top();
		mvSortedQueue.pop();
		int labelOfNeighbour = 0;

		// (a) Pop element and find positive labeled neighbour
		Point2i alreadyLabeledNeighbour;
		int x = lItemA.x;
		int y = lItemA.y;

		// (b) Check if current pixel is watershed pixel by checking if there are different finally labeled neighbours
		if (CheckForAlreadyLabeledNeighbours(x, y, label, alreadyLabeledNeighbour, labelOfNeighbour)) {
			if (!(CheckIfPixelIsWatershed(x, y, label, alreadyLabeledNeighbour))) {
				// c) if not watershed pixel, assign label of neighbour and add the LABEL_NOLOCALMINIMUM neighbours to priority_queue for processing
				/*UpdateLabel(x, y, distance, label, labelOfNeighbour, mvSortedQueue);*/

				label.at<int>(y, x) = labelOfNeighbour;

				for (int dx = -1; dx <= 1; ++dx) {
					for (int dy = -1; dy <= 1; ++dy) {
						if ((x + dx >= 0) && (x + dx < label.cols) && (y + dy >= 0) && (y + dy < label.rows)) {
							if (label.at<int>(y + dy, x + dx) == LABEL_NOLOCALMINIMUM) {
								label.at<int>(y + dy, x + dx) = LABEL_PROCESSING;
								mvSortedQueue.push(PixelElement(distance.at<float>(y + dy, x + dx), x + dx, y + dy));
							}
						}
					}
				}
			}
		}
	}

	binary.copyTo(object);

	// d) finalize the labelImage
	for (size_t i = 0; i < object.rows; ++i) {
		for (size_t j = 0; j < object.cols; ++j) {
			if (label.at<int>(i, j) == LABEL_RIDGE) {
				object.at<uchar>(i, j) = 0;
			}
		}
	}
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