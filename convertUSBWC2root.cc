//root
#include <TH1D.h>
#include <TStyle.h>
#include <TString.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

//C, C++
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

void convBunchDataUSB2root(TString dataFileList, TString datPreff, TString rootFileN);

int main(int argc, char *argv[] ){

  if(argc == 4){
    TString dataFileList = argv[1];
    TString datPreff = argv[2];
    TString rootFileN = argv[3];
    cout<<endl
	<<"       File containing the list of input data files: "<<dataFileList<<endl
	<<" Path of the directory containing these input files: "<<datPreff<<endl
	<<"                       Name of the output ROOT file: "<<rootFileN<<endl<<endl;
    convBunchDataUSB2root( dataFileList, datPreff, rootFileN);
  }
  else{
    cout<<endl
	<<"  ERROR ---> in input arguments "<<endl
	<<"             [1] : name of a text file containing the list of USBWC files to be converted to ROOT"<<endl
	<<"             [2] : path of the directory containing the input USBWC files"<<endl
	<<"             [3] : name of the output ROOT file"<<endl;
  }

  return 0;
}

void convBunchDataUSB2root(TString dataFileList, 
			   TString datPreff, 
			   TString rootFileN){

  const Int_t nMaxChannel=2;
  const Int_t nSamplingPoints=1024;

  //USBWC data in one ev//
  Int_t _eventID;
  Float_t _SamplingPeriod;
  Int_t _INLCorrection;
  Int_t _FCR;
  Double_t _UnixTime;
  Float_t _ch[nMaxChannel][nSamplingPoints];
  ///////////////////////

  ///////////////////Root file with data/////////////////
  TFile *hfile = new TFile( rootFileN, "RECREATE", "USBWC Data", 1);
  if (hfile->IsZombie()) {
    cout << "PROBLEM with the initialization of the output ROOT ntuple " 
	 << rootFileN << ": check that the path is correct!!!"
	 << endl;
    exit(-1);
  }
  TTree *tree = new TTree("T", "USBWC Data Tree");
  hfile->SetCompressionLevel(2);
  tree->SetAutoSave(1000000);
  // Create new event
  TTree::SetBranchStyle(0);

  //Event USBWC
  tree->Branch("eventID",&_eventID, "eventID/I");
  tree->Branch("SamplingPeriod", &_SamplingPeriod,  "SamplingPeriod/F");
  tree->Branch("INLCorrection", &_INLCorrection,  "INLCorrection/I");
  tree->Branch("UnixTime",&_UnixTime, "UnixTime/D");
  tree->Branch("ch", _ch, "ch[2][1024]/F");
  ///////////////////////////////////////////////////////

  Int_t i = 0;
  Int_t j = 0;
  string mot;
  Double_t amplVal;
  Int_t chID = 0;

  Int_t nEv_Tot = 0;

  Double_t timeNpevEv = -999.0;


  // Init event variables
  _eventID = -999;
  _SamplingPeriod = -999.0;
  _INLCorrection = -999;
  _UnixTime = -999.0;

  for(j = 0;j<nMaxChannel;j++){
    for(i = 0;i<nSamplingPoints;i++){
      _ch[j][i] = -999.0;
    }
  }

  ifstream indataFileUSB;
  string dataFileName;
  TString fileN;
  TString rootF;
  indataFileUSB.open(dataFileList.Data()); 
  assert(indataFileUSB.is_open());  
  while (indataFileUSB  >> dataFileName ){
    fileN = datPreff + "/" + dataFileName;
    if(indataFileUSB.eof()){
      std::cout<<"EOF"<<std::endl;
      break;
    }
    cout
      <<"\t * Converting ---> "<<fileN<<endl;

    /////////////////////////////
    ifstream indata;
    indata.open(fileN.Data()); 
    assert(indata.is_open());

    while (indata  >> mot ){
      //cout<<" New word: "<<mot<<endl;
      
      // Example of line decoded below
      // === EVENT 1 === FULLY ORDERED DATA [256 samples] in Volts === NB OF WAVECAT BOARDS = 8 ===
      //=== EVENT 1 === FULLY ORDERED DATA [1024 samples] in Volts === TRIG DELAY = 14 == 
      //Sampling Period = 312.5 ps, INL Correction: YES 
      if(mot == "EVENT"){
	indata  >> _eventID;
	//cout<<"_eventID "<<_eventID<<endl;
      }    
      //cout<<"1"<<endl;
      // Example of line decoded below
      // Sampling Period = 312.5 ps, INL Correction: YES 
      if(mot == "Sampling"){
	indata >> mot;
	if(mot == "Period"){
	  //cout<<mot<<endl;
	  indata >> mot;
	  indata >> _SamplingPeriod;
	  //cout<<" _SamplingPeriod "<<_SamplingPeriod<<endl;
	} // Period
      } // Sampling     
      //cout<<"2"<<endl;
      if(mot == "INL"){
	indata  >> mot;
	if(mot == "Correction:"){
	  indata  >> mot;
	  if(mot == "YES"){
	    _INLCorrection = 1;
	  }
	  if(mot == "NO"){
	    _INLCorrection = 0;
	  }
	} // Correction
	//cout<<" _INLCorrection "<<_INLCorrection<<endl;
      } // INL
      //cout<<"3"<<endl;
      if(mot == "UnixTime"){
	indata  >> mot;
	indata  >> _UnixTime;
	//printf("UnixTime: %20.4f \n", _UnixTime);
	/*
	if(timeNpevEv != -999.0 && (_UnixTime - timeNpevEv)<0.0){
	  cout<<endl<<" events (files) in data list are not ordered in time:"<<endl;
	  printf("\t * previous unix time: %20.4f \n", timeNpevEv);
	  printf("\t *  current unix time: %20.4f \n", _UnixTime);
	  assert(0);
	}
	timeNpevEv = _UnixTime;
	*/
      }
      //cout<<"4"<<endl;      
      //== CHANNEL : 0 == OFFSET = 0.00 == TRIG ENABLE = 1 == TRIG THRESH = 0.05 ==
      //
      if(mot == "CHANNEL"){
	indata  >> mot;
	indata  >> chID;
	//cout<<"chID = "<<chID<<endl;
	if( chID < 0 || chID > nMaxChannel){
	  cout<<"chID = " << chID << " should be between 0 and nMaxChannel!"<<endl
	      <<"nMaxChannel = "<<nMaxChannel<<endl;
	  assert(0);
	}
	
	indata  >> mot;//== 
	indata  >> mot;//OFFSET
	indata  >> mot;// = 
	indata  >> mot;//0.00
	indata  >> mot;// == 
	indata  >> mot;//TRIG 
	indata  >> mot;//ENABLE 
	indata  >> mot;//= 
	indata  >> mot;//1 
	indata  >> mot;//== 
	indata  >> mot;//TRIG 
	indata  >> mot;//THRESH 
	indata  >> mot;//= 
	indata  >> mot;//0.05 
	indata  >> mot;//==	
	//cout<<"mot = "<<mot<<endl;
	///////////////
	for(i = 0;i<nSamplingPoints;i++){
	  indata  >> amplVal;
	  _ch[chID][i] = amplVal;
	  //cout<<" i = "<<i<<" "<<_ch[chID][i]<<endl;
	}
	
	if(chID==nMaxChannel-1){
	  tree->Fill();
	  _eventID = -999;
	  _SamplingPeriod = -999.0;
	  _INLCorrection = -999;
	  _UnixTime = -999.0;
	  for(j = 0;j<nMaxChannel;j++){
	    for(int iii = 0;iii<nSamplingPoints;iii++){
	      _ch[j][iii] = -999.0;
	    }
	  }
	  nEv_Tot++;
	} // if(mot == "CHANNEL"){
	
	//cout<<"while (indata  >> mot )"<<endl;
	///////////////////////    
      }
    } // while (indata  >> mot ){
      //cout<<" close "<<endl;
    indata.close();
    cout << "\t   " << nEv_Tot << " events added to the ROOT tree" << endl; 
    
    /////////////////////////////
  }
  indataFileUSB.close();
  hfile = tree->GetCurrentFile();
  hfile->Write();  
  hfile->Close();
}
