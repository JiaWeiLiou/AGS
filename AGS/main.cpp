// CombineSegmentation.cpp : �w�q�D���x���ε{�����i�J�I�C
//

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

	/*�]�w��X���W*/

	int pos1 = infile.find_last_of('/\\');
	int pos2 = infile.find_last_of('.');
	string filepath(infile.substr(0, pos1));							//�ɮ׸��|
	string infilename(infile.substr(pos1 + 1, pos2 - pos1 - 1));		//�ɮצW��

	/****��¦�v���]�w****/

	/*���J��l�v��*/

	time_t time = clock();

	Mat image = imread(infile);			//��l�v��(8UC1 || 8UC3 )
	if (!image.data) { printf("Oh�Ano�AŪ��image���~~�I \n"); return false; }

	int imageMinLength = image.rows < image.cols ? image.rows : image.cols;

	/*�ഫ��l�v�����Ƕ��v��*/

	Mat gray;			//�Ƕ��v��(8UC1)

	cvtColor(image, gray, CV_BGR2GRAY);

#ifdef OUTPUTIMG
	Mat gray_R;			//��X��(8UC3)
	DrawColorBar(gray, gray_R);

	string gray_G_file = filepath + "\\" + infilename + "_0.0_GRAY(G).png";			//�ഫ��l�v�����Ƕ��v��(�Ƕ�)
	imwrite(gray_G_file, gray);
	string gray_R_file = filepath + "\\" + infilename + "_0.1_GRAY(R).png";			//�ഫ��l�v�����Ƕ��v��(����)
	imwrite(gray_R_file, gray_R);
#endif // OUTPUTIMG

	/****��󭱪��v���Ѩ�****/

	/*�ҽk�Ƕ��v��*/

	int ksize = ceil((double)imageMinLength / 10.0);
	ksize = ksize % 2 ? ksize : ksize + 1;
	double sigma = ksize / 5;

	Mat grayBlur;			//�ҽk�Ƕ��v��(8UC1)
	GaussianBlur(gray, grayBlur, Size(ksize, ksize), sigma, sigma);

#ifdef OUTPUTIMG
	Mat grayBlur_R;			//��X��(8UC3)
	DrawColorBar(grayBlur, grayBlur_R);

	string grayBlur_G_file = filepath + "\\" + infilename + "_1.0_BLUR_I(G).png";			//�ҽk�Ƕ��v��(�Ƕ�)
	imwrite(grayBlur_G_file, grayBlur);
	string grayBlur_R_file = filepath + "\\" + infilename + "_1.1_BLUR_I(R).png";			//�ҽk�Ƕ��v��(����)
	imwrite(grayBlur_R_file, grayBlur_R);
#endif // OUTPUTIMG

	/*�h���ϰ�G��*/

	Mat grayDIV;			//�����Ƕ��v���ϰ�G��(8UC1)
	DivideArea(gray, grayBlur, grayDIV);

#ifdef OUTPUTIMG
	Mat grayDIV_R;			//��X��(8UC3)
	DrawColorBar(grayDIV, grayDIV_R);

	string grayDIV_G_file = filepath + "\\" + infilename + "_2.0_DIV_I(G).png";			//�����Ƕ��v���ϰ�G��(�Ƕ�)
	imwrite(grayDIV_G_file, grayDIV);
	string grayDIV_R_file = filepath + "\\" + infilename + "_2.1_DIV_I(R).png";			//�����Ƕ��v���ϰ�G��(����)
	imwrite(grayDIV_R_file, grayDIV_R);
#endif // OUTPUTIMG

	/*�G�ȤƦǶ��v��*/

	Mat grayTH;			//�G�ȤƦǶ��v��(8UC1(BW))
	OtsuThreshold(grayDIV, grayTH);

#ifdef OUTPUTIMG
	string grayTH_B_file = filepath + "\\" + infilename + "_3.0_TH_I(B).png";			//�G�ȤƦǶ��v��(�G��)
	imwrite(grayTH_B_file, grayTH);
#endif // OUTPUTIMG

	/*��󭱪����ε��G*/

	Mat area = grayTH;			//��󭱪����ε��G(8UC1(BW))

#ifdef OUTPUTIMG
	Mat area_L, area_I;			//��X��(8UC3�B8UC3)
	DrawLabel(area, area_L);
	DrawImage(area, image, area_I);

	string area_B_file = filepath + "\\" + infilename + "_4.0_AREA(B).png";			//��󭱪����ε��G(�G��)
	imwrite(area_B_file, area);
	string area_L_file = filepath + "\\" + infilename + "_4.1_AREA(L).png";			//��󭱪����ε��G(����)
	imwrite(area_L_file, area_L);
	string area_I_file = filepath + "\\" + infilename + "_4.2_AREA(I).png";			//��󭱪����ε��G(�|��)
	imwrite(area_I_file, area_I);
#endif // OUTPUTIMG


	/****���u���v���Ѩ�****/

	/*�p��v�����*/

	Mat gradm;			//��״T��(8UC1)
	Gradient(gray, gradm);

#ifdef OUTPUTIMG
	Mat gradm_G, gradm_R;			//��X��(8UC1�B8UC3)
	DrawGrayBar(gradm, gradm_G);
	DrawColorBar(gradm, gradm_R);;

	string gradm_G_file = filepath + "\\" + infilename + "_5.0_GRAD_M(G).png";			//��״T��(�Ƕ�)
	imwrite(gradm_G_file, gradm_G);
	string gradm_R_file = filepath + "\\" + infilename + "_5.1_GRAD_M(R).png";			//��״T��(����)
	imwrite(gradm_R_file, gradm_R);
#endif // OUTPUTIMG

	/*�ҽk��״T��*/

	Mat gradmBlur;			//�ҽk��״T��(8UC1)	
	GaussianBlur(gradm, gradmBlur, Size(5, 5), 1, 1);

#ifdef OUTPUTIMG
	Mat gradmBlur_R;			//��X��(8UC3)
	DrawColorBar(gradmBlur, gradmBlur_R);

	string gradmBlur_G_file = filepath + "\\" + infilename + "_6.0_BLUR_M(G).png";			//�ҽk��״T��(�Ƕ�)
	imwrite(gradmBlur_G_file, gradmBlur);
	string gradmBlur_R_file = filepath + "\\" + infilename + "_6.1_BLUR_R(G).png";			//�ҽk��״T��(����)
	imwrite(gradmBlur_R_file, gradmBlur_R);
#endif // OUTPUTIMG

	/*������״T�Ȱϰ�G��*/

	Mat gradmDIV;			//������״T�Ȱϰ�G��(8UC1)
	DivideLine(gradm, gradmBlur, gradmDIV);

#ifdef OUTPUTIMG
	Mat gradmDIV_G, gradmDIV_R;			//��X��(8UC1�B8UC3)
	DrawGrayBar(gradmDIV, gradmDIV_G);
	DrawColorBar(gradmDIV, gradmDIV_R);

	string gradmDIV_G_file = filepath + "\\" + infilename + "_7.0_DIV_M(G).png";		//������״T�Ȱϰ�G��(�Ƕ�)
	imwrite(gradmDIV_G_file, gradmDIV_G);
	string gradmDIV_R_file = filepath + "\\" + infilename + "_7.1_DIV_M(R).png";		//������״T�Ȱϰ�G��(����)
	imwrite(gradmDIV_R_file, gradmDIV_R);
#endif // OUTPUTIMG

	/*�G�ȤƱ�״T��*/

	Mat gradmHT;			//�G�ȤƱ�״T��(8UC1(BW))
	threshold(gradmDIV, gradmHT, 1, 255, THRESH_BINARY);

#ifdef OUTPUTIMG
	string gradmHT_B_file = filepath + "\\" + infilename + "_8.0_HT_M(B).png";			//�G�ȤƱ�״T��(�G��)
	imwrite(gradmHT_B_file, gradmHT);
#endif // OUTPUTIMG

	/*������νu*/

	Mat lineHC;			//������νu(8UC1(BW))
	HysteresisCut(gradmHT, area, lineHC);

#ifdef OUTPUTIMG
	string lineHC_B_file = filepath + "\\" + infilename + "_9.0_HC_L(B).png";			//������νu(�G��)
	imwrite(lineHC_B_file, lineHC);
#endif // OUTPUTIMG

	/*���u�����ε��G*/

	Mat line;			//���u�����ε��G(8UC1(BW))
	ReverseBinary(lineHC, line);

#ifdef OUTPUTIMG
	Mat line_L, line_I;			//��X��(8UC3�B8UC3)
	DrawLabel(line, line_L);
	DrawImage(line, image, line_I);

	string line_B_file = filepath + "\\" + infilename + "_10.0_LINE(B).png";			//���u�����ε��G(�G��)
	imwrite(line_B_file, line);
	string line_L_file = filepath + "\\" + infilename + "_10.1_LINE(L).png";			//���u�����ε��G(����)
	imwrite(line_L_file, line_L);
	string line_I_file = filepath + "\\" + infilename + "_10.2_LINE(I).png";			//���u�����ε��G(�|��)
	imwrite(line_I_file, line_I);
#endif // OUTPUTIMG

	/****���X���P�u���Ѩ����G****/

	/*���X���P�u*/

	Mat objectCOM;			//���X���P�u(8UC1(BW))
	Combine(area, line, objectCOM);

#ifdef OUTPUTIMG
	Mat objectCOM_L, objectCOM_I;			//��X��(8UC3�B8UC3)
	DrawLabel(objectCOM, objectCOM_L);
	DrawImage(objectCOM, image, objectCOM_I);

	string  objectCOM_B_file = filepath + "\\" + infilename + "_11.0_COM_O(B).png";			//���X���P�u(�G��)
	imwrite(objectCOM_B_file, objectCOM);
	string  objectCOM_L_file = filepath + "\\" + infilename + "_11.1_COM_O(L).png";			//���X���P�u(����)
	imwrite(objectCOM_L_file, objectCOM_L);
	string  objectCOM_I_file = filepath + "\\" + infilename + "_11.2_COM_O(I).png";			//���X���P�u(�|��)
	imwrite(objectCOM_I_file, objectCOM_I);
#endif // OUTPUTIMG

	/*�}�B��*/

	Mat objectOpen;			//�}�B��(8UC1(BW))

	Mat elementO = (Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	morphologyEx(objectCOM, objectOpen, MORPH_OPEN, elementO);

#ifdef OUTPUTIMG
	Mat objectOpen_L, objectOpen_I;			//��X��(8UC3�B8UC3)
	DrawLabel(objectOpen, objectOpen_L);
	DrawImage(objectOpen, image, objectOpen_I);

	string  objectOpen_B_file = filepath + "\\" + infilename + "_12.0_OPEN_O(B).png";			//�}�B��(�G��)
	imwrite(objectOpen_B_file, objectOpen);
	string  objectOpen_L_file = filepath + "\\" + infilename + "_12.1_OPEN_O(L).png";			//�}�B��(����)
	imwrite(objectOpen_L_file, objectOpen_L);
	string  objectOpen_I_file = filepath + "\\" + infilename + "_12.2_OPEN_O(I).png";			//�}�B��(�|��)
	imwrite(objectOpen_I_file, objectOpen_I);
#endif // OUTPUTIMG

	/*�M�����T*/

	Mat objectCN;			//�M�����T(8UC1(BW))
	ClearNoise(objectOpen, objectCN);

#ifdef OUTPUTIMG
	Mat objectCN_L, objectCN_I;			//��X��(8UC3�B8UC3)
	DrawLabel(objectCN, objectCN_L);
	DrawImage(objectCN, image, objectCN_I);

	string  objectCN_B_file = filepath + "\\" + infilename + "_13.0_CN_O(B).png";			//�M�����T(�G��)
	imwrite(objectCN_B_file, objectCN);
	string  objectCN_L_file = filepath + "\\" + infilename + "_13.1_CN_O(L).png";			//�M�����T(����)
	imwrite(objectCN_L_file, objectCN_L);
	string  objectCN_I_file = filepath + "\\" + infilename + "_13.2_CN_O(I).png";			//�M�����T(�|��)
	imwrite(objectCN_I_file, objectCN_I);
#endif // OUTPUTIMG

	/*�Z���ഫ*/

	Mat objectDT;		//�Z���ഫ(32FC1)
	distanceTransform(objectCN, objectDT, CV_DIST_L2, 3);

#ifdef OUTPUTIMG
	Mat objectDT_G, objectDT_R;		//��X��(8UC1�B8UC3)
	DrawGrayBar(objectDT, objectDT_G);
	DrawColorBar(objectDT, objectDT_R);

	string objectDT_G_file = filepath + "\\" + infilename + "_14.0_DT_O(G).png";			//�Z���ഫ(�Ƕ�)
	imwrite(objectDT_G_file, objectDT_G);
	string objectDT_R_file = filepath + "\\" + infilename + "_14.1_DT_O(R).png";			//�Z���ഫ(����)
	imwrite(objectDT_R_file, objectDT_R);
#endif // OUTPUTIMG

	/*�D���X�i�ϰ�̤p��*/

	Mat objectEM;		//�D���X�i�ϰ�̤p��(8UC1(BW))
	ExtendLocalMinimaDetection(objectDT, objectEM, 3);

#ifdef OUTPUTIMG
	Mat objectEM_S;		//��X��(8UC1)
	DrawSeed(objectFH, objectEM, objectEM_S);

	string  objectEM_S_file = filepath + "\\" + infilename + "_15.0_EM_O(S).png";			//�D���X�i�ϰ�̤p��(�ؤl)
	imwrite(objectEM_S_file, objectEM_S);
#endif // OUTPUTIMG

	/*�Z���֭�*/

	Mat objectDC;		//�Z���֭�(8UC1(BW))
	DistanceCut(objectDT, objectDC);
	addWeighted(objectEM, 1, objectDC, 1, 0, objectDC);

#ifdef OUTPUTIMG
	Mat objectDC_S;		//��X��(8UC1)
	DrawSeed(objectFH, objectDC, objectDC_S);

	string  objectDC_S_file = filepath + "\\" + infilename + "_16.0_DC_O(S).png";			//�Z���֭�(�ؤl)
	imwrite(objectDC_S_file, objectDC_S);
#endif // OUTPUTIMG

	/*�W�[���аO������*/

	Mat objectAL;		//�W�[���аO������(8UC1(BW))
	AddLabel(objectFH, objectDC, objectAL);

#ifdef OUTPUTIMG
	Mat objectAL_S;		//��X��(8UC1)
	DrawSeed(objectFH, objectAL, objectAL_S);

	string  objectAL_S_file = filepath + "\\" + infilename + "_17.0_AL_O(S).png";			//�W�[���аO������(�ؤl)
	imwrite(objectAL_S_file, objectAL_S);
#endif // OUTPUTIMG

	/*�[�`�C�ڰ�*/

	Mat objectIM;		//�[�`�C�ڰ�(32FC1)
	ImposeMinima(objectDT, objectAL, objectIM);

#ifdef OUTPUTIMG
	Mat objectIM_G, objectIM_R;		//��X��(8UC1�B8UC3)
	DrawGrayBar(objectIM, objectIM_G);
	DrawColorBar(objectIM, objectIM_R);

	string objectIM_G_file = filepath + "\\" + infilename + "_18.0_IM_O(G).png";			//�[�`�C�ڰ�(�Ƕ�)
	imwrite(objectIM_G_file, objectIM_G);
	string objectIM_R_file = filepath + "\\" + infilename + "_18.1_IM_O(R).png";			//�[�`�C�ڰ�(����)
	imwrite(objectIM_R_file, objectIM_R);
#endif // OUTPUTIMG

	/*�������t��k*/

	Mat objectWT;		//�������t��k(8UC1(BW))
	WatershedTransform(objectFH, objectIM, objectWT);

#ifdef OUTPUTIMG
	Mat objectWT_L, objectWT_I;		//��X��(8UC3�B8UC3)
	DrawLabel(objectWT, objectWT_L);
	DrawImage(objectWT, image, objectWT_I);

	string  objectWT_B_file = filepath + "\\" + infilename + "_19.0_WT_O(B).png";			//�������t��k(�G��)
	imwrite(objectWT_B_file, objectWT);
	string  objectWT_L_file = filepath + "\\" + infilename + "_19.1_WT_O(L).png";			//�������t��k(����)
	imwrite(objectWT_L_file, objectWT_L);
	string  objectWT_I_file = filepath + "\\" + infilename + "_19.2_WT_O(I).png";			//�������t��k(�|��)
	imwrite(objectWT_I_file, objectWT_I);
#endif // OUTPUTIMG

	/*�R����ɪ���*/

	Mat objectDE;		//�R����ɪ���(3SC1(BW))
	DeleteEdge(objectWT, objectDE);

#ifdef OUTPUTIMG
	Mat objectDE_L, objectDE_I;		//��X��(8UC3�B8UC3)
	DrawLabel(objectDE, objectDE_L);
	DrawImage(objectDE, image, objectDE_I);

	string  objectDE_B_file = filepath + "\\" + infilename + "_20.0_DE_O(B).png";			//�R����ɪ���(�G��)
	imwrite(objectDE_B_file, objectDE);
	string  objectDE_L_file = filepath + "\\" + infilename + "_20.1_DE_O(L).png";			//�R����ɪ���(����)
	imwrite(objectDE_L_file, objectDE_L);
	string  objectDE_I_file = filepath + "\\" + infilename + "_20.2_DE_O(I).png";			//�R����ɪ���(�|��)
	imwrite(objectDE_I_file, objectDE_I);
#endif // OUTPUTIMG

	/*���X���*/

	Mat objectFE;		//���X���(8UC1)
	vector<Size2f> ellipse = DrawEllipse(objectED, objectFE);

#ifdef OUTPUTIMG
	string  objectFE_B_file = filepath + "\\" + infilename + "_21.0_FE_O(B).png";			//���X���(�G��)
	imwrite(objectFE_B_file, objectFE);
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