#include <iostream>

#include "TChain.h"
#include "TFile.h"

using namespace std;

//=============================================================================

const Int_t Ncha = 20;

const Int_t Nscan = 12;
Int_t z[Nscan] = {175,179,183,187,191,195,199,203,207,211,215,219};
// 171 no peak; 175 - 191 peak in seg 11; 195-215 in seg 14; 219 in seg 17

TString input="/.hb/raidg01/Siegfried_II/Run_05/Eu152_Scan_PS/";

//=============================================================================

void pre(Int_t theZ=0)             // a preliminary selection
{
   for (Int_t i=0; i<Nscan; i++) { // check if the data available
      if (theZ==z[i])              // if available,
	 break;                    // break the loop, start processing

      if (i==Nscan-1) {            // if not available till the end
	 cout<<"Scan data at "<<theZ<<" mm high is not available"<<endl;
	 cout<<"Please try one of these: \n \t"
	    <<"171,175,179,183,187,191,195,199,203,207,211,215,219"<<endl;
	 return;
      }
   }

   cout<<"start to process scan data at "<<theZ<<" mm..."<<endl;

   // input
   // =====
   TChain PSTree("PSTree","PSTree");
   PSTree.AddFile(input+Form("phi45_z%dmm/Eu_scan_phi45_z%dmm0001.root",theZ,theZ));
   PSTree.AddFile(input+Form("phi45_z%dmm/Eu_scan_phi45_z%dmm0002.root",theZ,theZ));
   PSTree.AddFile(input+Form("phi45_z%dmm/Eu_scan_phi45_z%dmm0003.root",theZ,theZ));

   Int_t Cha_MCAEnergy[Ncha];
   PSTree.SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // output
   // ======
   TFile fout(Form("z%d.root",theZ),"recreate");
   TTree *newtree = PSTree.CloneTree(0);

   // main loop
   // =========
   Int_t Nevt = PSTree.GetEntries();
   cout<<"total events to be processed: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
   //for (Int_t i=0; i<1000;i++) {
      if (i%10000==0) cout<<"events processed: "<<i<<endl;

      PSTree.GetEntry(i);

      Int_t NfiredSeg = 0, firedSegID = 0, EnergyThreshold = 1000;
      for (Int_t j=1; j<Ncha-1; j++) {
         if (Cha_MCAEnergy[j]>EnergyThreshold) {
            NfiredSeg++;
	    firedSegID=j;
         }
      }
      if (NfiredSeg==1) // single segment event
	 if (firedSegID==11 || firedSegID==14 || firedSegID==17)
	    if (Cha_MCAEnergy[firedSegID]<5500) // no high energy photons
	       if (Cha_MCAEnergy[firedSegID]>3000) // no low energy photons
		  newtree->Fill();
   }
   
   newtree->Write();
   fout.Close();
   cout<<"processing scan data at "<<theZ<<" mm finished."<<endl;
}

//=============================================================================

void zmm(Int_t theZ=0)
{
   if (theZ==0) // if no z specified, run all
      for (Int_t i=0; i<Nscan; i++) pre(z[i]);
   else             // if the z specified, run it
      pre(theZ);
}

//=============================================================================


