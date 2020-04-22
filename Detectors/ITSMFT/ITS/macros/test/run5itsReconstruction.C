#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <string>
#include <vector>
#include <TTree.h>
#include <TFile.h>

#include <FairMCEventHeader.h>

#include "DataFormatsITSMFT/Cluster.h"
#include "SimulationDataFormat/MCTrack.h"
#include "SimulationDataFormat/MCCompLabel.h"

#include "ITSMFTSimulation/Hit.h"
#endif

void run5itsReconstruction(std::string hitFileName = "AliceO2_TGeant3.mc_1_event.root", std::string path = "./")
{
  auto datafile = TFile::Open((path + hitFileName).data());
  auto o2simTree = (TTree*)datafile->Get("o2sim");

  std::vector<o2::MCTrackT<float>>* MCtracks = nullptr;
  std::vector<o2::itsmft::Hit>* ITSHits = nullptr;
  FairMCEventHeader* MCHeader = nullptr;

  o2simTree->SetBranchAddress("MCTrack", &MCtracks);
  o2simTree->SetBranchAddress("ITSHit", &ITSHits);
  o2simTree->SetBranchAddress("MCEventHeader.", &MCHeader);

  std::vector<o2::MCCompLabel> labels;
  for (auto iEvent{0}; iEvent < o2simTree->GetEntries(); ++iEvent) {
    o2simTree->GetEntry(iEvent);

    /// MC truth creation
    labels.clear();
    for (size_t iHit{0}; iHit < ITSHits->size(); ++iHit) {
      labels.emplace_back((*ITSHits)[iHit].GetTrackID(), iEvent, 0);
    }
  }
}

o2::itsmft::Cluster Hit2ClusterSmearer(const o2::itsmft::Hit hit)
{
    
}