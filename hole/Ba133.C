void Ba133()
{
//   hist();
   fitting();
}

void hist()
{
   Int_t    nbinsx = 880;
   Double_t xlow   = 10;
   Double_t xup    = 450;

   if (gDirectory->Get("singleSeg")) delete singleSeg;
   TH1F *singleSeg = new TH1F("singleSeg","",nbinsx,xlow,xup);

   TChain* fTree = new TChain("fTree","root");
   fTree->AddFile("Ba133.root");
   
   Float_t segEnergy[4][20];
   fTree->SetBranchAddress("segEnergy",segEnergy);

   Int_t Neve=fTree->GetEntries();
   cout<<" total entries = "<<Neve<<endl;
   
   for (Int_t i=0; i<Neve; i++) {
      if (i%10000==0) cout<<" now event "<<i<<endl;
      fTree->GetEntry(i);

      Int_t Nseg=0;
      for (Int_t j=1; j<=18; j++) {
         if (segEnergy[1][j]>5) {
            Nseg += 1;
         }
         if (Nseg==1) singleSeg->Fill(smearE(segEnergy[1][0]));
      }
   }
   // save histograms
   TFile* f1 = new TFile("Ba133plot.root","recreate");
   f1->cd();
   singleSeg->Write();
   f1->Close();
}

Float_t smearE(Float_t E)
{
   Float_t sig= (1.30511+0.000995415*E)/2.355; //FWHM/2.35=Sigma
   
   //because sig is too small, 
   //we *100 & /100 to avoid strange things happen during the calculation:
   Float_t dE = gRandom -> Gaus(0.0, sig*100.0)/100.0; 
   
   return E+dE;
}

void fitting() 
{
   TFile* fdat = new TFile("Ba133plot.root");
   TH1F*  singleSeg = (TH1F*) fdat->Get("singleSeg"); 
   
   TF1 *FitFcn = new TF1("FitFcn","[0]*gausn(1)+pol1(4)");
   FitFcn->SetParNames("bin width","events","mean","sigma","pol0","pol1");
  
   const Int_t n = 2;
   Double_t events[n] = {1e3,1e4};
   Double_t mean[n]   = {81,356};
   Double_t sigma[n]  = {1., 1.};
   Double_t range[n]  = {10, 10};

   Double_t pol0[n]   = {0};
   Double_t pol1[n]   = {0};

   Double_t xlow, xup;
   
   Double_t N[n]  = {0};
   Double_t dN[n] = {0};

   Double_t E[n]  = {0};
   Double_t dE[n] = {0};

   for (Int_t i=0; i<n; i++) {
      xlow = mean[i]-range[i];
      xup  = mean[i]+range[i];

      FitFcn->SetParameters(1.0,events[i],mean[i],sigma[i],pol0[i],pol1[i]);
      FitFcn->FixParameter(0,singleSeg->GetBinWidth(1));

      singleSeg->SetAxisRange(xlow,xup);
      singleSeg->Fit("FitFcn","","",xlow,xup);

      N[i] = FitFcn->GetParameter("events");
      dN[i] = FitFcn->GetParError(1);

      E[i] = FitFcn->GetParameter("mean");
      dE[i] = FitFcn->GetParError(2);
   }

   cout<<"Nevent 81 / Nevent 356 = "<<N[0]/N[1]<<" +- "
       <<sqrt(dN[0]/N[0]*dN[0]/N[0]+dN[1]/N[1]*dN[1]/N[1])*N[0]/N[1]<<endl;
}
