import RunCaloSim as rcs

particle = 'gamma'

useUI = False
nThreads = 10

if __name__ == '__main__' :
    for energy in range(50, 90, 10):
        rcs.runJob(particle, energy, useUI, nThreads)
