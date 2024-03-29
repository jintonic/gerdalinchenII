const Int_t Nscan = 20;
Int_t angle[Nscan] = {5,10,15,20,25,30,35,40,45,50,55,60,65,70,73,75,77,80,85,90};

void fit(Int_t theAngle=0)
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

   TCanvas *cc1 = new TCanvas("cc1","cc1",800,800);
   cc1->Divide(2,2,0.0001,0.0001);

//=============================================================
   TChain *A0Tree = new TChain("PSTree");
   A0Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/1408kev/phi%d.root",theAngle));

   cc1->cd(1);
   A0Tree->Draw("Cha_MCAEnergy[0]>>hdat0(150,34700.0,35900.0)");
   TF1 *FitFcn0 = new TF1("FitFcn0","[0]*gausn(1)");
   FitFcn0->SetParNames("bin width","events","mean","sigma");

   // fit data
   FitFcn0->SetParameters(1.0,1000.0,35300.0,5.0);
   FitFcn0->FixParameter(0,hdat0->GetBinWidth(1));
      
   hdat0->Fit("FitFcn0");

   Int_t N_core = FitFcn0->GetParameter("events");
   Int_t dN_core= FitFcn0->GetParError(1);
   Int_t sig_core = FitFcn0->GetParameter("sigma");
//=============================================================

   TChain *A13Tree = new TChain("PSTree");
   A13Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/1408kev/phi%d.root",theAngle));

   cc1->cd(2);
   A13Tree->Draw("Cha_MCAEnergy[13]>>hdat13(150,39300.0,41100.0)");
   TF1 *FitFcn13 = new TF1("FitFcn13","[0]*gausn(1)");
   FitFcn13->SetParNames("bin width","events","mean","sigma");

   // fit data
   FitFcn13->SetParameters(1.5,100.0,40200.0,3335.0);
   FitFcn13->FixParameter(0,hdat13->GetBinWidth(1));
   
   hdat13->Fit("FitFcn13");

   Int_t N_13 = FitFcn13->GetParameter("events");
   Int_t dN_13= FitFcn13->GetParError(1);
   Int_t sig_13 = FitFcn13->GetParameter("sigma");

//=============================================================
   TChain *A14Tree = new TChain("PSTree");
   A14Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/1408kev/phi%d.root",theAngle));

   cc1->cd(3);
   A14Tree->Draw("Cha_MCAEnergy[14]>>hdat14(150,43500.0,45000.0)");
   TF1 *FitFcn14 = new TF1("FitFcn14","[0]*gausn(1)");
   FitFcn14->SetParNames("bin width","events","mean","sigma");

   // fit data
   FitFcn14->SetParameters(1.0,500.0,44250.0,118.0);
   FitFcn14->FixParameter(0,hdat14->GetBinWidth(1));
   
   hdat14->Fit("FitFcn14");

   Int_t N_14 = FitFcn14->GetParameter("events");
   Int_t dN_14= FitFcn14->GetParError(1);
   Int_t sig_14 = FitFcn14->GetParameter("sigma");
//=============================================================

   TChain *A15Tree = new TChain("PSTree");
   A15Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/1408kev/phi%d.root",theAngle));

   cc1->cd(4);
   A15Tree->Draw("Cha_MCAEnergy[15]>>hdat15(150,43700.0,45200.0)");
   TF1 *FitFcn15 = new TF1("FitFcn15","[0]*gausn(1)");
   FitFcn15->SetParNames("bin width","events","mean","sigma");

   // fit data
   FitFcn15->SetParameters(1.5,500.0,44450.0,25.0);
   FitFcn15->FixParameter(0,hdat15->GetBinWidth(1));
   
   hdat15->Fit("FitFcn15");

   Int_t N_15 = FitFcn15->GetParameter("events");
   Int_t dN_15 = FitFcn15->GetParError(1);
   Int_t sig_15 = FitFcn15->GetParameter("sigma");
   
//=============================================================
   cout<<""<<theAngle<<"    &"<<N_core<<" $\\pm$ "<<dN_core<<"    &"<<sig_core<<"    &"<<N_13<<" $\\pm$ "<<dN_13<<"    &"<<sig_13<<"    &"<<N_14<<" $\\pm$ "<<dN_14<<"    &"<<sig_14<<"   &"<<N_15<<" $\\pm$ "<<dN_15<<"    &"<<sig_15<<"   & \\hline"<<endl;

   cout<<" total number of events at "<<theAngle<<" degrees in the core "
       <<N_core<<" plus-minus "<<dN_core<<";  sigma = "<<sig_core<<endl;

   cout<<" total number of events at "<<theAngle<<" degrees in seg  13 "
       <<N_13<<" plus-minus "<<dN_13<<";  sigma = "<<sig_13<<endl;

   cout<<" total number of events at "<<theAngle<<" degrees in seg  14 "
       <<N_14<<" plus-minus "<<dN_14<<";  sigma = "<<sig_14<<endl;

    cout<<" total number of events at "<<theAngle<<" degrees in seg  15 "
	<<N_15<<" plus-minus "<<dN_15<<";  sigma = "<<sig_15<<endl;

}
