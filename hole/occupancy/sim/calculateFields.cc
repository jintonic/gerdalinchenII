#include <cmath>

#include "waveform/MGSORFieldCalculation.hh"
#include "waveform/MGSORCrystalFields.hh"

int main()
{
  //create some crystalData
  MGCrystalData* fCrystalData = new MGCrystalData(0,
						  MGCrystalData::kNType,
						  MGCrystalData::kCoaxial,
						  5*CLHEP::mm,
						  37.5*CLHEP::mm,
						  7*CLHEP::cm,
						  7*CLHEP::cm,
						  0., 0., 0.,
						  3, 6);
  
  //create the crystalfields
  MGSORCrystalFields* fFields = new MGSORCrystalFields(fCrystalData);

  //create an instance of the calculation class
  MGSORFieldCalculation* fCalc =  new MGSORFieldCalculation(fFields);
  
  //set Impurity Density in 1/cm^3
  fCrystalData->SetImpurityDensities(1e10,1e10);
  //fCrystalData->SetImpurityDensities(0.414e10,0.414e10); //for sII
  //fCrystalData->SetImpurityDensities(0,0);
  //set operation voltage of crystal
  fCrystalData->SetVoltage(2000.); // for sII
  //fCrystalData->SetVoltage(3000.); //for sI and susie

  //set the grid for calculation 100 steps in r 100 in phi and 100 in z
  fFields->SetGrid(100,120,30); // dr = 1.25 mm, dphi = 6 deg, 
  
  //set a filename ending EField.dat or WField_#segmentnumber.dat will be added
  fFields->SetFileName("f1e10");
  //fFields->SetFileName("f0impurity");
  
  //if you set this the Convergence parameter will be written to disk, nice to check if the algorithm did converge
  // fCalc->SaveConvergenceToFile("TESTConvergence");
  
  //when should the algortihm stop, at what level of change of the fields
  fCalc->SetConvergenceLimit(1E-4);
  
  //set the SOR Constant. SOR algorithm only converges if SOR const 1<SorConst < 2, you could calculate exact and best value, or you just try some. 1.4 seemd to be good  
  fCalc->SetSORConst(1.4);
  
  // then you need to initialize everything 
  fCalc->Initialize();
  
  //then do the Calculation!
  fCalc->CalculateFields();
 
  //////////////////////////////////////////////////////////////////////////
  ////////////////All this is needed for Getting EFields
  ////////////////
  /////////////////////////////////////////////////////////////////////////


//   //set a filename ending EField.dat or WField_#segmentnumber.dat will be added
//   fFields->SetFileName("FirstTests");
  
//   //thry to load the Field from File
//   fFields->LoadFields();
  

//   TRandom3* rand = new TRandom3(0);
//   CLHEP::Hep3Vector coordinates;
  
//   CLHEP::Hep3Vector EField;
//   CLHEP::Hep3Vector WField;
//   double WPotential;
  
//   for(int i=0;i<100;i++){
//     coordinates.setRhoPhiZ(rand->Uniform(0,fCrystalData->GetOuterRadius()/CLHEP::m),rand->Uniform(0,2*M_PI),rand->Uniform(0,fCrystalData->GetCrystalHeight()/CLHEP::m));

//     //    coordinates.setRhoPhiZ(0,0,0);
     
//     EField = fFields->GetEField(coordinates);

//     //+1 because we need core as well. core has number 0
//     for(size_t segment=0;segment<fCrystalData->GetNSegments()+1;segment++){
//       WField =fFields->GetWField(coordinates,segment);
//       WPotential = fFields->GetWPotential(coordinates,segment);
//       std::cout<<"WField for segment "<< segment <<" :"<<std::endl;
//       std::cout<<WField.x()<<WField.x()<<WField.x()<<std::endl;
//       std::cout<<"WPotential for segment "<<segment <<" :"<<std::endl;
//       std::cout<<WPotential<<std::endl;
//     }
//     std::cout<<"EField  :"<<std::endl;
//     std::cout<<EField.x()<<EField.x()<<EField.x()<<std::endl;

//   }
  
//   delete rand;

  //////////////////////////////////////////////////////////////////////////
  ////////////////Don't forget to delete!!!
  ////////////////Do it in the right order! First  Calculation, then Fields, then Crystaldata
  /////////////////////////////////////////////////////////////////////////

  delete fCalc;
  delete fFields;
  delete fCrystalData;
}
