///////////////////////////////////////////////////////////////////////
//                Date: Fri Sep  2 14:26:48 CEST 2016                //
//               Autor: Leonid Burmistrov                            //
// Program description: Convertion of the binary data into the root  //
//                      format. Initioal binary data file produced   //
//                      by the single board (2 channels) by one of   //
//                      the first USB-WaveCatcher version            //
///////////////////////////////////////////////////////////////////////

//root
#include <TH1D.h>
#include <TStyle.h>
#include <TString.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>

//C, C++
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

const Int_t _nCh = 2;
const Int_t _USBWCnSamplesPoint = 256;
TString _softwareVersion = "????";

void convertBunchUSBWC2rootBin(TString inputDataFileList, TString inputDataFileFolder, TString outputrootFile);

int main(int argc, char *argv[]){  
  TString inputDataFileList;
  TString inputDataFileFolder;
  TString outputrootFile;
  if(argc == 4){
    inputDataFileList = argv[1];
    inputDataFileFolder = argv[2];
    outputrootFile = argv[3];
    cout<<"In data file list : "<<inputDataFileList<<endl
        <<"In data path      : "<<inputDataFileFolder<<endl
        <<"Out root file     : "<<outputrootFile<<endl;
    convertBunchUSBWC2rootBin(inputDataFileList, inputDataFileFolder, outputrootFile);
  }
  else{
    cout<<" ERROR ---> in input arguments"<<endl
	<<"   [1]   :  File containing the list of input data files"<<endl
	<<"   [2]   :  Path of the directory containing these input files"<<endl
	<<"   [3]   :  Name of the output ROOT file"<<endl;
  }  
  return 0;
}

void convertBunchUSBWC2rootBin(TString inputDataFileList, TString inputDataFileFolder, TString outputrootFile){

  FILE *pFile;
  long totFileSizeByte;
  long currentPositionByte;
  char *buffer;
  TString buffTmp;

  TString inputDataFile;

  const Int_t nChannels = _nCh;
  const Int_t nSamplesPoint = _USBWCnSamplesPoint;

  //USBWC data in one ev//
  
  float samplingPeriod;
  int FirstCellToPlot;
  int channel;
  int channelIsPresent;
  short tempBuffer;

  Float_t samplingPeriod_usbwc;
  Int_t FirstCellToPlot_usbwc;
  Int_t channel_usbwc[nChannels];
  Int_t channelIsPresent_usbwc[nChannels];
  Short_t ampl_usbwc[nChannels][nSamplesPoint];

  ///////////////////Root file with data/////////////////
  TFile *hfile = new TFile( outputrootFile, "RECREATE", "USBWC Data", 1);
  if (hfile->IsZombie()) {
    cout << "PROBLEM with the initialization of the output ROOT ntuple " 
	 << outputrootFile << ": check that the path is correct!!!"
	 << endl;
    exit(-1);
  }
  TTree *tree = new TTree("T", "USBWC Data Tree");
  hfile->SetCompressionLevel(2);
  tree->SetAutoSave(1000000);
  // Create new event
  TTree::SetBranchStyle(0);

  TString chuName_ss = "channel_usbwc[";
  chuName_ss += nChannels;
  chuName_ss += "]/I";
  cout<<"chuName_ss = "<<chuName_ss<<endl;

  TString chuisName_ss = "channelIsPresent_usbwc[";
  chuisName_ss += nChannels;
  chuisName_ss += "]/I";
  cout<<"chuisName_ss = "<<chuisName_ss<<endl;

  TString chName_ss = "ampl_usbwc[";
  chName_ss += nChannels;
  chName_ss += "][";
  chName_ss += nSamplesPoint;
  chName_ss +="]/S";
  cout<<"chName_ss = "<<chName_ss<<endl;

  tree->Branch("samplingPeriod_usbwc", &samplingPeriod_usbwc, "samplingPeriod_usbwc/F");
  tree->Branch("FirstCellToPlot_usbwc",&FirstCellToPlot_usbwc, "FirstCellToPlot_usbwc/I");
  tree->Branch("channel_usbwc",channel_usbwc, chuName_ss.Data());
  tree->Branch("channelIsPresent_usbwc",channelIsPresent_usbwc, chuisName_ss.Data());
  tree->Branch("ampl_usbwc", ampl_usbwc, chName_ss.Data());

  //assert(0);

  Int_t i = 0;
  Int_t j = 0;
  Int_t k = 0;

  string mot;
  Double_t amplVal;
  Int_t chID = 0;

  Int_t nEv = 0;
  Int_t nEv_Tot = 0;
  
  // Init event variables
  samplingPeriod_usbwc = -999.0;
  FirstCellToPlot_usbwc = -999;
  for(j = 0;j<nChannels;j++){
    channel_usbwc[j] = -999;
    channelIsPresent_usbwc[j] = -999;
    for(i = 0;i<nSamplesPoint;i++){
      ampl_usbwc[j][i] = -999.0;
    }
  }

  //ifstream indata;
  ifstream indataList;

  indataList.open(inputDataFileList.Data());
  assert(indataList.is_open());
  while (indataList >> mot ){
    inputDataFile = inputDataFileFolder;
    inputDataFile += mot;
    //cout<<indata.is_open()<<endl;
    cout<<" ---> Conversion of "<<inputDataFile<<endl;
    nEv = 0; // Event counter for a given file
    pFile = fopen(inputDataFile.Data(), "rb" );
    if (pFile==NULL) {fputs ("File error",stderr); assert(0);}  
    //cout<<" ---> File to convert : "<<inputDataFile<<endl;
    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    totFileSizeByte = ftell (pFile);
    rewind (pFile);
    //cout<<"totFileSizeByte = "<<totFileSizeByte<<endl;
    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*totFileSizeByte);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
    // copy the file into the buffer:
    //result = fread (buffer,1,lSize,pFile);
    fread (buffer,1,268,pFile);
    currentPositionByte = 268;
    while(currentPositionByte<totFileSizeByte){
      currentPositionByte += fread(&samplingPeriod,1,4,pFile);
      currentPositionByte += fread(&FirstCellToPlot,1,4,pFile);
      samplingPeriod_usbwc = samplingPeriod;
      FirstCellToPlot_usbwc = FirstCellToPlot;
      for(i = 0;i<_nCh;i++){
	currentPositionByte += fread(&channel,1,4,pFile);
	currentPositionByte += fread(&channelIsPresent,1,4,pFile);
	channel_usbwc[i] = channel;
	channelIsPresent_usbwc[i] = channelIsPresent;
	for(j = 0;j<_USBWCnSamplesPoint;j++){
	  currentPositionByte += fread(&tempBuffer,1,4,pFile);
	  ampl_usbwc[i][j] = tempBuffer;
	  //cout<<ampl_usbwc[i][j]<<endl;
	}
      }
      tree->Fill();
      //assert(0);
    }
  }
  indataList.close();
  
  /////////////////////////////
  
  hfile = tree->GetCurrentFile();
  hfile->Write();

  hfile->Close();  

}

// result = fwrite (&samplingPeriod, sizeof(float), 1, FileForSavingDataPntr);
// result = fwrite(&FirstCellToPlot,sizeof(int),1,FileForSavingDataPntr);
// for(channel = 0; channel <MAX_NB_OF_CHANNELS; channel ++){                
//    result = fwrite(&channel,sizeof(int),1,FileForSavingDataPntr);
//    result= fwrite(&channelIsPresent,sizeof(int),1, FileForSavingDataPntr);
//    for(k= 0; k<Nb_Of_Samples; k++){
//       tempBuffer[k]= (signed short)(FullyOrderedValues[channel][k]*10);
//    }
//    result= fwrite (tempBuffer, sizeof(signed short), Nb_Of_Samples,FileForSavingDataPntr);
// }

/*
//Dominique Breton
//breton@lal.in2p3.fr
//Ok ! The equivalent code for binary data saving is herebelow (between ***) :
//30.08.2016
*******************************************************************
            tempBuffer = malloc(Nb_Of_Samples* sizeof(signed short));
             // Sampling Period
             result = fwrite (&samplingPeriod, sizeof(float), 1, FileForSavingDataPntr);
             //FCR
             result= fwrite(&FirstCellToPlot,sizeof(int),1,FileForSavingDataPntr);
             for(channel = 0; channel <MAX_NB_OF_CHANNELS; channel ++)
             {
                 
                 result= fwrite(&channel,sizeof(int),1,FileForSavingDataPntr);
                  if(GuiParamValues.ChannelMask & (0x1 << channel))
                  {
                      channelIsPresent = TRUE;
                      result= fwrite(&channelIsPresent,sizeof(int),1, FileForSavingDataPntr);
                   
                    for(k= 0; k<Nb_Of_Samples; k++)
                    {
                       tempBuffer[k]= (signed short)(FullyOrderedValues[channel][k]*10);
                    }
                   
                    result= fwrite (tempBuffer, sizeof(signed short), Nb_Of_Samples,FileForSavingDataPntr);
                   
                 }
                 else
                  {
                      channelIsPresent = FALSE;
                      result= fwrite(&channelIsPresent,sizeof(int),1, FileForSavingDataPntr);
                 }
             }
             
            free(tempBuffer);     

*********************************************************************************
*/
