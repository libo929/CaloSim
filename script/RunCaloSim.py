#!/usr/bin/env python

import os
import sys
import math
import getopt

import CaloSim as sim

def runJob(particle, energy, useUI, nThreads):

        exeDir = sys.path[0]

        exeDir = os.path.join(exeDir, '../bin/CaloSim')

        os.environ["SIMEXE"] = exeDir

        params = sim.Params()
        params.physicsList = 'QGSP_BERT'
        params.nEvent = 1000
        params.seed = 1

        params.outputFileName = '' 

        if useUI:
            params.outputFileName = 'CaloSimUI_' 
            nThreads = 1
        else:
            outputDir = 'output/' + particle + '_' + str(energy) + 'GeV'
            os.system('mkdir -p ' + outputDir)
            params.outputFileName = outputDir + '/CaloSim_'

        params.outputFileName = params.outputFileName + particle + '_' + str(energy) + 'GeV'
	
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

        sim.launch(params, nThreads)

