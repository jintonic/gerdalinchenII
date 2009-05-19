#include <iostream>
#include <fstream>

#include <TChain.h>
#include <TString.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TROOT.h>
#include <TSpectrum.h>

using namespace std;

TString inputdir="/.hb/raidg01/Siegfried_II/Run_05/Cd_PS/";

const Int_t Ncha = 20;
const Int_t Nseg = 7; // 6 + core

//=============================================================================

void mcanocut() // mca spectra without any cut
{
   // input
   TChain* pstr = new TChain("PSTree","root"); 
   for (Int_t i=1; i<10; i++) pstr->AddFile(inputdir+Form("Cd000%d.root",i));
   //for (Int_t i=10; i<30; i++) pstr->AddFile(inputdir+Form("Cd00%d.root",i));

   Int_t Cha_MCAEnergy[Ncha];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // book histograms
   TH1F *MCA[Nseg]; // segment MCA
   TH1F *SOC[Nseg]; // segment MCA / core MCA
   for (Int_t i=1; i<Nseg; i++) {
      MCA[i] = new TH1F(Form("MCA%d",i),
            Form("MCA for segment %d",i),300,1500,4500);
      SOC[i] = new TH1F(Form("SOC%d",i),
            Form("MCA segment %d over core",i),200,0.8,1.6);
   }

   // main loop
   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      Float_t corMCA=(Float_t)Cha_MCAEnergy[0];
      if (corMCA>0) {
         for (Int_t j=1; j<=3; j++) {
            Float_t segMCA=(Float_t)Cha_MCAEnergy[j];
            MCA[j]->Fill(segMCA);
            SOC[j]->Fill(segMCA/corMCA);
         }
         for (Int_t j=4; j<Nseg; j++) {
            Float_t segMCA=(Float_t)Cha_MCAEnergy[j+12];
            MCA[j]->Fill(segMCA);
            SOC[j]->Fill(segMCA/corMCA);
         }
      }
   }
 
   // output
   TFile* foutput = new TFile("Run5mcanocut.root","recreate");
   for (Int_t i=1; i<Nseg; i++) {
      MCA[i]->Write();
      SOC[i]->Write();
   }
   foutput->Close();

   // clean up
   delete foutput;
   delete pstr;
   for (Int_t i=1; i<Nseg; i++) {
      delete MCA[i];
      delete SOC[i];
   }
}

//=============================================================================

void fitmcanocut() // get calibration parameters and widths of 88 keV peaks
{
   // input mca spectra
   TH1F *MCA[Nseg];
   TFile* fmca = new TFile("Run5mcanocut.root");
   for (Int_t i=1; i<Nseg; i++) {
      fmca->GetObject(Form("MCA%d",i),MCA[i]);
   }

   // define fit function and parameters
   TF1 *FitFcn = new TF1("FitFcn","[0]*gausn(1)+pol1(4)");
   FitFcn->SetParNames("bin width","events","mean","sigma","pol0","pol1");
  
   Float_t events[Nseg] = {1e3,1e3,1e3,1e3,1e3,1e3,1e3};
   Float_t mean[Nseg]   = {2303,2825,2557,2689,3206,2770,2602};
   Float_t sigma[Nseg]  = {150,150,150,150,150,150,150};
   Float_t range[Nseg]  = {550,550,550,550,550,550,550};
   Float_t pol0[Nseg]   = {0};
   Float_t pol1[Nseg]   = {0};

   Float_t xlow, xup;
   
   Float_t N[Nseg]  = {0};
   Float_t dN[Nseg] = {0};

   Float_t ADC[Nseg]  = {0};
   Float_t dADC[Nseg] = {0};

   Float_t ADCw[Nseg]  = {0};
   Float_t dADCw[Nseg] = {0};

   // fit peaks in each channel (no core)
   TCanvas *c2 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("c2");
   if (!c2)  c2 = new TCanvas("c2","",0,0,1200,800);
   c2 -> Clear();
   c2->Divide(3,2);
   ofstream dat;
   dat.open("Run5cali.txt");
   for (Int_t i=1; i<Nseg; i++) {
      xlow = mean[i]-range[i];
      xup  = mean[i]+range[i];      

      FitFcn->SetParameters(1.0,events[i],mean[i],sigma[i],pol0[i],pol1[i]);
      FitFcn->FixParameter(0,MCA[i]->GetBinWidth(1));

      c2->cd(i);
      MCA[i]->SetAxisRange(xlow,xup);
      MCA[i]->Fit("FitFcn","","",xlow,xup);

      N[i] = FitFcn->GetParameter("events");
      dN[i] = FitFcn->GetParError(1);

      ADC[i] = FitFcn->GetParameter("mean");
      dADC[i] = FitFcn->GetParError(2);

      ADCw[i] = FitFcn->GetParameter("sigma");
      dADCw[i] = FitFcn->GetParError(3);

      dat<<ADC[i]<<" \t\t "<<dADC[i]<<" \t\t "
         <<ADCw[i]<<" \t\t "<<dADCw[i]<<" \t\t "
         <<N[i]<<" \t\t "<<dN[i]<<endl;
   }
   dat.close();
}

//=============================================================================

void pre(Int_t setID=1) // main shrinking function
{
   // input
   TChain* pstr = new TChain("PSTree","root"); 
   if (setID==1)
      for (Int_t i=1; i<10; i++)
         pstr->AddFile(inputdir+Form("Cd000%d.root",i));
   else if (setID==2)
      for (Int_t i=10; i<20; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));
   else if (setID==3)
      for (Int_t i=20; i<30; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));
   else if (setID==4)
      for (Int_t i=30; i<40; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));
   else if (setID==5)
      for (Int_t i=40; i<50; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));
   else if (setID==6)
      for (Int_t i=50; i<60; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));
   else if (setID==7)
      for (Int_t i=60; i<70; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));
   else if (setID==8)
      for (Int_t i=70; i<80; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));
   else if (setID==9)
      for (Int_t i=80; i<88; i++)
         pstr->AddFile(inputdir+Form("Cd00%d.root",i));

   Int_t Cha_MCAEnergy[Ncha];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // get cut = Eseg/Ecore
   TFile* fmca = new TFile("Run5mcanocut.root");
   TH1F* SOC[Nseg]={0};
   Double_t cut[Nseg]={0};
   for (Int_t i=1; i<Nseg; i++) {
      SOC[i] = (TH1F*) fmca->Get(Form("SOC%d",i));
      SOC[i]->Draw();

      TSpectrum soc(2);
      cout<<"number of peaks found = "<<soc.Search(SOC[i],0.1,"",0.5)<<endl;
      cut[i] = soc.GetPositionX()[0];
      cout<<"the first peak position = "<<cut[i]<<endl;
   }


   // output
   TFile* foutput = new TFile(inputdir+Form("Run5pre%d.root",setID),"recreate");
   TTree *newtree = pstr->CloneTree(0);

   // main loop
   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      Int_t Nsegs=0, firedSeg=0;
      for (Int_t j=1; j<Ncha-1; j++)
         if (Cha_MCAEnergy[j]>1000) {
            Nsegs++;
            firedSeg=j;
         }

      Float_t Ecor = (Float_t) Cha_MCAEnergy[0];
      if (Nsegs==1 && Ecor>0) {   // if single segment event
         Float_t Eseg = (Float_t) Cha_MCAEnergy[firedSeg];
         Float_t Esoc = Eseg/Ecor;

         if (firedSeg==1 || firedSeg==2 || firedSeg==3)
            if (fabs(Esoc-cut[firedSeg])<0.05 &&  // if Eseg = Ecore
                  (Cha_MCAEnergy[firedSeg]>1500 && 
                   Cha_MCAEnergy[firedSeg]<4500))
               newtree->Fill();     
         
         if (firedSeg==16 || firedSeg==17 || firedSeg==18)
            if (fabs(Esoc-cut[firedSeg-12])<0.05 &&  // if Eseg = Ecore
                  (Cha_MCAEnergy[firedSeg]>1500 && 
                   Cha_MCAEnergy[firedSeg]<4500))
               newtree->Fill();     
      }

      if (i%10000==0) newtree->Write("", TObject::kWriteDelete);
   }
   
   newtree->Write("", TObject::kWriteDelete);
   foutput->Close();

   delete foutput;
} 

//=============================================================================

void spectra() // create energy spectra of 88 keV peaks after cuts
{
   // read in parameters
   Float_t ADC[Nseg]  = {0};
   Float_t dADC[Nseg] = {0};

   Float_t ADCw[Nseg]  = {0};
   Float_t dADCw[Nseg] = {0};

   Float_t N[Nseg]  = {0};
   Float_t dN[Nseg] = {0};

   ifstream dat;
   dat.open("Run5cali.txt");
   cout<<"cha \t ADC \t dADC \t ADCw \t dADCw \t N \t dN"<<endl;
   for (Int_t i=1; i<Nseg; i++) {
      dat>>ADC[i]>>dADC[i]>>ADCw[i]>>dADCw[i]>>N[i]>>dN[i];
      cout<<i<<" \t "<<ADC[i]<<" \t "<<dADC[i]<<" \t "
         <<ADCw[i]<<" \t "<<dADCw[i]<<" \t "
         <<N[i]<<" \t "<<dN[i]<<endl;
   }
   dat.close();

   // input
   TChain* pstr = new TChain("PSTree","root"); 
   for (Int_t i=1; i<=9; i++)
      pstr->AddFile(inputdir+Form("Run5pre%d.root",i));

   Int_t Cha_MCAEnergy[Ncha];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // define MCA histograms
   TH1F *MCA[Nseg]={0};
   for (Int_t i=1; i<=3; i++)
      MCA[i] = new TH1F(Form("MCA%d",i),
            Form("Segment %d",i),80,68,108);
   for (Int_t i=4; i<=6; i++)
      MCA[i] = new TH1F(Form("MCA%d",i),
            Form("Segment %d (channel %d)",i,i+12),80,68,108);

   // main loop
   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      for (Int_t j=1; j<=3; j++)
         MCA[j]->Fill(((Float_t) Cha_MCAEnergy[j])/ADC[j]*88);
      for (Int_t j=4; j<=6; j++)
         MCA[j]->Fill(((Float_t) Cha_MCAEnergy[j+12])/ADC[j]*88);
   }

   // define fit function and parameters
   TF1 *FitFcn = new TF1("FitFcn","[0]*gausn(1)+pol1(4)");
   FitFcn->SetParNames("bin width","events","mean","sigma","pol0","pol1");
  
   Float_t events[Nseg] = {1e3,1e3,1e3,1e3,1e3,1e3,1e3};
   Float_t mean[Nseg]   = {88, 88, 88, 88, 88, 88, 88};
   Float_t sigma[Nseg]  = {2,2,2,2,2,2,2};
   Float_t range[Nseg]  = {10,10,10,10,10,10,10};
   Float_t pol0[Nseg]   = {0};
   Float_t pol1[Nseg]   = {0};

   Float_t xlow, xup;
   
   Float_t Nbg[Nseg] = {0};
   Float_t SoN[Nseg] = {0};

   // fit peaks in each channel (no core)
   TCanvas *c2 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("c2");
   if (!c2)  c2 = new TCanvas("c2","",0,0,1200,800);
   c2 -> Clear();
   c2->Divide(3,2);
   ofstream dson;
   dson.open("Run5SoN.txt");
   for (Int_t i=1; i<Nseg; i++) {
      xlow = mean[i]-range[i];
      xup  = mean[i]+range[i];      

      FitFcn->SetParameters(1.0,events[i],mean[i],sigma[i],pol0[i],pol1[i]);
      FitFcn->FixParameter(0,MCA[i]->GetBinWidth(1));

      c2->cd(i);
      MCA[i]->SetAxisRange(xlow,xup);
      MCA[i]->Fit("FitFcn","","",xlow,xup);

      N[i] = FitFcn->GetParameter("events");
      dN[i] = FitFcn->GetParError(1);

      Nbg[i] = FitFcn->Integral(mean[i]-3*sigma[i],mean[i]+3*sigma[i])/
         MCA[i]->GetBinWidth(1)-N[i];

      SoN[i] = N[i]/Nbg[i];

      cout<<N[i]<<" \t "<<Nbg[i]<<" \t "<<SoN[i]<<endl;
      dson<<N[i]<<" \t "<<Nbg[i]<<" \t "<<SoN[i]<<endl;
   }
   dson.close();
 
   // output
   TFile* foutput = new TFile("Run5specta.root","recreate");
   for (Int_t i=1; i<Nseg; i++) MCA[i]->Write();
   foutput->Close();

   //cleanup
   delete foutput;
   delete pstr;
}

//=============================================================================

void final() // make strict energy cut, the output is ready for converting 
{
   // read in parameters
   Float_t ADC[Nseg]  = {0};
   Float_t dADC[Nseg] = {0};

   Float_t ADCw[Nseg]  = {0};
   Float_t dADCw[Nseg] = {0};

   Float_t N[Nseg]  = {0};
   Float_t dN[Nseg] = {0};

   ifstream dat;
   dat.open("Run5cali.txt");
   cout<<"cha \t ADC \t dADC \t ADCw \t dADCw \t N \t dN"<<endl;
   for (Int_t i=1; i<Nseg; i++) {
      dat>>ADC[i]>>dADC[i]>>ADCw[i]>>dADCw[i]>>N[i]>>dN[i];
      cout<<i<<" \t "<<ADC[i]<<" \t "<<dADC[i]<<" \t "
         <<ADCw[i]<<" \t "<<dADCw[i]<<" \t "
         <<N[i]<<" \t "<<dN[i]<<endl;
   }
   dat.close();

   // input
   TChain* pstr = new TChain("PSTree","root"); 
   for (Int_t i=1; i<=9; i++)
      pstr->AddFile(inputdir+Form("Run5pre%d.root",i));

   Int_t Cha_MCAEnergy[20];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   // output
   TFile* foutput = new TFile("Run5small.root","recreate");
   TTree *newtree = pstr->CloneTree(0);

   // main loop
   Int_t Nevt = pstr->GetEntries();
   cout<<"total events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) cout<<"now event: "<<i<<endl;

      pstr->GetEntry(i);

      Int_t Nsegs=0;
      for (Int_t j=1; j<=3; j++)
         if (fabs((Float_t) Cha_MCAEnergy[j]-ADC[j])<=ADCw[j]) Nsegs++;

      for (Int_t j=4; j<Nseg; j++)
         if (fabs((Float_t) Cha_MCAEnergy[j+12]-ADC[j])<=ADCw[j]) Nsegs++;

      if (Nsegs==1) newtree->Fill();
   } 

   newtree->Write("", TObject::kWriteDelete);
   foutput->Close();

   delete foutput;
   delete pstr;
}

//=============================================================================
