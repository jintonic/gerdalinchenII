#include "CLHEP/Units/PhysicalConstants.h"

#include "io/MGLogger.hh"
#include "io/MGCrystalHits.hh"
#include "waveform/MGWFlibGenRK4.hh"
#include "waveform/MGSORCrystalFields.hh"

#include "TChain.h"
#include "TF1.h"

using namespace std;
using namespace CLHEP;

int main()
{
//   MGLogger::SetSeverity(MGLogger::debugging);
   MGLogger::SetSeverity(MGLogger::routine);

   // read hits from a ROOT file
   TChain* chain = new TChain("fTree");
   chain->Add("SII_Cd.root");

   const int maxNhits = 1000;
   int   Nhits;  
   float hitX[maxNhits],hitY[maxNhits],hitZ[maxNhits],hitT[maxNhits],hitE[maxNhits];

   chain->SetBranchStatus("*",1);
   chain->SetBranchAddress("Nhits",&Nhits);
   chain->SetBranchAddress("hitX",hitX);
   chain->SetBranchAddress("hitY",hitY);
   chain->SetBranchAddress("hitZ",hitZ);
   chain->SetBranchAddress("hitT",hitT);
   chain->SetBranchAddress("hitE",hitE);
   cout<<"TChain properly setup - DONE"<<endl;

   // combine hits
   MGCrystalHits xtalHits;
   MGCrystalHit oneHit;

   std::vector<Hep3Vector>* coordinates = new std::vector<Hep3Vector>;
   Hep3Vector hit;

   int Nevt=chain->GetEntries();
   cout<<"Nevt = "<<Nevt<<endl;
   //for (int i=0; i<10; i++) {
   for (int i=0; i<Nevt; i++) {
      if (i%10000==0) cout<<"now event "<<i<<endl;
      chain->GetEntry(i);

      xtalHits.Reset();
      for (int j=0; j<Nhits; j++) {
	 xtalHits.AddCrystalHit(hitX[j]*cm,hitY[j]*cm,0,hitT[j]*ns,hitE[j]*keV,0);
      }
      xtalHits.CombineHits(1*ns,1*mm,512*ns); //last parameter cannot be omitted.
      //xtalHits.Print();

      size_t NcombinedHits = xtalHits.GetNHits();
      //cout<<"NcombinedHits = "<<NcombinedHits<<endl;
      for (size_t j=0; j<NcombinedHits; j++) {
	 oneHit=xtalHits.GetHit(j);
	 //cout<<"hit "<<j<<": ("
	 //   <<oneHit.fX/mm<<", "
	 //   <<oneHit.fY/mm<<", "
	 //   <<oneHit.fZ/mm<<")*mm"<<endl;

	 hit.set(oneHit.fX,oneHit.fY,oneHit.fZ);
	 coordinates->push_back(hit);
      }
   }
   cout<<"hits are all distributed."<<endl;

   // fill in crystal data
   int id = 0;
   double ri = 5.0*CLHEP::mm, ro = 37.5*CLHEP::mm, hi = 70.0*CLHEP::mm;
   MGCrystalData* crystal = new MGCrystalData(
	 id, 		// crystal ID
	 MGCrystalData::kNType,
	 MGCrystalData::kCoaxial,
	 ri,  		// inner radius
	 ro, 		// outer radius
	 hi, 		// height
	 hi, 		// well depth
	 0.0, 		// global x
	 0.0, 		// global y
	 0.0, 		// global z
	 3,   		// Nseg in z
	 6);  		// Nseg in phi
   crystal->SetCrystalAxesAngle(0.0*CLHEP::degree);
   crystal->SetSegmentationAngle(0.0*CLHEP::degree);
   
   // load pre-calculated fields
   MGSORCrystalFields* fields = new MGSORCrystalFields(crystal);
   fields->SetFileName("f0impurity");
   //fields->SetFileName("f4e9");
   //fields->SetFileName("f1e10");
   fields->LoadFields();

   double wfSampRate = 1.*CLHEP::GHz;
   size_t wfLength = 1000;
   MGWFlibGenRK4 gen2(
	 crystal,
	 fields,
	 coordinates,
	 wfSampRate,
	 wfLength);

   gen2.Occupancy("Occ0impurity.root");
   //gen2.Occupancy("Occ4e9.root");
   //gen2.Occupancy("Occ1e10.root");

   delete fields;
   delete crystal;
   delete coordinates;
   delete chain;

   return 0;
}

