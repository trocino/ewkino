#include <iostream>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"

void trainMvaMethods(const std::string& jetsCat = "", const std::string& mllCat = ""){
    //output file
    TFile* outputFile = TFile::Open((const TString&) "trainingOutput" + jetsCat + mllCat +".root", "RECREATE" );
    TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

    //names of categories for which to do the training
    //const std::string mllNames[nMll] = {"mllInclusive", "onZ", "offZ"};
    //const std::string catNames[nCat] = {"nJetsInclusive", "0bJets_01Jets", "0bJets_2Jets", "1bJet_01jets", "1bJet_23Jets", "1bJet_4Jets", "2bJets"};

    TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");

    dataloader->AddVariable("topMass", 'F');
    if(jetsCat != "1bJet_01jets") dataloader->AddVariable("pTForwardJets", 'F');
    dataloader->AddVariable("etaMostForward", 'F');
    if(jetsCat == "1bJet_4Jets" || jetsCat == "2bJets") dataloader->AddVariable("numberOfJets", 'F');
    if(jetsCat != "1bJet_4Jets" && jetsCat != "2bJets") dataloader->AddVariable("pTLeadingJet", 'F');
    dataloader->AddVariable("pTLeadingLepton", 'F');
    dataloader->AddVariable("mNotSoForwardJets");
    if(jetsCat != "1bJet_01jets" && jetsCat != "0bJets_01Jets" && jetsCat != "1bJet_4Jets" && jetsCat != "2bJets") dataloader->AddVariable("pTLeadingBJet", 'F');
    dataloader->AddVariable("missingET", 'F');
    dataloader->AddVariable("highestDeepCSV", 'F');
    if(jetsCat != "1bJet_01jets" && jetsCat != "0bJets_01Jets") dataloader->AddVariable("maxMjj", 'F');
    if(jetsCat != "0bJets_01Jets" && jetsCat != "0bJets_2Jets") dataloader->AddVariable("minMlb", 'F');
    if(jetsCat != "1bJet_01jets") dataloader->AddVariable("asymmetryWlep", 'F'); //mystery crash for "0bJets_2Jets" category on this variable
    dataloader->AddVariable("etaZ", 'F');
    dataloader->AddVariable("m3l", 'F');
    if(jetsCat != "0bJets_01Jets" && jetsCat != "1bJet_01jets" && jetsCat != "1bJet_4Jets" && jetsCat != "2bJets") dataloader->AddVariable("maxDeltaPhijj", 'F');
    if(jetsCat != "0bJets_01Jets" && jetsCat != "1bJet_01jets") dataloader->AddVariable("maxDeltaRjj", 'F');
    dataloader->AddVariable("maxDeltaPhill", 'F');
    //dataloader->AddVariable("maxM2l", 'F');           //not used now due to correlation with 3l variables
    if(jetsCat != "0bJets_01Jets" && jetsCat != "0bJets_2Jets")  dataloader->AddVariable("pTMaxlb", 'F');
    //dataloader->AddVariable("ptMax2l", 'F');          //not used now due to correlation with 3l variables
    dataloader->AddVariable("pT3l", 'F');
    dataloader->AddVariable("mForwardJetsLeadinBJetW", 'F');
    dataloader->AddVariable("ht", 'F');

    dataloader->SetSignalWeightExpression("eventWeight");
    dataloader->SetBackgroundWeightExpression("eventWeight");

    //get training trees
    TFile* file =  new TFile("../trainingTrees/bdtTrainingTree.root");
    TTree* signalTree = (TTree*) (file->Get((const TString&) "signalTree" + jetsCat + mllCat));
    TTree* backgroundTree = (TTree*) (file->Get((const TString&) "backgroundTree" + jetsCat + mllCat));

    dataloader->AddSignalTree(signalTree, 1.);
    dataloader->AddBackgroundTree(backgroundTree, 1.);


    TCut mycuts = ""; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
    TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";
    dataloader->PrepareTrainingAndTestTree( mycuts, mycutb, "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:NormMode=None:SplitMode=Random:!V" );
    //BDT 
    //factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTG", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2" );
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGAlt", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:nCuts=200:MaxDepth=2:IgnoreNegWeightsInTraining=True");
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGAlt_200trees", "!H:!V:NTrees=200:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:nCuts=200:MaxDepth=2:IgnoreNegWeightsInTraining=True");
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGAlt_shrinkage04", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.4:nCuts=200:MaxDepth=2:IgnoreNegWeightsInTraining=True");
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGAlt_minNode005", "!H:!V:NTrees=1000:MinNodeSize=5%:BoostType=Grad:Shrinkage=0.10:nCuts=200:MaxDepth=2:IgnoreNegWeightsInTraining=True");
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGAlt_negWeights", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:nCuts=200:MaxDepth=2");
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGAlt_MaxDepth4", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:nCuts=200:MaxDepth=4:IgnoreNegWeightsInTraining=True");
    factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGAlt_20Cuts", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:nCuts=20:MaxDepth=4:IgnoreNegWeightsInTraining=True");
    //factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTGOtherAn", "!H:!V:NTrees=200:BoostType=Grad:Shrinkage=0.4:nCuts=200:MaxDepth=2:IgnoreNegWeightsInTraining=True");
    //factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT", "!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" ); //850
    //factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTB", "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20");
    //factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDTD", "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate" );

    //NN
    //factory->BookMethod( dataloader, TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );
    // General layout.
    TString layoutString ("Layout=TANH|128,TANH|128,TANH|128,LINEAR");
     
    // Training strategies.
    TString training0("LearningRate=1e-1,Momentum=0.9,Repetitions=1,"
    "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
    "WeightDecay=1e-4,Regularization=L2,"
    "DropConfig=0.0+0.5+0.5+0.5, Multithreading=True");
    TString training1("LearningRate=1e-2,Momentum=0.9,Repetitions=1,"
    "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
    "WeightDecay=1e-4,Regularization=L2,"
    "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
    TString training2("LearningRate=1e-3,Momentum=0.0,Repetitions=1,"
    "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
    "WeightDecay=1e-4,Regularization=L2,"
    "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
    TString trainingStrategyString ("TrainingStrategy=");
    trainingStrategyString += training0 + "|" + training1 + "|" + training2;
    
    // General Options.
    TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:"
    "WeightInitialization=XAVIERUNIFORM");
    dnnOptions.Append (":"); dnnOptions.Append (layoutString);
    dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);
    
    // Standard implementation, no dependencies.
    TString stdOptions = dnnOptions + ":Architecture=STANDARD";
    //factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN", stdOptions);
    
    // Multi-core CPU implementation.
    //TString cpuOptions = dnnOptions + ":Architecture=CPU";
    //factory->BookMethod(dataloader, TMVA::Types::kDNN, "DNN CPU", cpuOptions);
    //
    //train MVAs using the set of training events
    factory->TrainAllMethods();

    //evaluate all MVAs using the set of test events
    factory->TestAllMethods();

    //evaluate and compare performance of all configured MVAs
    factory->EvaluateAllMethods();

    //save the output
    outputFile->Close();

    delete factory;
    delete dataloader;
}

int main(int argc, char* argv[]){
    if(argc > 2) trainMvaMethods(argv[1], argv[2]);
    else trainMvaMethods("nJetsInclusive", "mllInclusive");
    return 0;
}
