const Int_t Nscan = 20;
Int_t angle[Nscan] = {5,10,15,20,25,30,35,40,45,50,55,60,65,70,73,75,77,80,85,90};

void vfit(Int_t theAngle=0)
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
   A0Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/super/phi%d.root",theAngle));

   cc1->cd(1);
   A0Tree->Draw("Cha_MCAEnergy[0]>>hdat0(200,2300.0,4000.0)");
   TF1 *FitFcn0 = new TF1("FitFcn0","[0]*gausn(1)+pol2(4)");
   FitFcn0->SetParNames("bin width","events","mean","sigma","pol0","pol1","pol2");

   // fit data
   FitFcn0->SetParameters(1.0,5000.0,3050.0,5.0,0,0,0);
   FitFcn0->FixParameter(0,hdat0->GetBinWidth(1));
   
   hdat0->Fit("FitFcn0");

   Int_t N_core = FitFcn0->GetParameter("events");
   Int_t dN_core= FitFcn0->GetParError(1);

//=============================================================

   TChain *A13Tree = new TChain("PSTree");
   A13Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/super/phi%d.root",theAngle));

   cc1->cd(2);
   A13Tree->Draw("Cha_MCAEnergy[13]>>hdat13(200,2900.0,5100.0)");
   TF1 *FitFcn13 = new TF1("FitFcn13","[0]*gausn(1)+pol2(4)");
   FitFcn13->SetParNames("bin width","events","mean","sigma","pol0","pol1","pol2");

   // fit data
   FitFcn13->SetParameters(1.0,5000.0,3450.0,6.0,0,0,0);
   FitFcn13->FixParameter(0,hdat13->GetBinWidth(1));
   
   hdat13->Fit("FitFcn13");

   Int_t N_13 = FitFcn13->GetParameter("events");
   Int_t dN_13= FitFcn13->GetParError(1);

//=============================================================
   TChain *A14Tree = new TChain("PSTree");
   A14Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/super/phi%d.root",theAngle));

   cc1->cd(3);
   A14Tree->Draw("Cha_MCAEnergy[14]>>hdat14(200,2900.0,5100.0)");
   TF1 *FitFcn14 = new TF1("FitFcn14","[0]*gausn(1)+pol2(4)");
   FitFcn14->SetParNames("bin width","events","mean","sigma","pol0","pol1","pol2");

   // fit data
   FitFcn14->SetParameters(1.0,5000.0,3700.0,5.0,0,0,0);
   FitFcn14->FixParameter(0,hdat14->GetBinWidth(1));
   
   hdat14->Fit("FitFcn14");

   Int_t N_14 = FitFcn14->GetParameter("events");
   Int_t dN_14= FitFcn14->GetParError(1);
//=============================================================

   TChain *A15Tree = new TChain("PSTree");
   A15Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/super/phi%d.root",theAngle));

   cc1->cd(4);
   A15Tree->Draw("Cha_MCAEnergy[15]>>hdat15(200,2900.0,5100.0)");
   TF1 *FitFcn15 = new TF1("FitFcn15","[0]*gausn(1)+pol2(4)");
   FitFcn15->SetParNames("bin width","events","mean","sigma","pol0","pol1","pol2");

   // fit data
   FitFcn15->SetParameters(1.0,5000.0,3700.0,25.0,0,0,0);
   FitFcn15->FixParameter(0,hdat15->GetBinWidth(1));
   
   hdat15->Fit("FitFcn15");

   Int_t N_15 = FitFcn15->GetParameter("events");
   Int_t dN_15 = FitFcn15->GetParError(1);
   
//=============================================================
   cout<<""<<theAngle<<"    &"<<N_core<<" $\\pm$ "<<dN_core<<"    &"<<N_13<<" $\\pm$ "<<dN_13<<"    &"<<N_14<<" $\\pm$ "<<dN_14<<"    &"<<N_15<<" $\\pm$ "<<dN_15<<"    &\\\\ \\hline"<<endl;

   cout<<" total number of events at "<<theAngle<<" degrees in the core "
       <<N_core<<" plus-minus "<<dN_core<<endl;

   cout<<" total number of events at "<<theAngle<<" degrees in seg  13 "
       <<N_13<<" plus-minus "<<dN_13<<endl;

   cout<<" total number of events at "<<theAngle<<" degrees in seg  14 "
       <<N_14<<" plus-minus "<<dN_14<<endl;

    cout<<" total number of events at "<<theAngle<<" degrees in seg  15 "
	<<N_15<<" plus-minus "<<dN_15<<endl;

}
