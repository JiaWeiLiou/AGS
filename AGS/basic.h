/******************************************************************************
FileName     [ basic.h ]
PackageName  [ AGS ]
Synopsis     [ Automatic Grain Sizing ]
Author       [ Jia-Wei (Jimmy) Liou ]
Copyright    [ Copyleft(c) 2018-present LaDF, CE-Hydrolic, NTU, Taiwan ]
******************************************************************************/
#ifndef BASIC_H
#define BASIC_H

#include "output.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//Remove Ambient Light for Area
//gray : gray image (8UC1)
//blur : blur image (8UC1)
//divide : remove ambient light image (8UC1)
void DivideArea(InputArray _gray, InputArray _blur, OutputArray _divide);

//Otsu Threshold
//gray : gray image (8UC1)
//binary : binary image (8UC1(BW))
void OtsuThreshold(InputArray _gray, OutputArray _binary);

//Calculate Image Gradient
//gray : gray image(8UC1)
//gradient : gradient image(8UC1)
void Gradient(InputArray _gray, OutputArray _gradient);

//Remove Ambient Light for Line
//gradient : gradient image(8UC1)
//blur : blur gradient image (8UC1)
//divide : remove ambient light gradient image (8UC1)
void DivideLine(InputArray _gradient, InputArray _blur, OutputArray _divide);

//Hysteresis Cut Binary Image to Line by Area
//binary : binary image (8UC1(BW))
//area : area binary image (8UC1(BW))
//line : line binary image (8UC1(BW))
void HysteresisCut(InputArray _binary, InputArray _area, OutputArray _line);

//Reverse Binary Image
//binary : binary image (8UC1(BW))
//rbinary : reversed binary image (8UC1(BW))
void ReverseBinary(InputArray _binary, OutputArray _rbinary);

//Combine Area and Line Extraction
//area : area binary image (8UC1(BW))
//line : line binary image (8UC1(BW))
//binary : combine binary image (8UC1(BW))
void Combine(InputArray _area, InputArray _line, OutputArray _binary);

//Clear Black Noise
//binary : binary image (8UC1(BW))
//clear : clear noise image (8UC1(BW))
void ClearNoise(InputArray _binary, OutputArray _clear);



//Delete Edge object
//binary : binary image (8UC1(BW))
//object : object image (8UC1(BW))
void DeleteEdge(InputArray _binary, OutputArray _object);

#endif // BASIC_H