#include <iostream>
#include <fstream>

#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TMath.h"

using namespace std;

void pre(Int_t index=1)
{
   // input ============================
   if (index!=1 && index!=4 && index!=6) { // index = 1, 4, 6
      cout<<"try pre(1), pre(4) or pre(6)"<<endl;
      return;
   }
   TChain* pstr = new TChain("PSTree","root"); 
   pstr->AddFile(Form("/.hb/raidg01/Siegfried_II/Run_05/Cd_ET/Cd_et000%d.root",index));

   const Int_t Ncha = 20;
   Int_t Cha_MCAEnergy[Ncha];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // read calibration parameter   
   Float_t Escale[Ncha]={0};
   ifstream califile(Form("/.hb/raidg01/Siegfried_II/Run_05/Cd_ET/Cd_et000%d.cal",index));
   for (Int_t i = 0; i < Ncha-1; i++) {
      califile >> Escale[i];
      cout<<Escale[i]<<endl;
   }
   califile.close();

   // output ===========================
   TFile* foutput = new TFile(Form("pre%d.root",index),"recreate");
   TTree *newtree = pstr->CloneTree(0);

   // main loop ========================
   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   //for (Int_t i=0; i<1e6;i++) {
   for (Int_t i=0; i<Nevt;i++) {
      if (i%100000==0) cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      Int_t Nseg=0, firedSeg=0;
      for (Int_t j=1; j<Ncha-1; j++) {
	 if (((Float_t) Cha_MCAEnergy[j])*Escale[j]>15) { // Eseg>15 keV
	    Nseg++;
	    firedSeg=j;
	 }
      }
      if (Nseg==1) // single segment event
	if (firedSeg==1 || firedSeg==2 || firedSeg==3 ||
	    firedSeg==16|| firedSeg==17|| firedSeg==18) // fired seg in top layer
	  if (((Float_t) Cha_MCAEnergy[firedSeg])*Escale[firedSeg]<100) { // Eseg<100 keV
	     newtree->Fill();
	  }
      if (i%100000==0) newtree->Write(newtree->GetName(),TObject::kWriteDelete);
   }
   
   newtree->Write();
   foutput->Close();
} 

// ----------------------------------------------------------------------------

void mca(Int_t index=1)
{
   // input ============================
   if (index!=1 && index!=4 && index!=6) { // index = 1, 4, 6
      cout<<"try mca(1), mca(4) or mca(6)"<<endl;
      return;
   }
   TChain* pstr = new TChain("PSTree","root"); 
   pstr->AddFile(Form("pre%d.root",index));

   const Int_t Ncha = 20;
   Int_t Cha_MCAEnergy[Ncha];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // read calibration parameter   
   Float_t Escale[Ncha]={0};
   ifstream califile(Form("/.hb/raidg01/Siegfried_II/Run_05/Cd_ET/Cd_et000%d.cal",index));
   for (Int_t i = 0; i < Ncha-1; i++) {
      califile >> Escale[i];
      cout<<Escale[i]<<endl;
   }
   califile.close();

   // book histograms =================
   TH1F *MCA[Ncha]={0};
   for (Int_t i=0; i<Ncha-1; i++)
      MCA[i] = new TH1F(Form("MCA%d",i),
	    Form("MCA Spectrum for segment %d",i),
	    60,70/Escale[i],100/Escale[i]);

   // main loop =======================
   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   for (Int_t i=0; i<Nevt;i++) {
      if (i%100000==0) cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      for (Int_t j=0; j<Ncha-1; j++)
	 MCA[j]->Fill((Float_t) Cha_MCAEnergy[j]);
   }
 
   // output ==========================
   TFile* foutput = new TFile(Form("mca%d.root",index),"recreate");
   for (Int_t i=0; i<Ncha-1; i++) MCA[i]->Write();
   foutput->Close();
   
   for (Int_t i=0; i<Ncha-1; i++) delete MCA[i];
}

// ----------------------------------------------------------------------------

void fitmca(Int_t index=1)
{
   // input ============================
   if (index!=1 && index!=4 && index!=6) { // index = 1, 4, 6
      cout<<"try mca(1), mca(4) or mca(6)"<<endl;
      return;
   }

   const Int_t Ncha = 19;
   TH1F *MCA[Ncha]={0};
   TFile* fmca = new TFile(Form("mca%d.root",index));
   for (Int_t i=0; i<Ncha; i++) {
      fmca->GetObject(Form("MCA%d",i),MCA[i]);
   }

   // look for peak position ===========
   Float_t mean[Ncha] = {0};
   Int_t segID[Ncha]={0,1,2,3,7,8,9,13,14,15,16,17,18,10,11,12,4,5,6};
   TSpectrum peak(2);
   for (Int_t i=0; i<Ncha; i++) { // i is the channel ID
      cout<<"Channel number = "<<i<<" =========================="<<endl;
      Int_t Npeak=peak.Search(MCA[i],2,"",0.3);
      cout<<"\t number of peaks found = "<<Npeak<<endl;
      if (Npeak ==1) {
	 mean[segID[i]] = peak.GetPositionX()[0];
	 cout<<"\t the peak position in segment "<<segID[i]<<" = "<<mean[segID[i]]<<endl;
      }
   }

   // prepare fit function =============
   TF1 *FitFcn = new TF1("FitFcn","[0]*gausn(1)+pol1(4)");
   FitFcn->SetParNames("bin width","events","mean","sigma","pol0","pol1");
  
   // initial values of the parameters
   // the first value is not used
   Float_t events[7] = {0,3e4,3e4,3e4,3e4,3e4,3e4};
   Float_t sigma[7]  = {0,50, 50, 50, 50, 50, 50};
   Float_t range[7]  = {0,550,550,550,550,550,550};
   Float_t pol0[7]   = {0,-2000,-2000,-2000,-2000,-2000,-2000};
   Float_t pol1[7]   = {0,1,1,1,1,1,1};

   // main loop ========================
   Float_t xlow, xup;
   // prepare arrays to hold the output from the fit
   Float_t N[7]  = {0};
   Float_t dN[7] = {0};

   Float_t ADC[7]  = {0};
   Float_t dADC[7] = {0};

   Float_t ADCw[7]  = {0};
   Float_t dADCw[7] = {0};
   // prepare graphic output
   TCanvas *c2 = new TCanvas ("c2","c2",0,0,1200,800);
   c2->Divide(3,2,0.0001,0.0001);
   // prepare output file
   ofstream dat;
   dat.open(Form("Nseg%d.txt",index));
   // main loop
   Int_t chaID[Ncha]={0,1,2,3,16,17,18,4,5,6,13,14,15,7,8,9,10,11,12};
   for (Int_t i=1; i<=6; i++) { // i is the segment ID
      xlow = mean[i]-range[i];
      xup  = mean[i]+range[i];      

      FitFcn->SetParameters(1.0,events[i],mean[i],sigma[i],pol0[i],pol1[i]);
      FitFcn->FixParameter(0,MCA[chaID[i]]->GetBinWidth(1));

      c2->cd(i);
      MCA[chaID[i]]->SetAxisRange(xlow,xup);
      MCA[chaID[i]]->Fit("FitFcn","","",xlow,xup);

      N[chaID[i]] = FitFcn->GetParameter("events");
      dN[chaID[i]] = FitFcn->GetParError(1);

      ADC[chaID[i]] = FitFcn->GetParameter("mean");
      dADC[chaID[i]] = FitFcn->GetParError(2);

      ADCw[chaID[i]] = FitFcn->GetParameter("sigma");
      dADCw[chaID[i]] = FitFcn->GetParError(3);

      dat<<i<<" \t "<<N[chaID[i]]<<" \t 0.5 \t "<<dN[chaID[i]]<<endl;
   }
   dat.close();
}

void occupancy(Int_t index=1)
{
   if (index!=1 && index!=4 && index!=6) { // index = 1, 4, 6
      cout<<"try occupancy(1), occupancy(4) or occupancy(6)"<<endl;
      return;
   }

   // input ============================
   // data
   TGraphErrors *gdat = new TGraphErrors(Form("Nseg%d.txt",index));
   // check bin contents and
   // calculate the sum of all bin contents
   Double_t Nevt[6] = {0}, dNevt[6] = {0}, Sumdat = 0;
   for (Int_t k=0; k<6; k++) { 
      Nevt[k] =(gdat->GetY())[k];
      dNevt[k]=(gdat->GetEY())[k];
      cout<<"segment "<<k+1<<" has "<<Nevt[k]<<" +- "<<dNevt[k]<<" entries"<<endl;
      Sumdat+=Nevt[k];
   }
   cout<<"Nevts in dat = "<<Sumdat<<endl;
 
   // simulation
   // book histogram
   if (gDirectory->Get("hsim")) delete (TH1D*)(gDirectory->Get("hsim"));
   TH1D *hsim = new TH1D("hsim","Occupancy",6,0.,360.);
   // read in tree
   TFile file("../sim/Occ0impurity.root","READ");
   //TFile file("../sim/Occ4e9.root","READ");
   //TFile file("../sim/Occ1e10.root","READ");
   TTree *occu;
   file.GetObject("occu", occu);
   Int_t Nhits = occu->GetEntries();
   cout<<"Nhits in sim = "<<Nhits<<endl;
   // set variables to hold contents of the tree
   Double_t x, y, z, r, phi;
   occu->SetBranchAddress("x",&x);
   occu->SetBranchAddress("y",&y);
   occu->SetBranchAddress("z",&z);
   occu->SetBranchAddress("r",&r);
   occu->SetBranchAddress("phi",&phi);

   // main loop ========================
   const Int_t na=360; // total number of loops
   Double_t phi110[na], Chi2[na];
   Double_t MaxChi2 = 1e9; // a big number for comparison
   Double_t Nbest[6] = {0}, dNbest[6] = {0}; // to hold the best simulated occupancy distribution
   for (Int_t i=0; i<na; i++) { // try different phi110
      phi110[i]=i; // 1 degree increase of phi110 per step
      // fill hsim
      hsim->Reset(); // ready for refill
      for (Int_t j=0; j<Nhits; j++) {
         occu->GetEntry(j);
         phi+=phi110[i]; // all phis are shifted by phi110
         if (phi>360.) phi-=360.;
         hsim->Fill(phi); // fill shifted phis
      }
      // get bin contents and normalized to data
      Double_t occ[6] = {0}, Sumsim = (Double_t) Nhits;
      Double_t rate[6] = {0}, drate[6] = {0}; 
      for (Int_t j=0; j<6; j++) { 
	 occ[j] = (Double_t) hsim->GetBinContent(j+1);
	 rate[j] = occ[j]/Sumsim*Sumdat;
	 drate[j]=rate[j]*TMath::Sqrt(1./occ[j]+1./Sumsim+1./Sumdat);
      }
      // calculate chi2
      Double_t sum = 0;
      for (Int_t j=0; j<6; j++)
         sum+=(Nevt[j]-rate[j])*(Nevt[j]-rate[j])/(dNevt[j]*dNevt[j]+drate[j]*drate[j]);
      Chi2[i]=sum;
      // record the simulated occupancy distribution
      // that is most similar to that in data
      if (MaxChi2>Chi2[i]) {
	 MaxChi2=Chi2[i];
	 cout<<"When phi110 = "<<i
	    <<" degrees, Chi2 reaches a new minimal "<<Chi2[i]<<endl;

	 for (Int_t j=0; j<6; j++) {
	    Nbest[j] = rate[j];
	    dNbest[j]=drate[j];
	 }
      }
   }

   // plot the results ====================
   TCanvas *c1 = new TCanvas("c1","Occupancy",0,0,1200,600);
   c1->Divide(2,1,0.0001,0.0001);

   // plot occupancy distribution in data
   c1->cd(1);
   gPad->SetLeftMargin(0.18);
   gPad->SetRightMargin(0.01);
   gPad->SetBottomMargin(0.11);
   
   gdat->SetTitle("");
   gdat->SetMarkerColor(kBlue);

   gdat->GetXaxis()->SetRangeUser(0.5,6.5);
   gdat->GetXaxis()->SetTitle("segment ID");
   gdat->GetXaxis()->CenterTitle();

   gdat->GetYaxis()->SetRangeUser(0,42000);
   gdat->GetYaxis()->SetTitle("Occupancy");
   gdat->GetYaxis()->SetTitleOffset(1.9);
   gdat->GetYaxis()->CenterTitle();

   gdat->Draw("ap");

   // overlay simulated occupancy distribution from the best fit
   Double_t segID[6] = {1,2,3,4,5,6};
   Double_t dsegID[6] = {0.5,0.5,0.5,0.5,0.5,0.5};
   TGraphErrors *gsim = new TGraphErrors(6,segID,Nbest,dsegID,dNbest);
   gsim->SetLineColor(kRed);
   gsim->SetMarkerColor(kRed);
   gsim->SetTitle("");
   gsim->Draw("p");

   // add legend
   TLegend* le=new TLegend(0.65,0.35,0.95,0.5);
   le->AddEntry(gdat,"Date","p");
   le->AddEntry(gsim,"Simulation","p");
   le->SetFillStyle(0);
   le->SetTextFont(132); 
   le->Draw();

   //TText *te=new TText();
   //te->SetTextFont(132);
   //te->DrawText(94,17384,"(a)");

   // draw Chi2/NDF V.S. phi110
   c1->cd(2);
   gPad->SetLeftMargin(0.15);
   gPad->SetRightMargin(0.01);
   gPad->SetBottomMargin(0.11);

   Double_t NDF=5, CdN[na]={0};
   for (Int_t i = 0; i < na; i++) CdN[i]=Chi2[i]/NDF;
   TGraph *gchi = new TGraph(na, phi110, CdN);

   gchi->SetTitle("");
   gchi->SetMarkerStyle(7);
   gchi->GetXaxis()->SetTitle("#phi_{110} [degree]");
   gchi->GetXaxis()->CenterTitle();
   gchi->GetXaxis()->SetRangeUser(-2,361);
   gchi->GetYaxis()->SetTitle("#chi^{2}/NDF");
   gchi->GetYaxis()->CenterTitle();
   gchi->GetYaxis()->SetTitleOffset(1.4);

   gchi->Draw("pa");

   c1->Print(Form("../memo/occ0_%d.eps",index));
   //c1->Print(Form("../memo/occ4e9_%d.eps",index));
   //c1->Print(Form("../memo/occ1e10_%d.eps",index));

   //te->DrawText(77,200,"(b)");
}
