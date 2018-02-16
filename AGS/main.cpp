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

#define OUTPUTIMG

int main()
{
	std::cout << "Please enter image path : ";
	string infile;
	std::cin >> infile;

	/* Set Output File Name */

	int pos1 = infile.find_last_of('/\\');
	int pos2 = infile.find_last_of('.');
	string filepath(infile.substr(0, pos1));							//file path
	string infilename(infile.substr(pos1 + 1, pos2 - pos1 - 1));		//file name

	time_t time = clock();

	/**** Image Pre-Processing ****/

	/*Read Raw Image*/

	Mat image = cv::imread(infile);			//raw image (8UC3)
	if (!image.data) { 
		printf("Oh no! reading image error... \n"); 
		return false; 
	}

	int imageMinLength = image.rows < image.cols ? image.rows : image.cols;

	/* Convert RGB Image to Gray */

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

	/**** Area-Based Image Extraction ****/

	/* Bluring Image */

	int ksize = ceil((double)imageMinLength / 10.0);
	ksize = ksize % 2 ? ksize : ksize + 1;
	double sigma = ksize / 5;

	Mat grayBlur;			//8UC1
	cv::GaussianBlur(gray, grayBlur, Size(ksize, ksize), sigma, sigma);

#ifdef OUTPUTIMG
	Mat grayBlur_C;			//output(8UC3)
	DrawColorBar(grayBlur, grayBlur_C);

	string grayBlur_G_file = filepath + "\\" + infilename + "_1.0_BLUR_I(G).png";			//Gray
	cv::imwrite(grayBlur_G_file, grayBlur);
	string grayBlur_C_file = filepath + "\\" + infilename + "_1.1_BLUR_I(C).png";			//Color
	cv::imwrite(grayBlur_C_file, grayBlur_C);
#endif // OUTPUTIMG

	/* Remove Ambient Light for Area */

	Mat grayDIV;			//8UC1
	DivideArea(gray, grayBlur, grayDIV);

#ifdef OUTPUTIMG
	Mat grayDIV_C;			//output(8UC3)
	DrawColorBar(grayDIV, grayDIV_C);

	string grayDIV_G_file = filepath + "\\" + infilename + "_2.0_DIV_I(G).png";			//Gray
	cv::imwrite(grayDIV_G_file, grayDIV);
	string grayDIV_C_file = filepath + "\\" + infilename + "_2.1_DIV_I(C).png";			//Color
	cv::imwrite(grayDIV_C_file, grayDIV_C);
#endif // OUTPUTIMG

	/* Otsu Threshold */

	Mat grayTH;			//8UC1(BW)
	OtsuThreshold(grayDIV, grayTH);

#ifdef OUTPUTIMG
	string grayTH_B_file = filepath + "\\" + infilename + "_3.0_TH_I(B).png";			//Binary
	cv::imwrite(grayTH_B_file, grayTH);
#endif // OUTPUTIMG

	/* Area */

	Mat area = grayTH;			//8UC1(BW)

#ifdef OUTPUTIMG
	Mat area_L, area_I;			//output(8UC3¡B8UC3)
	DrawLabel(area, area_L);
	DrawImage(area, image, area_I);

	string area_B_file = filepath + "\\" + infilename + "_4.0_AREA(B).png";			//Binary
	cv::imwrite(area_B_file, area);
	string area_L_file = filepath + "\\" + infilename + "_4.1_AREA(L).png";			//Labels
	cv::imwrite(area_L_file, area_L);
	string area_I_file = filepath + "\\" + infilename + "_4.2_AREA(I).png";			//Combine
	cv::imwrite(area_I_file, area_I);
#endif // OUTPUTIMG


	/**** Line-Based Image Extraction ****/

	/* Calculate Image Gradient */

	Mat gradm;			//8UC1
	Gradient(gray, gradm);

#ifdef OUTPUTIMG
	Mat gradm_G, gradm_C;			//output(8UC1¡B8UC3)
	DrawGrayBar(gradm, gradm_G);
	DrawColorBar(gradm, gradm_C);;

	string gradm_G_file = filepath + "\\" + infilename + "_5.0_GRAD_M(G).png";			//Gray
	cv::imwrite(gradm_G_file, gradm_G);
	string gradm_C_file = filepath + "\\" + infilename + "_5.1_GRAD_M(C).png";			//Color
	cv::imwrite(gradm_C_file, gradm_C);
#endif // OUTPUTIMG

	/* Bluring Image Gradient */

	Mat gradmBlur;			//8UC1
	cv::GaussianBlur(gradm, gradmBlur, Size(5, 5), 1, 1);

#ifdef OUTPUTIMG
	Mat gradmBlur_C;			//output(8UC3)
	DrawColorBar(gradmBlur, gradmBlur_C);

	string gradmBlur_G_file = filepath + "\\" + infilename + "_6.0_BLUR_M(G).png";			//Gray
	cv::imwrite(gradmBlur_G_file, gradmBlur);
	string gradmBlur_C_file = filepath + "\\" + infilename + "_6.1_BLUR_R(C).png";			//Color
	cv::imwrite(gradmBlur_C_file, gradmBlur_C);
#endif // OUTPUTIMG

	/* Remove Ambient Light for Image Gradient */

	Mat gradmDIV;			//8UC1
	DivideLine(gradm, gradmBlur, gradmDIV);

#ifdef OUTPUTIMG
	Mat gradmDIV_G, gradmDIV_C;			//output(8UC1¡B8UC3)
	DrawGrayBar(gradmDIV, gradmDIV_G);
	DrawColorBar(gradmDIV, gradmDIV_C);

	string gradmDIV_G_file = filepath + "\\" + infilename + "_7.0_DIV_M(G).png";		//Gray
	cv::imwrite(gradmDIV_G_file, gradmDIV_G);
	string gradmDIV_C_file = filepath + "\\" + infilename + "_7.1_DIV_M(C).png";		//Color
	cv::imwrite(gradmDIV_C_file, gradmDIV_C);
#endif // OUTPUTIMG

	/* Binary Image Gradient */

	Mat gradmHT;			//8UC1(BW)
	cv::threshold(gradmDIV, gradmHT, 1, 255, THRESH_BINARY);

#ifdef OUTPUTIMG
	string gradmHT_B_file = filepath + "\\" + infilename + "_8.0_HT_M(B).png";			//Binary
	cv::imwrite(gradmHT_B_file, gradmHT);
#endif // OUTPUTIMG

	/* Hysteresis Cut Binary Image to Line by Area */

	Mat lineHC;			//8UC1(BW)
	HysteresisCut(gradmHT, area, lineHC);

#ifdef OUTPUTIMG
	string lineHC_B_file = filepath + "\\" + infilename + "_9.0_HC_L(B).png";			//Binary
	cv::imwrite(lineHC_B_file, lineHC);
#endif // OUTPUTIMG

	/* Line */

	Mat line;			//8UC1(BW)
	ReverseBinary(lineHC, line);

#ifdef OUTPUTIMG
	Mat line_L, line_I;			//output(8UC3¡B8UC3)
	DrawLabel(line, line_L);
	DrawImage(line, image, line_I);

	string line_B_file = filepath + "\\" + infilename + "_10.0_LINE(B).png";			//Binary
	cv::imwrite(line_B_file, line);
	string line_L_file = filepath + "\\" + infilename + "_10.1_LINE(L).png";			//Labels
	cv::imwrite(line_L_file, line_L);
	string line_I_file = filepath + "\\" + infilename + "_10.2_LINE(I).png";			//Combine
	cv::imwrite(line_I_file, line_I);
#endif // OUTPUTIMG

	/**** Combine Image Extraction ****/

	/* Combine Area and Line */

	Mat objectCOM;			//8UC1(BW)
	Combine(area, line, objectCOM);

#ifdef OUTPUTIMG
	Mat objectCOM_L, objectCOM_I;			//output(8UC3¡B8UC3)
	DrawLabel(objectCOM, objectCOM_L);
	DrawImage(objectCOM, image, objectCOM_I);

	string  objectCOM_B_file = filepath + "\\" + infilename + "_11.0_COM_O(B).png";			//Binary
	cv::imwrite(objectCOM_B_file, objectCOM);
	string  objectCOM_L_file = filepath + "\\" + infilename + "_11.1_COM_O(L).png";			//Labels
	cv::imwrite(objectCOM_L_file, objectCOM_L);
	string  objectCOM_I_file = filepath + "\\" + infilename + "_11.2_COM_O(I).png";			//Combine
	cv::imwrite(objectCOM_I_file, objectCOM_I);
#endif // OUTPUTIMG

	/* Image morphology Opening */

	Mat objectOpen;			//8UC1(BW)

	Mat elementO = (Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	cv::morphologyEx(objectCOM, objectOpen, MORPH_OPEN, elementO);

#ifdef OUTPUTIMG
	Mat objectOpen_L, objectOpen_I;			//output(8UC3¡B8UC3)
	DrawLabel(objectOpen, objectOpen_L);
	DrawImage(objectOpen, image, objectOpen_I);

	string  objectOpen_B_file = filepath + "\\" + infilename + "_12.0_OPEN_O(B).png";			//Binary
	cv::imwrite(objectOpen_B_file, objectOpen);
	string  objectOpen_L_file = filepath + "\\" + infilename + "_12.1_OPEN_O(L).png";			//Labels
	cv::imwrite(objectOpen_L_file, objectOpen_L);
	string  objectOpen_I_file = filepath + "\\" + infilename + "_12.2_OPEN_O(I).png";			//Combine
	cv::imwrite(objectOpen_I_file, objectOpen_I);
#endif // OUTPUTIMG

	/* Clear Noise of Black Pepper */

	Mat objectCN;			//8UC1(BW)
	ClearNoise(objectOpen, objectCN);

#ifdef OUTPUTIMG
	Mat objectCN_L, objectCN_I;			//output(8UC3¡B8UC3)
	DrawLabel(objectCN, objectCN_L);
	DrawImage(objectCN, image, objectCN_I);

	string  objectCN_B_file = filepath + "\\" + infilename + "_13.0_CN_O(B).png";			//Binary
	cv::imwrite(objectCN_B_file, objectCN);
	string  objectCN_L_file = filepath + "\\" + infilename + "_13.1_CN_O(L).png";			//Labels
	cv::imwrite(objectCN_L_file, objectCN_L);
	string  objectCN_I_file = filepath + "\\" + infilename + "_13.2_CN_O(I).png";			//Combine
	cv::imwrite(objectCN_I_file, objectCN_I);
#endif // OUTPUTIMG

	/**** Watershed Algorithm ****/

	/* Distance Transform */

	Mat objectDT;		//32FC1
	cv::distanceTransform(objectCN, objectDT, CV_DIST_L2, 3);

#ifdef OUTPUTIMG
	Mat objectDT_G, objectDT_C;		//output(8UC1¡B8UC3)
	DrawGrayBar(objectDT, objectDT_G);
	DrawColorBar(objectDT, objectDT_C);

	string objectDT_G_file = filepath + "\\" + infilename + "_14.0_DT_O(G).png";			//Gray
	cv::imwrite(objectDT_G_file, objectDT_G);
	string objectDT_C_file = filepath + "\\" + infilename + "_14.1_DT_O(C).png";			//Color
	cv::imwrite(objectDT_C_file, objectDT_C);
#endif // OUTPUTIMG

	/* Extend Local Minima */

	Mat objectEM;		//8UC1(BW)
	ExtendRegionalMinima(objectDT, objectEM, 3);

#ifdef OUTPUTIMG
	Mat objectEM_S;		//output(8UC1)
	DrawSeed(objectCN, objectEM, objectEM_S);

	string  objectEM_S_file = filepath + "\\" + infilename + "_15.0_EM_O(S).png";			//Seed
	cv::imwrite(objectEM_S_file, objectEM_S);
#endif // OUTPUTIMG

	/* Add unlabeled labels */

	Mat objectAS;		//8UC1(BW)
	AddSeed(objectCN, objectEM, objectAS);

#ifdef OUTPUTIMG
	Mat objectAS_S;		//output(8UC1)
	DrawSeed(objectCN, objectAS, objectAS_S);

	string  objectAS_S_file = filepath + "\\" + infilename + "_16.0_AS_O(S).png";			//Seed
	cv::imwrite(objectAS_S_file, objectAS_S);
#endif // OUTPUTIMG

	/* Imposing Minima */

	Mat objectIM;		//32FC1
	ImposeMinima(objectDT, objectAS, objectIM);

#ifdef OUTPUTIMG
	Mat objectIM_G, objectIM_C;		//output(8UC1¡B8UC3)
	DrawGrayBar(objectIM, objectIM_G);
	DrawColorBar(objectIM, objectIM_C);

	string objectIM_G_file = filepath + "\\" + infilename + "_17.0_IM_O(G).png";			//Gray
	cv::imwrite(objectIM_G_file, objectIM_G);
	string objectIM_C_file = filepath + "\\" + infilename + "_17.1_IM_O(C).png";			//Color
	cv::imwrite(objectIM_C_file, objectIM_C);
#endif // OUTPUTIMG

	/* Watershed Segmentation */

	Mat objectWT;		//8UC1(BW)
	WatershedTransform(objectCN, objectIM, objectWT);

#ifdef OUTPUTIMG
	Mat objectWT_L, objectWT_I;		//output(8UC3¡B8UC3)
	DrawLabel(objectWT, objectWT_L);
	DrawImage(objectWT, image, objectWT_I);

	string  objectWT_B_file = filepath + "\\" + infilename + "_18.0_WT_O(B).png";			//Binary
	cv::imwrite(objectWT_B_file, objectWT);
	string  objectWT_L_file = filepath + "\\" + infilename + "_18.1_WT_O(L).png";			//Labels
	cv::imwrite(objectWT_L_file, objectWT_L);
	string  objectWT_I_file = filepath + "\\" + infilename + "_18.2_WT_O(I).png";			//Combine
	cv::imwrite(objectWT_I_file, objectWT_I);
#endif // OUTPUTIMG

	/**** Particle Post-Calculation ****/

	/* Delete Edge object */

	Mat objectDE;		//8UC1(BW)
	DeleteEdge(objectWT, objectDE);

#ifdef OUTPUTIMG
	Mat objectDE_L, objectDE_I;		//output(8UC3¡B8UC3)
	DrawLabel(objectDE, objectDE_L);
	DrawImage(objectDE, image, objectDE_I);

	string  objectDE_B_file = filepath + "\\" + infilename + "_19.0_DE_O(B).png";			//Binary
	cv::imwrite(objectDE_B_file, objectDE);
	string  objectDE_L_file = filepath + "\\" + infilename + "_19.1_DE_O(L).png";			//Labels
	cv::imwrite(objectDE_L_file, objectDE_L);
	string  objectDE_I_file = filepath + "\\" + infilename + "_19.2_DE_O(I).png";			//Combine
	cv::imwrite(objectDE_I_file, objectDE_I);
#endif // OUTPUTIMG

	/* Fitting Ellipse */

	Mat objectFE;		//8UC1(BW)
	vector<Size2f> ellipse = DrawEllipse(objectDE, objectFE);

#ifdef OUTPUTIMG
	string  objectFE_B_file = filepath + "\\" + infilename + "_20.0_FE_O(B).png";			//Binary
	cv::imwrite(objectFE_B_file, objectFE);
#endif // OUTPUTIMG

	fstream outfile;
	string outputPath = filepath + "\\" + "GrainSize.txt";
	outfile.open(outputPath, ios::out | ios::trunc);

	for (size_t i = 0; i < ellipse.size(); ++i) {
		outfile << ellipse[i].width << "\t" << ellipse[i].height << endl;
	}

	outfile.close();

	time = clock() - time;
	cout << "spend" << (float)time / CLOCKS_PER_SEC << "seconds" << endl;
	system("pause");

	return 0;
}