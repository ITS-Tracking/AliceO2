#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <string>
#include <vector>
#include <TTree.h>
#include <TFile.h>
#include <TNtuple.h>

#include <FairMCEventHeader.h>

#include "ITStracking/IOUtils.h"
#include "DataFormatsITSMFT/ROFRecord.h"
#include "DataFormatsITSMFT/Cluster.h"
#include "SimulationDataFormat/MCTrack.h"
#include "SimulationDataFormat/MCCompLabel.h"

#include "ITStracking/Vertexer.h"
#include "GPUO2Interface.h"
#include "GPUReconstruction.h"
#include "GPUChainITS.h"

#include "ITSMFTSimulation/Hit.h"
#endif

using Vertex = o2::dataformats::Vertex<o2::dataformats::TimeStamp<int>>;
using namespace o2::gpu;

void run5itsReconstruction(const int inspEvt = -1,
                           const int numEvents = 1,
                           std::string hitFileName = "AliceO2_TGeant3.mc_10_event.root",
                           std::string path = "./")
{
  TFile* outputfile = new TFile("ITSRecoRun5.root", "recreate");
  TTree outTree("o2sim", "Vertexer Vertices");
  std::vector<o2::dataformats::Vertex<o2::dataformats::TimeStamp<int>>>* verticesITS =
    new std::vector<o2::dataformats::Vertex<o2::dataformats::TimeStamp<int>>>;
  outTree.Branch("ITSVertices", &verticesITS);
  TNtuple foundVerticesBenchmark("foundVerticesBenchmark", "Found vertices benchmark", "frameId:foundVertices");

  std::cout << "\n================== Simulation Fast-Process Begins =====================\n"
            << std::endl;
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
  std::cout << "\n================== Reconstruction Begins ==============================\n"
            << std::endl;
  std::unique_ptr<GPUReconstruction> rec(GPUReconstruction::CreateInstance(GPUDataTypes::DeviceType::CPU, true));
  auto* chainITS = rec->AddChain<GPUChainITS>();
  rec->Init();
  o2::its::Vertexer vertexer(chainITS->GetITSVertexerTraits());

  const int stopAt = (inspEvt == -1) ? o2simTree->GetEntries() : inspEvt + numEvents;
  const int startAt = (inspEvt == -1) ? 0 : inspEvt;

  for (size_t iROfCount{static_cast<size_t>(startAt)}; iROfCount < static_cast<size_t>(stopAt); ++iROfCount) {
    std::cout << "Processing evt: " << iROfCount << std::endl;
    auto& rof = rofrecords[iROfCount];
    o2::its::ROframe frame(iROfCount, nLayersDeducted + 1);
    int nclUsed = o2::its::ioutils::loadROFrameDataRun5(rof, frame, gsl::span(hits.data(), hits.size()), labels.data(), nLayersDeducted + 1);
    std::array<float, 3> total{0.f, 0.f, 0.f};
    o2::its::ROframe* eventptr = &frame;

    // Run Vertexer
    total[0] = vertexer.evaluateTask(&o2::its::Vertexer::initialiseVertexer, "Vertexer initialisation", std::cout, eventptr);
    total[1] = vertexer.evaluateTask(&o2::its::Vertexer::findTracklets, "Tracklet finding", std::cout);
    total[2] = vertexer.evaluateTask(&o2::its::Vertexer::validateTracklets, "Adjacent tracklets validation", std::cout);
    total[3] = vertexer.evaluateTask(&o2::its::Vertexer::findVertices, "Vertex finding", std::cout);
    std::vector<Vertex> vertITS = vertexer.exportVertices();
    const size_t numVert = vertITS.size();
    foundVerticesBenchmark.Fill(static_cast<float>(iROfCount), static_cast<float>(numVert));
    verticesITS->swap(vertITS);
    outTree.Fill();
  }
  outputfile->cd();
  outTree.Write();
  foundVerticesBenchmark.Write();
  outputfile->Close();
}

o2::itsmft::Hit Hit2ClusterSmearer(const o2::itsmft::Hit hit, const int seed = 1234)
{
  o2::itsmft::Hit smearedhit;
  return smearedhit;
}