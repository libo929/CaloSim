import RunCaloSim as rcs

particle = 'pi+'

useUI = False
nThreads = 10

if __name__ == '__main__' :
    for energy in range(10, 50, 10):
        rcs.runJob(particle, energy, useUI, nThreads)
