import RunCaloSim as rcs

particle = 'mu-'
energy = 2

useUI = True
nThreads = 1

if __name__ == '__main__' :
        rcs.runJob(particle, energy, useUI, nThreads)
