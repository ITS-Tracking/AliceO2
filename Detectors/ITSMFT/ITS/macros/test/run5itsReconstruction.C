#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <string>
#include <vector>
#include <TTree.h>
#include <TFile.h>

#include <FairMCEventHeader.h>

#include "ITStracking/IOUtils.h"
#include "DataFormatsITSMFT/ROFRecord.h"
#include "DataFormatsITSMFT/Cluster.h"
#include "SimulationDataFormat/MCTrack.h"
#include "SimulationDataFormat/MCCompLabel.h"

#include "ITSMFTSimulation/Hit.h"
#endif

void run5itsReconstruction(const int inspEvt = -1,
                           const int numEvents = 1,
                           std::string hitFileName = "AliceO2_TGeant3.mc_10_event.root",
                           std::string path = "./")
{
  auto datafile = TFile::Open((path + hitFileName).data());
  auto o2simTree = (TTree*)datafile->Get("o2sim");

  int nLayersDeducted{0};
  std::vector<o2::MCTrackT<float>>* MCtracks = nullptr;
  std::vector<o2::itsmft::Hit>* ITSHits = nullptr;
  FairMCEventHeader* MCHeader = nullptr;

  o2simTree->SetBranchAddress("MCTrack", &MCtracks);
  o2simTree->SetBranchAddress("ITSHit", &ITSHits);
  o2simTree->SetBranchAddress("MCEventHeader.", &MCHeader);

  std::vector<o2::MCCompLabel> labels;
  std::vector<o2::itsmft::Hit> hits;
  std::vector<o2::itsmft::ROFRecord> rofrecords;

  for (auto iEvent{0}; iEvent < o2simTree->GetEntries(); ++iEvent) {
    o2simTree->GetEntry(iEvent);
    o2::itsmft::ROFRecord rofevent;
    int evententries{0};
    int startoffset = hits.size();
    if (ITSHits->size() > 0) {

      rofevent.setFirstEntry(startoffset);
      for (size_t iHit{0}; iHit < ITSHits->size(); ++iHit, ++evententries) {
        if ((*ITSHits)[iHit].GetDetectorID() > nLayersDeducted)
          nLayersDeducted = (*ITSHits)[iHit].GetDetectorID();
        labels.emplace_back((*ITSHits)[iHit].GetTrackID(), iEvent, 0);
        hits.emplace_back((*ITSHits)[iHit]);
      }
    }
    std::cout << "rof " << iEvent << " starts at: " << startoffset << " ends at: " << startoffset + evententries - 1 << " nLayers: " << nLayersDeducted + 1 << std::flush << std::endl;
    rofevent.setNEntries(evententries);
    rofrecords.push_back(rofevent);
  }
  std::cout << "\ntotal flattened entries: " << hits.size() << "; total ROFRecord created: " << rofrecords.size() << std::endl;

  const int stopAt = (inspEvt == -1) ? o2simTree->GetEntries() : inspEvt + numEvents;
  const int startAt = (inspEvt == -1) ? 0 : inspEvt;

  for (size_t iROfCount{static_cast<size_t>(startAt)}; iROfCount < static_cast<size_t>(stopAt); ++iROfCount) {
    std::cout << "Processing evt: " << iROfCount << std::endl;
    auto& rof = rofrecords[iROfCount];
    o2::its::ROframe frame(iROfCount, nLayersDeducted + 1);
    o2::its::ioutils::loadROFrameDataRun5(rof, frame, gsl::span(hits.data(), hits.size()), labels.data(), nLayersDeducted + 1);
  }
}

o2::itsmft::Hit Hit2ClusterSmearer(const o2::itsmft::Hit hit, const int seed = 1234)
{
  o2::itsmft::Hit smearedhit;
  return smearedhit;
}