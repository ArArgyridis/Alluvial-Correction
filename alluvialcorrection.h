/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ALLUVIALCORRECTION_H
#define ALLUVIALCORRECTION_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <sstream>

#include <itkImageRegionIterator.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkNeighborhoodAlgorithm.h>
#include <itkNeighborhoodIterator.h>
#include  <itkGrayscaleErodeImageFilter.h>
#include  <itkGrayscaleDilateImageFilter.h>

#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkGrayscaleFillholeImageFilter.h>

#include <otbImage.h>
#include <otbImageFileReader.h>
#include <otbImageFileWriter.h>
#include <otbImportImageFilter.h>
#include <otbLabelImageToVectorDataFilter.h>
#include <otbVectorData.h>
#include <otbVectorDataFileWriter.h>

//for double images
typedef double PixelType;
typedef otb :: Image<PixelType, 2> ImageType;
typedef ImageType :: IndexType IndexType;
typedef otb :: ImageFileReader<ImageType> ReaderType;
typedef otb :: ImageFileWriter<ImageType> WriterType;
typedef itk :: ImageRegionIterator <ImageType> IteratorType;
typedef itk :: ImageRegionConstIterator < ImageType > ConstIteratorType;
typedef itk::NeighborhoodIterator<ImageType> NeighborhoodIteratorType;
typedef itk :: ImageRegionIterator < ImageType >  IteratorType;

//for int images
typedef int IntPixelType;
typedef otb :: Image<PixelType, 2> IntImageType;
typedef IntImageType :: IndexType IntIndexType;
typedef otb :: ImageFileReader<IntImageType> IntReaderType;
typedef otb :: ImageFileWriter<IntImageType> IntWriterType;
typedef itk :: ImageRegionIterator <IntImageType> IntIteratorType;
typedef itk :: ImageRegionConstIterator < IntImageType > IntConstIteratorType;
typedef itk::NeighborhoodIterator<IntImageType> IntNeighborhoodIteratorType;
typedef itk :: ImageRegionIterator < IntImageType >  IntIteratorType;

//for binary images

//for int images
typedef unsigned char BinPixelType;
typedef otb :: Image<PixelType, 2> BinImageType;
typedef BinImageType :: IndexType BinIndexType;
typedef otb :: ImageFileReader<BinImageType> BinReaderType;
typedef otb :: ImageFileWriter<BinImageType> BinWriterType;
typedef itk :: ImageRegionIterator <BinImageType> BinIteratorType;
typedef itk :: ImageRegionConstIterator < BinImageType > BinConstIteratorType;
typedef itk::NeighborhoodIterator<BinImageType> BinNeighborhoodIteratorType;
typedef itk :: ImageRegionIterator < BinImageType >  BinIteratorType;


const unsigned int Dimension = 2;
typedef itk :: BinaryBallStructuringElement <BinPixelType,  Dimension> StructuringElementType;

typedef itk :: GrayscaleFillholeImageFilter<BinImageType, BinImageType> GrayFillholeImageFilterType;

typedef itk::NeighborhoodAlgorithm :: ImageBoundaryFacesCalculator<ImageType> FaceCalculatorType;

typedef itk :: BinaryDilateImageFilter <BinImageType, BinImageType, StructuringElementType > DilateFilterType;


typedef itk :: BinaryBallStructuringElement <PixelType,  Dimension> GrayStructuringElementType;
typedef itk :: GrayscaleDilateImageFilter <ImageType, ImageType, GrayStructuringElementType> GrayDilateFilterType;
typedef itk :: GrayscaleErodeImageFilter <ImageType, ImageType, GrayStructuringElementType> GrayErodeFilterType;

typedef itk::BinaryThresholdImageFilter<ImageType, BinImageType> ThresholdFilterType;

typedef otb::LabelImageToVectorDataFilter<BinImageType> VectorizationFilterType;

typedef otb::VectorData<double,2> VectorDataType;
typedef otb::VectorDataFileWriter<VectorDataType> VectorDataWriterType;



class AlluvialCorrection {
    FaceCalculatorType::FaceListType::iterator innerAreaIt;

    ImageType :: Pointer groupImage;

    std :: map <int, long  double> mean, std;

    NeighborhoodIteratorType *fanIt, *idxIt, *playaIt, *groupIt;

    NeighborhoodIteratorType :: RadiusType radius;

    ReaderType :: Pointer fanReader, indexReader, playaReader;

    int id;

    void group(int*, long double*, long double*);


public:
    void dilateFan();
    void groupFan();

    AlluvialCorrection();
    AlluvialCorrection( std :: string, std :: string, std :: string);
    ~AlluvialCorrection();

};

#endif // ALLUVIALCORRECTION_H
