/******************************************************************************
FileName     [ main.h ]
PackageName  [ AGS ]
Synopsis     [ Main Operation for AGS ]
Author       [ Jia-Wei (Jimmy) Liou ]
Copyright    [ Copyleft(c) 2018-present LaDF, CE-Hydrolic, NTU, Taiwan ]
******************************************************************************/

#include "basic.h"
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>

//#define OUTPUTIMG
#define OUTPUTTIME

int main()
{
	std::cout << "Please enter image path : ";
	string imgfile;
	std::cin >> imgfile;

	/* Set Output File Name */

	int pos1 = imgfile.find_last_of('/\\');
	int pos2 = imgfile.find_last_of('.');
	string filepath(imgfile.substr(0, pos1));							//file path
	string infilename(imgfile.substr(pos1 + 1, pos2 - pos1 - 1));		//file name
	string infilefullname(imgfile.substr(pos1 + 1));					//file full name

	/*Read Raw Image*/

	Mat image = cv::imread(imgfile);			//raw image (8UC3)
	if (!image.data) { 
		printf("Oh no! reading image error... \n"); 
		return false; 
	}

	std::cout << "Image's width  : " << image.cols << endl;
	std::cout << "Image's height : " << image.rows << endl;

	ifstream infile;
	string inputPath = filepath + "\\" + "IMG(PT).txt";
	infile.open(inputPath, ios::in);
	if (!infile.is_open()) {
		cout << "Oh no! reading points error... \n" << endl;
		return 0;
	}

	Point2f bPt[4];
	for (size_t i = 0; i < 4; ++i) {
		string xp, yp;
		std::getline(infile, xp, '\t');
		std::getline(infile, yp, '\t');
		bPt[i] = Point2f(stof(xp), stof(yp));
	}
	infile.close();
	
	int pl = round((std::sqrt(std::pow(bPt[0].x - bPt[1].x, 2) + std::pow(bPt[0].y - bPt[1].y, 2))
		+ std::sqrt(std::pow(bPt[2].x - bPt[3].x, 2) + std::pow(bPt[2].y - bPt[3].y, 2))
		+ std::sqrt(std::pow(bPt[0].x - bPt[3].x, 2) + std::pow(bPt[0].y - bPt[3].y, 2))
		+ std::sqrt(std::pow(bPt[1].x - bPt[2].x, 2) + std::pow(bPt[1].y - bPt[2].y, 2))) / 4.0f);
	Point2f aPt[4] = { cv::Point2f(0, 0), cv::Point2f(pl, 0), cv::Point2f(pl, pl), cv::Point2f(0, pl) };

	std::cout << "Output of image's size (pixel) : " << pl << endl;
	std::cout << "Please enter square's length (mm) : ";
	float rl;
	std::cin >> rl;

#ifdef OUTPUTTIME
	time_t time0 = clock();
	time_t time1, time2;
#endif // OUTPUTTIME

	/**** Image Pre-Processing ****/

	/* Convert RGB Image to Gray */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat gray;			//8UC1
	cv::cvtColor(image, gray, CV_BGR2GRAY);

#ifdef OUTPUTIMG
	Mat gray_C;			//output(8UC3)
	DrawColorBar(gray, gray_C);

	string gray_G_file = filepath + "\\" + infilename + "_0.0_GRAY(G).png";			//Gray
	cv::imwrite(gray_G_file, gray);
	string gray_C_file = filepath + "\\" + infilename + "_0.1_GRAY(C).png";			//Color
	cv::imwrite(gray_C_file, gray_C);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Convert RGB Image to Gray : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Perspective Projection Transformation */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat grayWarp;
	Mat perspectiveMatrix = getPerspectiveTransform(bPt, aPt);
	cv::warpPerspective(gray, grayWarp, perspectiveMatrix, Size(pl, pl), INTER_CUBIC);

#ifdef OUTPUTIMG
	string grayWarp_G_file = filepath + "\\" + infilename + "_1.0_WARP_I(G).png";			//Gray
	cv::imwrite(grayWarp_G_file, grayWarp);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Perspective Projection Transformation : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/**** Area-Based Image Extraction ****/

	/* Bluring Image */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	int imageMinLength = grayWarp.rows < grayWarp.cols ? grayWarp.rows : grayWarp.cols;
	int ksize = ceil((double)imageMinLength / 10.0);
	ksize = ksize % 2 ? ksize : ksize + 1;
	double sigma = ksize / 5;

	Mat grayBlur;			//8UC1
	cv::GaussianBlur(grayWarp, grayBlur, Size(ksize, ksize), sigma, sigma);

#ifdef OUTPUTIMG
	Mat grayBlur_C;			//output(8UC3)
	DrawColorBar(grayBlur, grayBlur_C);

	string grayBlur_G_file = filepath + "\\" + infilename + "_2.0_BLUR_I(G).png";			//Gray
	cv::imwrite(grayBlur_G_file, grayBlur);
	string grayBlur_C_file = filepath + "\\" + infilename + "_2.1_BLUR_I(C).png";			//Color
	cv::imwrite(grayBlur_C_file, grayBlur_C);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Bluring Image : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Remove Ambient Light for Area */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat grayDIV;			//8UC1
	DivideArea(grayWarp, grayBlur, grayDIV);

#ifdef OUTPUTIMG
	Mat grayDIV_C;			//output(8UC3)
	DrawColorBar(grayDIV, grayDIV_C);

	string grayDIV_G_file = filepath + "\\" + infilename + "_3.0_DIV_I(G).png";			//Gray
	cv::imwrite(grayDIV_G_file, grayDIV);
	string grayDIV_C_file = filepath + "\\" + infilename + "_3.1_DIV_I(C).png";			//Color
	cv::imwrite(grayDIV_C_file, grayDIV_C);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Remove Ambient Light for Area : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Otsu Threshold */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat grayTH;			//8UC1(BW)
	OtsuThreshold(grayDIV, grayTH);

#ifdef OUTPUTIMG
	string grayTH_B_file = filepath + "\\" + infilename + "_4.0_TH_I(B).png";			//Binary
	cv::imwrite(grayTH_B_file, grayTH);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Otsu Threshold : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Area */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat area = grayTH;			//8UC1(BW)

#ifdef OUTPUTIMG
	Mat area_L, area_I;			//output(8UC3¡B8UC3)
	DrawLabel(area, area_L);
	DrawImage(area, grayWarp, area_I);

	string area_B_file = filepath + "\\" + infilename + "_5.0_AREA(B).png";			//Binary
	cv::imwrite(area_B_file, area);
	string area_L_file = filepath + "\\" + infilename + "_5.1_AREA(L).png";			//Labels
	cv::imwrite(area_L_file, area_L);
	string area_I_file = filepath + "\\" + infilename + "_5.2_AREA(I).png";			//Combine
	cv::imwrite(area_I_file, area_I);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Area : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/**** Line-Based Image Extraction ****/

	/* Calculate Image Gradient */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat gradm;			//8UC1
	Gradient(grayWarp, gradm);

#ifdef OUTPUTIMG
	Mat gradm_G, gradm_C;			//output(8UC1¡B8UC3)
	DrawGrayBar(gradm, gradm_G);
	DrawColorBar(gradm, gradm_C);;

	string gradm_G_file = filepath + "\\" + infilename + "_6.0_GRAD_M(G).png";			//Gray
	cv::imwrite(gradm_G_file, gradm_G);
	string gradm_C_file = filepath + "\\" + infilename + "_6.1_GRAD_M(C).png";			//Color
	cv::imwrite(gradm_C_file, gradm_C);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Calculate Image Gradient : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Bluring Image Gradient */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat gradmBlur;			//8UC1
	cv::GaussianBlur(gradm, gradmBlur, Size(5, 5), 1, 1);

#ifdef OUTPUTIMG
	Mat gradmBlur_C;			//output(8UC3)
	DrawColorBar(gradmBlur, gradmBlur_C);

	string gradmBlur_G_file = filepath + "\\" + infilename + "_7.0_BLUR_M(G).png";			//Gray
	cv::imwrite(gradmBlur_G_file, gradmBlur);
	string gradmBlur_C_file = filepath + "\\" + infilename + "_7.1_BLUR_R(C).png";			//Color
	cv::imwrite(gradmBlur_C_file, gradmBlur_C);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Bluring Image Gradient : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Remove Ambient Light for Image Gradient */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat gradmDIV;			//8UC1
	DivideLine(gradm, gradmBlur, gradmDIV);

#ifdef OUTPUTIMG
	Mat gradmDIV_G, gradmDIV_C;			//output(8UC1¡B8UC3)
	DrawGrayBar(gradmDIV, gradmDIV_G);
	DrawColorBar(gradmDIV, gradmDIV_C);

	string gradmDIV_G_file = filepath + "\\" + infilename + "_8.0_DIV_M(G).png";		//Gray
	cv::imwrite(gradmDIV_G_file, gradmDIV_G);
	string gradmDIV_C_file = filepath + "\\" + infilename + "_8.1_DIV_M(C).png";		//Color
	cv::imwrite(gradmDIV_C_file, gradmDIV_C);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Remove Ambient Light for Image Gradient : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Binary Image Gradient */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat gradmHT;			//8UC1(BW)
	cv::threshold(gradmDIV, gradmHT, 1, 255, THRESH_BINARY);

#ifdef OUTPUTIMG
	string gradmHT_B_file = filepath + "\\" + infilename + "_9.0_HT_M(B).png";			//Binary
	cv::imwrite(gradmHT_B_file, gradmHT);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Binary Image Gradient : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Hysteresis Cut Binary Image to Line by Area */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat lineHC;			//8UC1(BW)
	HysteresisCut(gradmHT, area, lineHC);

#ifdef OUTPUTIMG
	string lineHC_B_file = filepath + "\\" + infilename + "_10.0_HC_L(B).png";			//Binary
	cv::imwrite(lineHC_B_file, lineHC);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Hysteresis Cut Binary Image to Line by Area : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Line */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat line;			//8UC1(BW)
	ReverseBinary(lineHC, line);

#ifdef OUTPUTIMG
	Mat line_L, line_I;			//output(8UC3¡B8UC3)
	DrawLabel(line, line_L);
	DrawImage(line, grayWarp, line_I);

	string line_B_file = filepath + "\\" + infilename + "_11.0_LINE(B).png";			//Binary
	cv::imwrite(line_B_file, line);
	string line_L_file = filepath + "\\" + infilename + "_11.1_LINE(L).png";			//Labels
	cv::imwrite(line_L_file, line_L);
	string line_I_file = filepath + "\\" + infilename + "_11.2_LINE(I).png";			//Combine
	cv::imwrite(line_I_file, line_I);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Line : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/**** Combine Image Extraction ****/

	/* Combine Area and Line */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectCOM;			//8UC1(BW)
	Combine(area, line, objectCOM);

#ifdef OUTPUTIMG
	Mat objectCOM_L, objectCOM_I;			//output(8UC3¡B8UC3)
	DrawLabel(objectCOM, objectCOM_L);
	DrawImage(objectCOM, grayWarp, objectCOM_I);

	string  objectCOM_B_file = filepath + "\\" + infilename + "_12.0_COM_O(B).png";			//Binary
	cv::imwrite(objectCOM_B_file, objectCOM);
	string  objectCOM_L_file = filepath + "\\" + infilename + "_12.1_COM_O(L).png";			//Labels
	cv::imwrite(objectCOM_L_file, objectCOM_L);
	string  objectCOM_I_file = filepath + "\\" + infilename + "_12.2_COM_O(I).png";			//Combine
	cv::imwrite(objectCOM_I_file, objectCOM_I);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Combine Area and Line : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Image morphology Opening */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectOpen;			//8UC1(BW)
	Mat elementO = (Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	cv::morphologyEx(objectCOM, objectOpen, MORPH_OPEN, elementO);

#ifdef OUTPUTIMG
	Mat objectOpen_L, objectOpen_I;			//output(8UC3¡B8UC3)
	DrawLabel(objectOpen, objectOpen_L);
	DrawImage(objectOpen, grayWarp, objectOpen_I);

	string  objectOpen_B_file = filepath + "\\" + infilename + "_13.0_OPEN_O(B).png";			//Binary
	cv::imwrite(objectOpen_B_file, objectOpen);
	string  objectOpen_L_file = filepath + "\\" + infilename + "_13.1_OPEN_O(L).png";			//Labels
	cv::imwrite(objectOpen_L_file, objectOpen_L);
	string  objectOpen_I_file = filepath + "\\" + infilename + "_13.2_OPEN_O(I).png";			//Combine
	cv::imwrite(objectOpen_I_file, objectOpen_I);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Image morphology Opening : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Clear Noise of Black Pepper */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectCN;			//8UC1(BW)
	ClearNoise(objectOpen, objectCN);

#ifdef OUTPUTIMG
	Mat objectCN_L, objectCN_I;			//output(8UC3¡B8UC3)
	DrawLabel(objectCN, objectCN_L);
	DrawImage(objectCN, grayWarp, objectCN_I);

	string  objectCN_B_file = filepath + "\\" + infilename + "_14.0_CN_O(B).png";			//Binary
	cv::imwrite(objectCN_B_file, objectCN);
	string  objectCN_L_file = filepath + "\\" + infilename + "_14.1_CN_O(L).png";			//Labels
	cv::imwrite(objectCN_L_file, objectCN_L);
	string  objectCN_I_file = filepath + "\\" + infilename + "_14.2_CN_O(I).png";			//Combine
	cv::imwrite(objectCN_I_file, objectCN_I);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Clear Noise of Black Pepper : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/**** Watershed Algorithm ****/

	/* Distance Transform */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectDT;		//32FC1
	cv::distanceTransform(objectCN, objectDT, CV_DIST_L2, 3);

#ifdef OUTPUTIMG
	Mat objectDT_G, objectDT_C;		//output(8UC1¡B8UC3)
	DrawGrayBar(objectDT, objectDT_G);
	DrawColorBar(objectDT, objectDT_C);

	string objectDT_G_file = filepath + "\\" + infilename + "_15.0_DT_O(G).png";			//Gray
	cv::imwrite(objectDT_G_file, objectDT_G);
	string objectDT_C_file = filepath + "\\" + infilename + "_15.1_DT_O(C).png";			//Color
	cv::imwrite(objectDT_C_file, objectDT_C);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Distance Transform : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Extend Local Minima */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectEM;		//8UC1(BW)
	ExtendRegionalMinima(objectDT, objectEM, 3);

#ifdef OUTPUTIMG
	Mat objectEM_S;		//output(8UC1)
	DrawSeed(objectCN, objectEM, objectEM_S);

	string  objectEM_S_file = filepath + "\\" + infilename + "_16.0_EM_O(S).png";			//Seed
	cv::imwrite(objectEM_S_file, objectEM_S);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Extend Local Minima : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Add unlabeled labels */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectAS;		//8UC1(BW)
	AddSeed(objectCN, objectEM, objectAS);

#ifdef OUTPUTIMG
	Mat objectAS_S;		//output(8UC1)
	DrawSeed(objectCN, objectAS, objectAS_S);

	string  objectAS_S_file = filepath + "\\" + infilename + "_17.0_AS_O(S).png";			//Seed
	cv::imwrite(objectAS_S_file, objectAS_S);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Add unlabeled labels : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Watershed Segmentation */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectWT;		//8UC1(BW)
	WatershedTransform(objectCN, objectAS, objectDT, objectWT);

#ifdef OUTPUTIMG
	Mat objectWT_L, objectWT_I;		//output(8UC3¡B8UC3)
	DrawLabel(objectWT, objectWT_L);
	DrawImage(objectWT, grayWarp, objectWT_I);

	string  objectWT_B_file = filepath + "\\" + infilename + "_18.0_WT_O(B).png";			//Binary
	cv::imwrite(objectWT_B_file, objectWT);
	string  objectWT_L_file = filepath + "\\" + infilename + "_18.1_WT_O(L).png";			//Labels
	cv::imwrite(objectWT_L_file, objectWT_L);
	string  objectWT_I_file = filepath + "\\" + infilename + "_18.2_WT_O(I).png";			//Combine
	cv::imwrite(objectWT_I_file, objectWT_I);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Watershed Segmentation : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/**** Particle Post-Calculation ****/

	/* Delete Edge object */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectDE;		//8UC1(BW)
	DeleteEdge(objectWT, objectDE);

#ifdef OUTPUTIMG
	Mat objectDE_L, objectDE_I;		//output(8UC3¡B8UC3)
	DrawLabel(objectDE, objectDE_L);
	DrawImage(objectDE, grayWarp, objectDE_I);

	string  objectDE_B_file = filepath + "\\" + infilename + "_19.0_DE_O(B).png";			//Binary
	cv::imwrite(objectDE_B_file, objectDE);
	string  objectDE_L_file = filepath + "\\" + infilename + "_19.1_DE_O(L).png";			//Labels
	cv::imwrite(objectDE_L_file, objectDE_L);
	string  objectDE_I_file = filepath + "\\" + infilename + "_19.2_DE_O(I).png";			//Combine
	cv::imwrite(objectDE_I_file, objectDE_I);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Delete Edge object : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	/* Fitting Ellipse */

#ifdef OUTPUTTIME
	time1 = clock();
#endif // OUTPUTTIME

	Mat objectFE;		//8UC1(BW)
	vector<Size2f> ellipse;
	vector<Size2f> square;
	DrawEllipse(objectDE, objectFE, ellipse, square);

#ifdef OUTPUTIMG
	string  objectFE_B_file = filepath + "\\" + infilename + "_20.0_FE_O(B).png";			//Binary
	cv::imwrite(objectFE_B_file, objectFE);
#endif // OUTPUTIMG
#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Fitting Ellipse : " << (float)(time2 - time1) / CLOCKS_PER_SEC << " s" << endl;
#endif // OUTPUTTIME

	ofstream outfileM, outfileL, outfileSL, outfileSD;
	string outputPathM = filepath + "\\" + "AGS_M(PSD).txt";
	string outputPathL = filepath + "\\" + "AGS_L(PSD).txt";
	string outputPathSL = filepath + "\\" + "AGS_SL(PSD).txt";
	string outputPathSD = filepath + "\\" + "AGS_SD(PSD).txt";
	outfileM.open(outputPathM, ios::out | ios::trunc);
	outfileL.open(outputPathL, ios::out | ios::trunc);
	outfileSL.open(outputPathSL, ios::out | ios::trunc);
	outfileSD.open(outputPathSD, ios::out | ios::trunc);

	vector<float> outAxisM, outAxisL, outAxisSL, outAxisSD;
	for (size_t i = 0; i < ellipse.size(); ++i) {
		float wAxis = ellipse[i].width * rl / pl;
		float hAxis = ellipse[i].height * rl / pl;
		if (wAxis < hAxis) {
			outAxisM.push_back(wAxis);
			outAxisL.push_back(hAxis);
		} else {
			outAxisM.push_back(hAxis);
			outAxisL.push_back(wAxis);
		}

		outAxisSL.push_back(square[i].width);
		outAxisSD.push_back(square[i].height);
	}

	std::sort(outAxisM.begin(), outAxisM.end());
	std::sort(outAxisL.begin(), outAxisL.end());
	std::sort(outAxisSL.begin(), outAxisSL.end());
	std::sort(outAxisSD.begin(), outAxisSD.end());

	outfileM << infilefullname << ":\t";
	outfileL << infilefullname << ":\t";
	outfileSL << infilefullname << ":\t";
	outfileSD << infilefullname << ":\t";

	for (size_t i = 0; i < outAxisM.size(); ++i) {
		outfileM << outAxisM[i];
		if (i != outAxisM.size() - 1) {
			outfileM << "\t";
		}
	}
	

	for (size_t i = 0; i < outAxisL.size(); ++i) {
		outfileL << outAxisL[i];
		if (i != outAxisL.size() - 1) {
			outfileL << "\t";
		}
	}

	for (size_t i = 0; i < outAxisSL.size(); ++i) {
		outfileSL << outAxisSL[i];
		if (i != outAxisSL.size() - 1) {
			outfileSL << "\t";
		}
	}

	for (size_t i = 0; i < outAxisSD.size(); ++i) {
		outfileSD << outAxisSD[i];
		if (i != outAxisSD.size() - 1) {
			outfileSD << "\t";
		}
	}

	outfileM << endl;
	outfileL << endl;
	outfileSL << endl;
	outfileSD << endl;

	outfileM.close();
	outfileL.close();
	outfileSL.close();
	outfileSD.close();

#ifdef OUTPUTTIME
	time2 = clock();
	cout << "Total : " << (float)(time2 - time0) / CLOCKS_PER_SEC << " s" << endl;
	system("pause");
#endif // OUTPUTTIME

	return 0;
}