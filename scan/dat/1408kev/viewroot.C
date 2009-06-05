void viewroot(Int_t theChannel, Int_t theAngle){
TChain *A0Tree = new TChain("PSTree");
A0Tree->AddFile(Form("/remote/pclg-10/aghaei/scan/dat/1408kev/phi%d.root",theAngle));
A0Tree->Draw(Form("Cha_MCAEnergy[%d]>>hdat0(200,10000.0,26000.0)",theChannel));
}
