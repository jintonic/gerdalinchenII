void Ba133()
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
         if (Nseg==1) singleSeg->Fill(segEnergy[1][0]);
       
      }
   }
   // save histograms
   TFile* f1 = new TFile("Ba133plot.root","recreate");
   f1->cd();
   singleSeg->Write();
   f1->Close();
}

