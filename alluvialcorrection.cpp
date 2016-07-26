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

#include "alluvialcorrection.h"
#include <iomanip>      // std::setprecision

void AlluvialCorrection :: dilateFan() {
    StructuringElementType structuringElement;
    structuringElement.SetRadius (30);
    structuringElement.CreateStructuringElement();

    DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
    binaryDilate -> SetKernel ( structuringElement);

    ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
    thresholder->SetInput( groupImage  );
    BinImageType :: Pointer tmpImage = BinImageType :: New();
    tmpImage->SetRegions( fanReader->GetOutput()->GetRequestedRegion() );
    tmpImage->CopyInformation(fanReader->GetOutput() );
    tmpImage->Allocate();

    thresholder->SetOutsideValue( 0 );
    thresholder->SetInsideValue( 1 );

    for (register int i = 1; i <id; i++)  {

        //these should have both the fan id value
        thresholder->SetLowerThreshold( i);
        thresholder->SetUpperThreshold( i );

        binaryDilate->SetInput( thresholder->GetOutput() );
        binaryDilate->SetDilateValue( 1 );
        binaryDilate->Update();

        tmpImage = binaryDilate->GetOutput();
        tmpImage->SetRegions( fanReader->GetOutput()->GetRequestedRegion() );
        tmpImage->CopyInformation(fanReader->GetOutput());
        tmpImage->Update();

        BinNeighborhoodIteratorType  dilIt (radius, tmpImage, *innerAreaIt);
        //dilIt.SetRegion(fanReader->GetOutput()->GetRequestedRegion() );

        playaIt->GoToBegin();
        dilIt.SetLocation( playaIt->GetIndex() );
        long double min = mean[ i ] - 1.0*std [ i ];
        long double max = mean[ i ] + 1.0*std [ i ];
        for ( playaIt->GoToBegin(); !playaIt->IsAtEnd();  playaIt->operator ++(),  dilIt.SetLocation(playaIt->GetIndex()) )  {
            //std :: cout << playaIt->GetIndex() <<"\t" <<dilIt.GetIndex() <<"\n";
            if ( ( playaIt->GetCenterPixel() == 1 ) && ( dilIt.GetCenterPixel() == 1 ) ) {
                idxIt->SetLocation( playaIt->GetIndex() );
                groupIt->SetLocation( playaIt->GetIndex() );
                double val = idxIt->GetCenterPixel();
                if ( ( val >= min  ) && (val <= max) ) {
                    dilIt.SetCenterPixel(2);
                    groupIt->SetCenterPixel( i );
                }
            }

        }

    //group
        /*
        BinWriterType :: Pointer dilwriter = BinWriterType :: New();
        dilwriter->SetInput(tmpImage);
        std :: stringstream outName;
        outName <<i <<".tif";
        dilwriter->SetFileName(outName.str());
        dilwriter->Update();
        */

    }

    GrayFillholeImageFilterType :: Pointer fillHole = GrayFillholeImageFilterType :: New();
    fillHole->SetInput(groupImage);
    fillHole->Update();

    GrayErodeFilterType :: Pointer erode = GrayErodeFilterType :: New();
    GrayDilateFilterType :: Pointer dilate = GrayDilateFilterType :: New();


   GrayStructuringElementType gStructuringElement;
   gStructuringElement.SetRadius (2);
   gStructuringElement.CreateStructuringElement();

    erode->SetKernel(gStructuringElement);
    erode->SetInput(fillHole->GetOutput());
    erode->Update();

    dilate->SetKernel(gStructuringElement);
    dilate->SetInput(erode->GetOutput());
    dilate->Update();

    WriterType :: Pointer imgWriter = WriterType :: New();
    imgWriter->SetInput( dilate->GetOutput() );
    imgWriter->SetFileName( "finalFans.tif" );
    imgWriter->Update();

    VectorizationFilterType::Pointer vectorFilter = VectorizationFilterType :: New();
    vectorFilter->SetInput(dilate->GetOutput());

    VectorDataWriterType::Pointer vectorWriter = VectorDataWriterType :: New();
    vectorWriter->SetInput(vectorFilter->GetOutput() );
    vectorWriter->SetFileName("finalFans.shp");
    vectorWriter->Update();



}


void AlluvialCorrection ::group(int* cnt, long double* mn, long double* std) {

    groupIt->SetCenterPixel(id);
    (*cnt)++;
    (*mn) += idxIt->GetCenterPixel();
    (*std) += std :: pow (idxIt->GetCenterPixel(), 2 );

    NeighborhoodIteratorType :: IndexType * tmpIdx;
    tmpIdx = new NeighborhoodIteratorType :: IndexType( groupIt->GetIndex() );

    int *i;
    i  = new int(0);
    for ((*i) = 0; *i <9; (*i)++) {
        if  ( (fanIt->IndexInBounds(*i)   ) && ( fanIt->GetPixel(*i) == 1  ) && (groupIt->GetPixel(*i) == 0 ) ) {

            groupIt->SetLocation( fanIt->GetIndex(*i) );
            idxIt->SetLocation( fanIt->GetIndex(*i) );
            fanIt->SetLocation( fanIt->GetIndex(*i) );

            group(cnt, mn, std);
            fanIt->SetLocation(*tmpIdx);
            groupIt->SetLocation(*tmpIdx);
            idxIt->SetLocation(*tmpIdx);
        }
    }
    delete tmpIdx;
    tmpIdx = NULL;
    delete i;
    i = NULL;

}

void AlluvialCorrection :: groupFan() {

    for ( fanIt->GoToBegin(), groupIt->GoToBegin(); !fanIt->IsAtEnd(); fanIt->operator ++(), groupIt->operator ++() ) {
        if ( ( fanIt->GetCenterPixel() == 1 ) && (groupIt->GetCenterPixel() == 0 ) ) {
            idxIt->SetLocation( fanIt->GetIndex() );
            long double mn = 0, sd = 0;
            int cnt = 0;
            group(&cnt, &mn, &sd);
            mean[id] = mn/cnt;
            std[id] =  std :: sqrt( ( sd/cnt - std :: pow( mean[id] , 2)  ) );
            //std :: cout <<id <<"\t" << mean[id] <<"\t"<< std[id] <<"\n";


            id++;
        }
    }
}



AlluvialCorrection::AlluvialCorrection()
{

}

AlluvialCorrection :: AlluvialCorrection(std :: string inFn, std :: string inIdx, std :: string ply): id(1) {
    fanReader         =     ReaderType :: New();
    indexReader     =     ReaderType :: New();
    playaReader     =     ReaderType :: New();

    fanReader->SetFileName(inFn);
    indexReader->SetFileName(inIdx);
    playaReader->SetFileName(ply);

    fanReader->Update();
    indexReader->Update();
    playaReader->Update();

    groupImage = BinImageType :: New();
    groupImage->CopyInformation( fanReader->GetOutput() );
    groupImage->SetRegions(fanReader->GetOutput()->GetRequestedRegion());
    groupImage->Allocate();
    groupImage->FillBuffer(0);




    radius.Fill(1);

    FaceCalculatorType faceCalculator;
    FaceCalculatorType::FaceListType faceList;

    faceList = faceCalculator(fanReader->GetOutput(), fanReader->GetOutput()->GetLargestPossibleRegion(), radius);
    innerAreaIt = faceList.begin();

    fanIt      =    new NeighborhoodIteratorType(radius, fanReader->GetOutput(), *innerAreaIt);
    idxIt       =   new NeighborhoodIteratorType(radius, indexReader->GetOutput(), *innerAreaIt);
    playaIt   =    new NeighborhoodIteratorType(radius, playaReader->GetOutput(), *innerAreaIt);
    groupIt  =    new NeighborhoodIteratorType(radius, groupImage, *innerAreaIt);

}

AlluvialCorrection :: ~AlluvialCorrection() {
    delete fanIt;
    fanIt = NULL;

    delete idxIt;
    idxIt = NULL;

    delete playaIt;
    playaIt = NULL;

    delete groupIt;
    groupIt = NULL;

}
