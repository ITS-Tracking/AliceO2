// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \file PrimaryVertexContext.h
/// \brief
///

#ifndef TRACKINGITSU_INCLUDE_PRIMARYVERTEXCONTEXT_H_
#define TRACKINGITSU_INCLUDE_PRIMARYVERTEXCONTEXT_H_

#include <algorithm>
#include <array>
#include <iosfwd>
#include <vector>

#include "ITStracking/Cell.h"
#include "ITStracking/Configuration.h"
#include "ITStracking/Constants.h"
#include "ITStracking/Definitions.h"
#include "ITStracking/Road.h"
#include "ITStracking/Tracklet.h"

namespace o2
{
namespace its
{

class PrimaryVertexContext
{
 public:
  PrimaryVertexContext() = default;

  virtual ~PrimaryVertexContext() = default;

  PrimaryVertexContext(const PrimaryVertexContext&) = delete;
  PrimaryVertexContext& operator=(const PrimaryVertexContext&) = delete;

  virtual void initialise(const MemoryParameters& memParam, const std::vector<std::vector<Cluster>>& cl,
                          const std::array<float, 3>& pv, const int iteration);

  virtual void initialise(const MemoryParameters& memParam, const std::vector<std::vector<Cluster>>& cl,
                          const std::array<float, 3>& pv, const int iteration, const o2::its::lightGeometry lGeom);

  const float3& getPrimaryVertex() const;
  lightGeometry getLightGeometry() const;
  std::vector<std::vector<Cluster>>& getClusters();
  std::vector<std::vector<Cell>>& getCells();
  std::vector<std::vector<int>>& getCellsLookupTable();
  std::vector<std::vector<std::vector<int>>>& getCellsNeighbours();
  std::vector<Road>& getRoads();

  bool isClusterUsed(int layer, int clusterId) const;
  void markUsedCluster(int layer, int clusterId);

  std::vector<std::array<int, constants::index_table::ZBins * constants::index_table::PhiBins + 1>>& getIndexTables();
  std::vector<std::vector<Tracklet>>& getTracklets();
  std::vector<std::vector<int>>& getTrackletsLookupTable();

  void initialiseRoadLabels();
  void setRoadLabel(int i, const unsigned long long& lab, bool fake);
  const unsigned long long& getRoadLabel(int i) const;
  bool isRoadFake(int i) const;

 protected:
  float3 mPrimaryVertex;
  lightGeometry mLightGeometry;
  std::array<std::vector<Cluster>, constants::its::LayersNumber> mUnsortedClusters;
  std::vector<std::vector<Cluster>> mClusters;
  std::vector<std::vector<bool>> mUsedClusters;
  std::vector<std::vector<Cell>> mCells;
  std::vector<std::vector<int>> mCellsLookupTable;
  std::vector<std::vector<std::vector<int>>> mCellsNeighbours;
  std::vector<Road> mRoads;

  std::vector<std::array<int, constants::index_table::ZBins * constants::index_table::PhiBins + 1>> mIndexTables;
  std::vector<std::vector<Tracklet>> mTracklets;
  std::vector<std::vector<int>> mTrackletsLookupTable;

  std::vector<std::pair<unsigned long long, bool>> mRoadLabels;
};

inline const float3& PrimaryVertexContext::getPrimaryVertex() const { return mPrimaryVertex; }

inline lightGeometry PrimaryVertexContext::getLightGeometry() const { return mLightGeometry; }

inline std::vector<std::vector<Cluster>>& PrimaryVertexContext::getClusters()
{
  return mClusters;
}

inline std::vector<std::vector<Cell>>& PrimaryVertexContext::getCells() { return mCells; }

inline std::vector<std::vector<int>>& PrimaryVertexContext::getCellsLookupTable()
{
  return mCellsLookupTable;
}

inline std::vector<std::vector<std::vector<int>>>&
  PrimaryVertexContext::getCellsNeighbours()
{
  return mCellsNeighbours;
}

inline std::vector<Road>& PrimaryVertexContext::getRoads() { return mRoads; }

inline bool PrimaryVertexContext::isClusterUsed(int layer, int clusterId) const
{
  return mUsedClusters[layer][clusterId];
}

inline void PrimaryVertexContext::markUsedCluster(int layer, int clusterId) { mUsedClusters[layer][clusterId] = true; }

inline std::vector<std::array<int, constants::index_table::ZBins * constants::index_table::PhiBins + 1>>&
  PrimaryVertexContext::getIndexTables()
{
  return mIndexTables;
}

inline std::vector<std::vector<Tracklet>>& PrimaryVertexContext::getTracklets()
{
  return mTracklets;
}

inline std::vector<std::vector<int>>& PrimaryVertexContext::getTrackletsLookupTable()
{
  return mTrackletsLookupTable;
}

inline void PrimaryVertexContext::initialiseRoadLabels()
{
  mRoadLabels.clear();
  mRoadLabels.resize(mRoads.size());
}

inline void PrimaryVertexContext::setRoadLabel(int i, const unsigned long long& lab, bool fake)
{
  mRoadLabels[i].first = lab;
  mRoadLabels[i].second = fake;
}

inline const unsigned long long& PrimaryVertexContext::getRoadLabel(int i) const
{
  return mRoadLabels[i].first;
}

inline bool PrimaryVertexContext::isRoadFake(int i) const
{
  return mRoadLabels[i].second;
}

} // namespace its
} // namespace o2

#endif /* TRACKINGITSU_INCLUDE_PRIMARYVERTEXCONTEXT_H_ */
