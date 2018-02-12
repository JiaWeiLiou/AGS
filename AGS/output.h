/******************************************************************************
FileName     [ output.h ]
PackageName  [ AGS ]
Synopsis     [ Output Special Image Processing ]
Author       [ Jia-Wei (Jimmy) Liou ]
Copyright    [ Copyleft(c) 2018-present LaDF, CE-Hydrolic, NTU, Taiwan ]
******************************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <queue>
#include <cmath>

using namespace std;
using namespace cv;

//Find Root
//labeltable : labels table
//label : label value for look up
//return : the root of the label
int findroot(int labeltable[], int label);

//Find Connect Object 
//binary : binary image(8UC1)
//labelImg : labels image(32SC1)
//nears : nears type(4/6/8)
//return : labels total number without 0(background)
int bwlabel(InputArray _binary, OutputArray _labelImg, int nears);

//Create Color Bar
//colorbar : color bar index
void makecolorbar(vector<Scalar> &colorbar);

//Auto Stretch and Show Gray Bar Color
//gray : gray image(8UC1/32FC1)
//colorbarImg : color bar image(8UC1)
void DrawGrayBar(InputArray _gray, OutputArray _graybarImg);

//Auto Stretch and Show Color Bar Color
//gray : gray image(8UC1/16SC1/32FC1)
//colorbarImg : colorbar image(8UC3)
void DrawColorBar(InputArray _gray, OutputArray _colorbarImg);

//Draw Binary Image with Random Color Labels
//binary : binary image(8UC1(BW))
//labelImg : labels image(8UC3)
void DrawLabel(InputArray _binary, OutputArray _labelImg, int num = 4);

//Combine Binary Image and Raw Image
//binary : binary image(8UC1(BW))
//image : raw image(8UC1/8UC3)
//combine : combine image(8UC3)
void DrawImage(InputArray _binary, InputArray _image, OutputArray _combineImg);

//Combine Binary Image and Seed Image
//binary : binary image(8UC1(BW))
//seed : seed image(8UC1(BW))
//combine : combine image(8UC1)
void DrawSeed(InputArray _binary, InputArray _seed, OutputArray _combineImg);

//Fitting and Draw Ellipse
//object : object image(8UC1(BW))
//ellipse : ellipse image(8UC1(BW))
//return : long and short axis of ellipse
vector<Size2f> DrawEllipse(InputArray _object, OutputArray _ellipseImg);

#endif // OUTPUT_H