#include <iostream>

#include "TChain.h"
#include "TFile.h"

using namespace std;

//=============================================================================

const Int_t Ncha = 20;

const Int_t Nscan = 20;
Int_t angle[Nscan] = {5,10,15,20,25,30,35,40,45,50,55,60,65,70,73,75,77,80,85,90};

TString input="/.hb/raidg01/Siegfried_II/Run_05/Eu152_Scan_PS/phi/super/";
//TString input="/.hb/raidg01/Siegfried_II/Run_05/Eu152_Scan_PS/fine_again/";

//=============================================================================

void pre(Int_t theAngle=0)         // a preliminary selection
{
   for (Int_t i=0; i<Nscan; i++) { // check if the data available
      if (theAngle==angle[i])      // if available,
	 break;                    // break the loop, start processing

      if (i==Nscan-1) {            // if not available till the end
	 cout<<"Scan data at "<<theAngle<<" degree is not available"<<endl;
	 cout<<"Please try one of these: \n \t"
	    <<"5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90"<<endl;
	 return;
      }
   }

   cout<<"start to process scan data at "<<theAngle<<" degree..."<<endl;

   // input
   // =====
   TChain PSTree("PSTree","PSTree");
   PSTree.AddFile(input+Form("Eu_scan_phi%d_z2070001.root",theAngle));
   PSTree.AddFile(input+Form("Eu_scan_phi%d_z2070002.root",theAngle));
   PSTree.AddFile(input+Form("Eu_scan_phi%d_z2070003.root",theAngle));
   //PSTree.AddFile(input+Form("Eu152_z207_phi%d_again0001.root",theAngle));
   //PSTree.AddFile(input+Form("Eu152_z207_phi%d_again0002.root",theAngle));
   //PSTree.AddFile(input+Form("Eu152_z207_phi%d_again0003.root",theAngle));
   //PSTree.AddFile(input+Form("Eu152_z207_phi%d_again0004.root",theAngle));
   //PSTree.AddFile(input+Form("Eu152_z207_phi%d_again0005.root",theAngle));
   //PSTree.AddFile(input+Form("Eu152_z207_phi%d_again0006.root",theAngle));

   Int_t Cha_MCAEnergy[Ncha];
   PSTree.SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // output
   // ======
   TFile fout(Form("phi%d.root",theAngle),"recreate");
   TTree *newtree = PSTree.CloneTree(0);

   // main loop
   // =========
   Int_t Nevt = PSTree.GetEntries();
   cout<<"total events to be processed: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
   //for (Int_t i=0; i<1000;i++) {
      if (i%10000==0) cout<<"events processed: "<<i<<endl;

      PSTree.GetEntry(i);

      Int_t NfiredSeg = 0, firedSegID = 0, EnergyThreshold = 1;
      for (Int_t j=1; j<Ncha-1; j++) {
         if (Cha_MCAEnergy[j]>EnergyThreshold) {
            NfiredSeg++;
	    firedSegID=j;
         }
      }
      	 if (firedSegID==13 || firedSegID==14 || firedSegID==15)
	    if (Cha_MCAEnergy[firedSegID]<22000) // no high energy photons
	       if (Cha_MCAEnergy[firedSegID]>16000) // no low energy photons
		  newtree->Fill();
   }
   
   newtree->Write();
   fout.Close();
   cout<<"processing scan data at "<<theAngle<<" degree finished."<<endl;
}

//=============================================================================

void phi(Int_t theAngle=0)
{
   if (theAngle==0) // if no angle specified, run all
      for (Int_t i=0; i<Nscan; i++) pre(angle[i]);
   else             // if the angle specified, run it
      pre(theAngle);
}

//=============================================================================


