void test(Int_t theChannel, Int_t theAngle){
TChain *A0Tree = new TChain("PSTree");
A0Tree->AddFile(Form("/.hb/raidg01/Siegfried_II/Run_05/Eu152_Scan_PS/phi/super/Eu_scan_phi%d_z2070001.root",theAngle));
A0Tree->Draw(Form("Cha_MCAEnergy[%d]>>hdat0(400,300.0,27000.0)",theChannel));
}
