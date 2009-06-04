seg14()
{   
   const int n = 18;

   float x[n];
   float ex[n];
   float ey[n] = {0,0,127,155,58,42,43,40,43,39,43,71,84,107,87,0,0,0};
   float y[n]  = {0,0,164,482,307,436,466,432,488,515,572,317,423,445,62,0,0,0};
   float z[n]  = {0,0,173,164,77,47,52,44,47,43,51,104,129,146,43,0,0,0};
   

   for(int i=0; i<18; i++){
     x[i] =(i+1)*5; //the angle
     ex[i]=0;
     
  }
  
   
   TGraph *gr = new TGraph(n,x,z);
   TGraphErrors *gre = new TGraphErrors(n,x,y,ex,ey);
  
      gr->Draw("alp");
      gre->Draw("alp");
      gre->GetXaxis()->SetTitle("angle");
      gre->GetYaxis()->SetTitle("#envents/FWHM"); 
}
