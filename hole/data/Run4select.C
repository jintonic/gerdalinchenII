#include <iostream>
#include <fstream>

#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"

using namespace std;

void pre(Int_t setID)
{
   TChain* pstr = new TChain("PSTree","root"); 
   if (setID==1){
      for (Int_t i=1; i<10; i++)
	 pstr->AddFile(Form("/remote/pclg-10/SIEGFRIED_II/Cd_SII_PS/Cd_In_SII_PS000%d.root",i));
      for (Int_t i=70; i<85; i++)
	 pstr->AddFile(Form("/remote/pclg-10/SIEGFRIED_II/Cd_SII_PS/Cd_In_SII_PS00%d.root",i));
   }
   else if (setID==2)
      for (Int_t i=10; i<30; i++)
	 pstr->AddFile(Form("/remote/pclg-10/SIEGFRIED_II/Cd_SII_PS/Cd_In_SII_PS00%d.root",i));
   else if (setID==3)
      for (Int_t i=30; i<50; i++)
	 pstr->AddFile(Form("/remote/pclg-10/SIEGFRIED_II/Cd_SII_PS/Cd_In_SII_PS00%d.root",i));
   else if (setID==4)
      for (Int_t i=50; i<70; i++)
	 pstr->AddFile(Form("/remote/pclg-10/SIEGFRIED_II/Cd_SII_PS/Cd_In_SII_PS00%d.root",i));

   Int_t Cha_MCAEnergy[8];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   TFile* foutput = new TFile(Form("Run4pre%d.root",setID),"recreate");
   TTree *newtree = pstr->CloneTree(0);

   const Int_t Ncha = 7;

   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) 
	 cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      Int_t Nseg=0;
      for (Int_t j=1; j<Ncha-1; j++) {
	 if (Cha_MCAEnergy[j]>1000 && Cha_MCAEnergy[j]<7000) {
	    Nseg++;
	 }
      }
      if (Nseg==1) {
	 newtree->Fill();
      }
      if (i%10000==0) newtree->Write();
   }
   
   newtree->Write();
   foutput->Close();
} 

void again()
{
   TChain* pstr = new TChain("PSTree","root"); 
   for (Int_t i=1; i<5; i++) pstr->AddFile(Form("Run4pre%d.root",i));

   Int_t Cha_MCAEnergy[8];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   TFile* foutput = new TFile("Run4selected.root","recreate");
   TTree *newtree = pstr->CloneTree(0);

   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) 
	 cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      Int_t Nseg=0;
      if (Cha_MCAEnergy[1]>3800 && Cha_MCAEnergy[1]<5200) {
	 Nseg++;
      }
      for (Int_t j=2; j<=3; j++) {
	 if (Cha_MCAEnergy[j]>2200 && Cha_MCAEnergy[j]<3200) {
	    Nseg++;
	 }
      }
      for (Int_t j=4; j<=5; j++) {
	 if (Cha_MCAEnergy[j]>3000 && Cha_MCAEnergy[j]<4400) {
	    Nseg++;
	 }
      }

      if (Nseg==1) {
	 newtree->Fill();
      }
   }
   
   newtree->Write();
   foutput->Close();
    
}

void mca()
{
   TChain* pstr = new TChain("PSTree","root"); 
   pstr->AddFile("Run4selected.root");

   Int_t Cha_MCAEnergy[8];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   const Int_t Ncha = 7;
   TH1F *MCA[Ncha];
   MCA[0] = new TH1F("MCA0","MCA Spectrum for the core",3000,1000,7000);
   MCA[1] = new TH1F("MCA1","MCA Spectrum for segment 1",700,3800,5200);
   for (Int_t i=2; i<=3; i++)
      MCA[i] = new TH1F(Form("MCA%d",i),Form("MCA Spectrum for segment %d",i),500,2200,3200);
   for (Int_t i=4; i<=5; i++)
      MCA[i] = new TH1F(Form("MCA%d",i),Form("MCA Spectrum for segment %d",i+12),700,3000,4400);
   MCA[6] = new TH1F("MCA6","MCA Spectrum for segment 18",10000,1000,21000);

   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) 
	 cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      for (Int_t j=0; j<Ncha; j++)
	 MCA[j]->Fill((Float_t) Cha_MCAEnergy[j]);
   }
 
   TFile* foutput = new TFile("Run4mca.root","recreate");
   for (Int_t i=0; i<Ncha; i++) MCA[i]->Write();
   foutput->Close();
}

void fitmca()
{
   const Int_t Ncha = 6;
   TH1F *MCA[Ncha];

   TFile* fmca = new TFile("Run4mca.root");
   for (Int_t i=0; i<Ncha; i++) {
      fmca->GetObject(Form("MCA%d",i),MCA[i]);
      MCA[i]->Rebin(10);
   }

   TF1 *FitFcn = new TF1("FitFcn","[0]*gausn(1)+pol1(4)");
   FitFcn->SetParNames("bin width","events","mean","sigma","pol0","pol1");
  
   Float_t events[Ncha] = {1e3,1e3,1e3,1e3,1e3,1e3};
   Float_t mean[Ncha]   = {1100,4477,2670,2730,3672,3610};
   Float_t sigma[Ncha]  = {150,150,150,150,150,150};
   Float_t range[Ncha]  = {550,550,550,550,550,550};
   Float_t pol0[Ncha]   = {0};
   Float_t pol1[Ncha]   = {0};

   Float_t xlow, xup;
   
   Float_t N[Ncha]  = {0};
   Float_t dN[Ncha] = {0};

   Float_t ADC[Ncha]  = {0};
   Float_t dADC[Ncha] = {0};

   Float_t ADCw[Ncha]  = {0};
   Float_t dADCw[Ncha] = {0};

   TCanvas *c2 = new TCanvas ("c2","",0,0,1200,400);
   c2->Divide(5,1);
   ofstream dat;
   dat.open("Run4Nseg.txt");
   for (Int_t i=1; i<Ncha; i++) {
      xlow = mean[i]-range[i];
      xup  = mean[i]+range[i];      

      FitFcn->SetParameters(1.0,events[i],mean[i],sigma[i],pol0[i],pol1[i]);
      FitFcn->FixParameter(0,MCA[i]->GetBinWidth(1));

      c2->cd(i);
      c2->Update();
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

void morestrict()
{
   const Int_t Ncha = 6;
 
   Float_t N[Ncha]  = {0};
   Float_t dN[Ncha] = {0};

   Float_t ADC[Ncha]  = {0};
   Float_t dADC[Ncha] = {0};

   Float_t ADCw[Ncha]  = {0};
   Float_t dADCw[Ncha] = {0};

   ifstream dat;
   dat.open("Run4Nseg.txt");
   cout<<"channel \t ADC \t dADC \t ADCw \t dADCw \t N \t dN"<<endl;
   for (Int_t i=1; i<Ncha; i++) {
      dat>>ADC[i]>>dADC[i]>>ADCw[i]>>dADCw[i]>>N[i]>>dN[i];
      cout<<i<<" \t "<<ADC[i]<<" \t "<<dADC[i]<<" \t "
	  <<ADCw[i]<<" \t "<<dADCw[i]<<" \t "
	  <<N[i]<<" \t "<<dN[i]<<endl;
   }
   dat.close();

   TChain* pstr = new TChain("pstree","root"); 
   pstr->AddFile("Run4selected.root");

   Int_t Cha_MCAEnergy[8];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   TFile* foutput = new TFile("Run4smaller.root","recreate");
   TTree *newtree = pstr->CloneTree(0);

   Int_t Nevt = pstr->GetEntries();
   cout<<"total events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) 
	 cout<<"now event: "<<i<<endl;

      pstr->GetEntry(i);

      Int_t Nseg=0;
      for (Int_t j=1; j<Ncha; j++) {
	 if (Cha_MCAEnergy[j]>ADC[j]-ADCw[j] && 
	     Cha_MCAEnergy[j]<ADC[j]+ADCw[j]) {
	    Nseg++;
	 }
      }

      if (Nseg==1) {
	 newtree->Fill();
      }
   }
   
   newtree->Write();
   foutput->Close();
}

void final()
{
   TChain* pstr = new TChain("PSTree","root"); 
   pstr->AddFile("Run4smaller.root");

   Int_t Cha_MCAEnergy[8];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   TFile* foutput = new TFile("Run4ps.root","recreate");
   TTree *newtree = pstr->CloneTree(0);

   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) 
	 cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      if (Cha_MCAEnergy[0]>1353-64 && Cha_MCAEnergy[0]<1353+64) {
	 newtree->Fill();
      }
   }
   
   newtree->Write();
   foutput->Close();
}

void segps()
{
   TChain* pstr = new TChain("PSTree","root"); 
   pstr->AddFile("Run4ps.root");

   Int_t Cha_MCAEnergy[8];
   pstr->SetBranchAddress("Cha_MCAEnergy",Cha_MCAEnergy);

   const Int_t Ncha=6;
   TFile *foutput[Ncha];
   TTree *newtree[Ncha];
   for (Int_t i=1; i<Ncha; i++) {
      foutput[i] = new TFile(Form("Run4PSseg%d.root",i),"recreate");
      newtree[i] = pstr->CloneTree(0);
   }

   Int_t Nevt = pstr->GetEntries();
   cout<<"Total Events: "<<Nevt<<endl;
   
   for (Int_t i=0; i<Nevt;i++) {
      if (i%10000==0) 
	 cout<<"Now event: "<<i<<endl;

      pstr->GetEntry(i);

      for (Int_t i=1; i<Ncha; i++) {
	 if (Cha_MCAEnergy[i]>2000 && Cha_MCAEnergy[i]<5000)
	    newtree[i]->Fill();
      }
   }
   
   for (Int_t i=1; i<Ncha; i++) {
      foutput[i]->cd();
      newtree[i]->Write();
   }
   for (Int_t i=1; i<Ncha; i++) 
      foutput[i]->Close();
}

void occupancy()
{
   const Int_t Ncha = 6;
 
   Float_t N[Ncha]  = {0};
   Float_t dN[Ncha] = {0};

   Float_t ADC[Ncha]  = {0};
   Float_t dADC[Ncha] = {0};

   Float_t ADCw[Ncha]  = {0};
   Float_t dADCw[Ncha] = {0};

   ifstream dat;
   dat.open("Run4Nseg.txt");
   for (Int_t i=1; i<Ncha; i++) {
      dat>>ADC[i]>>dADC[i]>>ADCw[i]>>dADCw[i]>>N[i]>>dN[i];
      cout<<ADC[i]<<" \t "<<dADC[i]<<" \t "
	  <<ADCw[i]<<" \t "<<dADCw[i]<<" \t "
	  <<N[i]<<" \t "<<dN[i]<<endl;
   }
   dat.close();
}
