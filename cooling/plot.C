//#include <sstream>
//#include <fstream>
//using namespace std;
//using namespace TMath;

void plot()
{
//   filling();
   dropping();
}

void filling()
{
   const Int_t n = 3452;
   Float_t ts[n],t[n],p1[n],p2[n],p3[n],p4[n],p5[n],bottom[n],xBottom[n],xTop[n],xTmB[n];
   ifstream record;
   string line;
   
   record.open("coolingTestRecord1.txt");
   Int_t i=0;
   while (record.good()) {
      getline(record,line);
      istrstream s(line);
      s>>ts[i]>>p1[i]>>p2[i]>>p3[i]>>p4[i]>>p5[i]>>bottom[i]>>xBottom[i]>>xTop[i];
      xTmB[i]=xTop[i]-xBottom[i];
      t[i]=ts[i]/60;
      i++;
   }
   record.close();
   
   gROOT->Macro("style.C");      
   MyStyle->SetTitleOffset(1.3,"y");
   MyStyle->cd();

   TCanvas *c1 = new TCanvas ("c1","",0,0,1000,800);
   TGraph *xb = new TGraph(n,t,xBottom);
   TGraph *xt = new TGraph(n,t,xTop);
   TGraph *xtmb = new TGraph(n,t,xTmB);

   xb->SetTitle("");
   xb->GetXaxis()->SetTitle("Time [Minute]");
   xb->GetXaxis()->CenterTitle();
   xb->GetYaxis()->SetTitle("Temperature [Celsius]");
   xb->GetYaxis()->CenterTitle();
   xb->GetYaxis()->SetRangeUser(-200,60);
   xb->SetLineColor(kRed);
   xb->Draw("al");
   xt->SetLineColor(kBlue);
   xt->Draw("l");
   xtmb->Draw("l");

   TLegend* leg = new TLegend(0.6, 0.35, 0.85, 0.65);
   leg->AddEntry(xtmb,"Top - Bottom","l");
   leg->AddEntry(xt,"Top PT100","l");
   leg->AddEntry(xb,"Bottom PT100","l");
   leg->SetLineColor(kWhite);
   leg->SetTextFont(132);
   leg->Draw();
}

void dropping()
{
   const Int_t n = 81675;
   Float_t ts[n],t[n],p1[n],p2[n],p3[n],p4[n],p5[n],bottom[n],xBottom[n],xTop[n],xTmB[n];
   ifstream record;
   string line;
   
   record.open("coolingTestRecord2.txt");
   Int_t i=0;
   while (record.good()) {
      getline(record,line);
      istrstream s(line);
      s>>ts[i]>>p1[i]>>p2[i]>>p3[i]>>p4[i]>>p5[i]>>bottom[i]>>xBottom[i]>>xTop[i];
      xTmB[i]=xTop[i]-xBottom[i];
      t[i]=(ts[i]/60-42);
      i++;
   }
   record.close();
   
   gROOT->Macro("style.C");      
   MyStyle->SetTitleOffset(1.3,"y");
   MyStyle->cd();

   TCanvas *c1 = new TCanvas ("c1","",0,0,1000,800);
   TGraph *xb = new TGraph(n,t,p2);
   TGraph *xt = new TGraph(n,t,p3);
   TGraph *xtmb = new TGraph(n,t,p4);

   xb->SetTitle("");
   xb->GetXaxis()->SetTitle("Time [Minute]");
   xb->GetXaxis()->CenterTitle();
//   xb->GetXaxis()->SetRangeUser(-15,100);
   xb->GetYaxis()->SetTitle("Temperature [Celsius]");
   xb->GetYaxis()->CenterTitle();
   xb->GetYaxis()->SetRangeUser(-196,-188);
   xb->SetLineColor(kRed);
   xb->Draw("al");
   xt->SetLineColor(kBlue);
   xt->Draw("l");
   xtmb->Draw("l");

   TLegend* leg = new TLegend(0.6, 0.45, 0.85, 0.7);
   leg->AddEntry(xb,"PT100 2","l");
   leg->AddEntry(xt,"PT100 3","l");
   leg->AddEntry(xtmb,"PT100 4","l");
   leg->SetLineColor(kWhite);
   leg->SetTextFont(132);
   leg->Draw();
}

