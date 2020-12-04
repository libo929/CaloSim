#!/usr/bin/env python

import os
import sys
import math

import SDHCALSim as sim

particle = 'pi0'   # partile type 
energy   = 40      # GeV
#useUI    = False   # use User Interface
useUI    = True # use User Interface


if __name__ == '__main__' :

        exeDir = sys.path[0]

        if useUI:
            exeDir = os.path.join(exeDir, '../bin/SDHCALSimUI')
        else:
            exeDir = os.path.join(exeDir, '../bin/SDHCALSim')
	os.environ["SIMEXE"] = exeDir

	params = sim.Params()
	params.physicsList = 'QGSP_BERT'
	params.nEvent = 1
	params.seed = 1

        if useUI:
            params.outputFileName = 'SDHCALSimUI_' + particle + '_' + str(energy) + 'GeV'
        else:
            params.outputFileName = 'SDHCALSim_' + particle + '_' + str(energy) + 'GeV'

	
	params.oldConfig = False
	params.killNeutrons = False


	part = sim.Particle()
	part.particleName = particle
	part.energy = energy


	part.momentumOption = 'gaus'
	part.sigmaMomentum = 0.

	part.positionOption = 'gaus'
	part.positionZ = -1800           # mm
	part.sigmaPos = 0


	params.particleList.append(part)

	sim.launch( params )
