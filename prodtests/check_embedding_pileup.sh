#!/bin/bash
set -x

# Script verifying pileup / embedding features of digitizers
# It is a basic and fully automatic status check of what should
# work

# The approach is the following:
# 1) for each detector we simulate event sequences that leave (identical) hits
# 2) we digitize the events with the same collision time assigned to both of them
#    and check whether the output digits have multiple labels --> checks pileup
# 3) we digitize with trivial embedding: background and signal events are the same
#    and check whether the output digits have multiple labels --> checks embedding

dets=(  ITS  TPC  TOF  EMC  HMP  MCH  MID  MFT  FV0  FT0  FDD  TRD  PHS CPV  ZDC )
generators=( boxgen boxgen boxgen boxgen hmpidgun fwpigen fwpigen fwpigen fddgen fddgen fddgen boxgen boxgen boxgen boxgen )

for idx in "${!dets[@]}"; do
  d=${dets[$idx]}
  gen=${generators[$idx]}
  # we put the detector plus pipe and magnet as materials
  o2-sim-serial -m PIPE MAG ${d} -g ${gen} -n 1 --seed 1 -o o2sim${d} > simlog${d} 2>&1

  # we duplicate the events/hits a few times in order to have a sufficient
  # condition for pileup
  f=4
  origin="o2sim${d}.root"
  target="o2sim${d}_${f}.root"
  root -q -b -l ${O2_ROOT}/share/macro/duplicateHits.C\(\"${origin}\",\"${target}\",${f}\)

  unlink o2sim.root
  unlink o2sim_grp.root
  ln -s ${target} o2sim.root
  ln -s o2sim${d}_grp.root o2sim_grp.root

  # verify that we have hits
  NUMHITS=`root -q -b -l ${O2_ROOT}/share/macro/analyzeHits.C | grep "${d}" | awk '//{print $2}'`
  MSG="Found ${NUMHITS} hits for ${d}"
  echo $MSG

  # digitize with extreme bunch crossing as well as with embedding the signal onto itself
  o2-sim-digitizer-workflow --onlyDet ${d} --interactionRate 1e9 -b --tpc-lanes 1 --simFileS o2sim.root > digilog${d} 2>&1

  # find newly created digitfile
  digitfile=`ls -lt *digi*.root | head -n 1 | awk '//{print $9}'`

  root -q -b -l ${O2_ROOT}/share/macro/analyzeDigitLabels.C\(\"${digitfile}\",\"${d}\"\)

done
