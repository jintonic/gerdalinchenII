// an example to fit peaks.
void fit() 
{
   TFile* fdat = new TFile("hdat.root");
   TH1F*  hdat = (TH1F*) fdat->Get("hdat"); 
   
   TF1 *FitFcn = new TF1("FitFcn","[0]*gausn(1)+pol2(4)");
   FitFcn->SetParNames("bin width","events","mean","sigma","pol0","pol1","pol2");

   // fit data
   FitFcn->SetParameters(1.0,5e3,121,1,0,0,0);
   FitFcn->FixParameter(0,hdat->GetBinWidth(1));
   
   hdat->Fit("FitFcn","","",110,130);

   Double_t N = FitFcn->GetParameter("events");
   Double_t dN= FitFcn->GetParError(1);

}


