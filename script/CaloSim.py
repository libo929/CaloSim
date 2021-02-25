#!/usr/bin/env python

import os
import time

from multiprocessing import Process

class Particle :
	def __init__(self) :
		self.particleName = "gamma"

		self.time = 0
		
		self.cosmic = False

		self.positionOption = "fixed"       #fixed uniform gaus
		self.positionX = 0                  #0 is calorimeter center
		self.positionY = 0
		self.positionZ = -20                #0 is calorimeter begin
		self.uniformDeltaPos = 0            #if uniform
		self.sigmaPos = 0                   #if gaus

		self.momentumOption = "fixed"       #fixed gaus
		self.momentumPhi = 0                #
		self.momentumTheta = 0              #
		self.sigmaMomentum = 0              #if gaus

		self.energyDistribution = "fixed"   #fixed gaus uniform forNN
		self.energy = 10                    #if fixed or gaus
		self.sigmaEnergy = 0                #if gaus
		self.minEnergy = 1                  #if uniform or forNN
		self.maxEnergy = 120                #if uniform or forNN


class Params :
	def __init__(self) :
		self.physicsList = "FTFP_BERT"

		self.nEvent = 100
		self.seed = 0
		self.particleList = []
		self.rpcType = "normal"
		self.oldConfig = False
		self.outputFileName = "output"
		self.killNeutrons = False



def particleConfig(particle) :
	cosmicBool = "true"
	if not particle.cosmic :
		cosmicBool = "false"
			
		return '''
			{
				"particleName" : "''' + particle.particleName + '''",
				"cosmic"   : ''' + cosmicBool + ''',		
				"energy"   : { "option" : "''' + particle.energyDistribution+ '''" , "value" : ''' + str(particle.energy) + ''' , "sigma" : ''' + str(particle.sigmaEnergy) + ''' , "min" : ''' + str(particle.minEnergy) + ''' , "max" : ''' + str(particle.maxEnergy) + ''' },	
				"vertex"   : { "time" : ''' + str(particle.time) + ''' , "option" : "''' + particle.positionOption + '''" , "position" : {"x":''' + str(particle.positionX) + ''', "y":''' + str(particle.positionY) + ''', "z":''' + str(particle.positionZ) + '''} , "delta" : ''' + str(particle.uniformDeltaPos) + ''' , "sigma" : ''' + str(particle.sigmaPos) + ''' },
				"momentum" : { "option" : "''' + particle.momentumOption + '''" , "direction" : {"phi": ''' + str(particle.momentumPhi) + ''', "theta" : ''' + str(particle.momentumTheta) + '''} , "sigma" : ''' + str(particle.sigmaMomentum) + ''' }
			}'''

def runSingleJob(cmd):
        #print cmd
	os.system(cmd)

def launch(a, nThread = 1, useDefaultSeed = True) :

        procList = []

        for i in range(0, nThread):
	    jsonFileName = 'tmpRunFile' + str(i) + '.json'
	    
	    oldConfigBool = "false"
	    if a.oldConfig :
	    	oldConfigBool = "true"

            a.seed = i * 10 + 20210224
            outputFileName = a.outputFileName + '_' + str(i)

	    killNeutronsBool = "false"
	    if a.killNeutrons :
	    	killNeutronsBool = "true"	

	    jsonFileContent = '''
	    {
	    	"outputFileName" : "'''+ outputFileName +'''",
	    	"physicsList" : "'''+ a.physicsList +'''",
	    	"nEvents" : '''+ str(a.nEvent) +''',
	    	"seed" : '''+ str(a.seed) +''',
	    	"killNeutrons" : ''' + killNeutronsBool + ''',
	    	
	    	"detectorConfig" :
	    	{
	    		"rpcType" : "'''+ a.rpcType +'''",
	    		"oldConfig" : ''' + oldConfigBool + '''
	    	},

	    	"particuleGuns" :
	    	['''


	    for particle in a.particleList[:-1] :
	    	jsonFileContent = jsonFileContent + particleConfig(particle) + ','

	    jsonFileContent = jsonFileContent + particleConfig(a.particleList[-1])


	    jsonFileContent = jsonFileContent + '''
	    	]
	    }'''


	    jsonFile = open(jsonFileName , 'w')
	    jsonFile.write(jsonFileContent)
	    jsonFile.close()


	    simuExe = os.environ["SIMEXE"]
	    logFileName = outputFileName + '.log'


	    cmd = simuExe + ' ' + jsonFileName + ' > ' + logFileName + ' 2>&1 &'
            proc = Process(target=runSingleJob, args=(cmd,))
            proc.start()
            procList.append(proc)

        while True:
            jobsFinished = True

            for p in procList:
                if p.is_alive():
                    jobsFinished = False
                    break

            time.sleep(3)

            if jobsFinished:
                break

	os.system('rm tmpRunFile*.json')
        print 'Done' 
