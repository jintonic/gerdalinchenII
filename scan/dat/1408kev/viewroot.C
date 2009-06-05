void viewroot(){
TChain *A0Tree = new TChain("PSTree");
A0Tree->AddFile("/remote/pclg-10/aghaei/scan/dat/1408kev/phi5.root");
A0Tree->Draw("Cha_MCAEnergy[13]>>hdat0(100,12000.0,20000.0)");
}
