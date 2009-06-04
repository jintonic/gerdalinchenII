seg13()
{   
   const int n = 18;

   float x[n];
   float ex[n];
   float ey[n] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,97,76,197};
   float y[n]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,442,508,633};
   float z[n]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,124,101,183};
   

   for(int i=0; i<18; i++){
     x[i] =(i+1)*5; //the angle
     ex[i]=0;
     
  }
  
   
   TGraph *gr = new TGraph(n,x,z);
   TGraphErrors *gre = new TGraphErrors(n,x,y,ex,ey);
  
   TMultiGraph *mg = new TMultiGraph();
   mg->Add(gr,"lp");
   mg->Add(gre,"cp");

   mg->GetXaxis()->SetTitle("angle");
   mg->GetYaxis()->SetTitle("#envents/Sigma"); 

   mg->Draw("a");
}
