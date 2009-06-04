seg15()
{   
   const int n = 18;

   float x[n];
   float ex[n];
   float ey[n] = {105,119,70,132,90,0,0,0,0,0,0,0,0,0,0,0,0,0};
   float y[n]  = {552,581,145,175,147,0,0,0,0,0,0,0,0,0,0,0,0,0};
   float z[n]  = {150,137,90,176,171,0,0,0,0,0,0,0,0,0,0,0,0,0};
   

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
