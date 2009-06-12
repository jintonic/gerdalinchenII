plot()
{   
   const int n = 18;

   float x[n];
   float ex[n];
   float y13[n] =  {125,154,154,152,109,43,64,57,55,50,61,136,135,159,140,145,136,150};
   float y14[n]  = {146,137,190,157,105,54,59,62,52,55,59,158,136,137,138,133,136,143};
   float y15[n]  = {152,130,179,172,108,57,53,50,46,57,47,158,148,150,158,222,170,249};
   

   for(int i=0; i<18; i++){
     x[i] =(i+1)*5; //the angle
     ex[i]=0;
     
  }
  
   
   TGraph *gr13 = new TGraph(n,x,y13);
   TGraph *gr14 = new TGraph(n,x,y14);
   TGraph *gr15 = new TGraph(n,x,y15);

   TMultiGraph *mg = new TMultiGraph();
   mg->Add(gr13,"lp");
   mg->Add(gr14,"cp");
   mg->Add(gr15,"cp");
    
  //  mg->GetXaxis()->SetTitle("angle");
  //  mg->GetYaxis()->SetTitle("FWHM");
   mg->Draw("a");
}
