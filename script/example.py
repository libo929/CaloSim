#!/usr/bin/env python

import os
import sys
import SDHCALSim as sim

import math


if __name__ == '__main__' :

        #os.environ["SIMEXE"] = '/path/to/SDHCALSim/bin/SDHCALSim'
	os.environ["SIMEXE"] = '/Users/libo/workplace/ECALSim/bin/SDHCALSim'

	particle = 'gamma'
	energy = 10


	params = sim.Params()
	params.physicsList = 'QGSP_BERT'
	params.nEvent = 2000
	params.seed = 0
	params.outputFileName = 'SDHCALSim_gm_10GeV'
	
	params.oldConfig = False
	params.killNeutrons = False


	charged = sim.Particle()
	charged.particleName = particle
	charged.energy = energy


	charged.momentumOption = 'gaus'
	#charged.sigmaMomentum = 0.15
	charged.sigmaMomentum = 0.

	charged.positionOption = 'gaus'
	charged.positionZ = -10
	#charged.sigmaPos = 33
	charged.sigmaPos = 0


	params.particleList.append(charged)

	sim.launch( params )


