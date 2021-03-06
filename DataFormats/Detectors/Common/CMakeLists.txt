# Copyright CERN and copyright holders of ALICE O2. This software is distributed
# under the terms of the GNU General Public License v3 (GPL Version 3), copied
# verbatim in the file "COPYING".
#
# See http://alice-o2.web.cern.ch/license for full licensing information.
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization or
# submit itself to any jurisdiction.

o2_add_library(DetectorsCommonDataFormats
               SOURCES src/DetID.cxx src/AlignParam.cxx src/DetMatrixCache.cxx
               PUBLIC_LINK_LIBRARIES
               ROOT::Core
               ROOT::Geom
               O2::GPUCommon
               O2::MathUtils)

o2_target_root_dictionary(
  DetectorsCommonDataFormats
  HEADERS include/DetectorsCommonDataFormats/DetID.h
          include/DetectorsCommonDataFormats/AlignParam.h
          include/DetectorsCommonDataFormats/DetMatrixCache.h)

o2_add_test(DetID
            SOURCES test/testDetID.cxx
            PUBLIC_LINK_LIBRARIES O2::DetectorsCommonDataFormats
            COMPONENT_NAME DetectorsCommonDataFormats
            LABELS dataformats)
