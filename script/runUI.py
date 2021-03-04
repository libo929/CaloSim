import RunCaloSim as rcs

particle = 'gamma'
energy = 50

useUI = True
nThreads = 1

if __name__ == '__main__' :
        rcs.runJob(particle, energy, useUI, nThreads)
