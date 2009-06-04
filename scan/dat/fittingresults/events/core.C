core()
{   
   const int n = 18;

   float x[n];
   float ex[n];
   float ey[n] = {60,60,62,60,71,47,74,76,59,61,66,69,66,44,54,73,58};
   float y[n] = {406,438,390,321,336,445,285,490,446,395,442,452,602,529,350,454,663,425};

   for(int i=0; i<18; i++){
     x[i] =(i+1)*5; //the angle
     ex[i]=0;
    
   }
  
   
   
   TGraphErrors *gre = new TGraphErrors(n,x,y,ex,ey);
  

   gre->Draw("alp");
   gre->GetXaxis()->SetTitle("angle");
   gre->GetYaxis()->SetTitle("#envents"); 
}
