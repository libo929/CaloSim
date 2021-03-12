import os

particle = 'pi+'

doMerge = True

for energy in range(10, 50, 10):
    sourceFiles = 'output/' + particle + '_' + str(energy) + 'GeV/CaloSim_' + particle + '_' + str(energy) + 'GeV_*.slcio'
    targetFile  = 'output/' + particle + '_' + str(energy) + 'GeV/CaloSim_' + particle + '_' + str(energy) + 'GeV'

    if doMerge:
        os.system('lcio_merge_files ' + targetFile  + ' ' + sourceFiles)

    os.system('./writeCaloHit ' + targetFile + '.slcio > /dev/null')
    os.system('mv -f caloHits.root output/' + particle + '_' + str(energy) + 'GeV')

    print(str(energy) + ' GeV done.')
