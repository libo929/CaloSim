import os

particle='gamma'

for energy in range(10, 90, 10):
    sourceFiles = 'output/' + particle + '_' + str(energy) + 'GeV/CaloSim_' + particle + '_' + str(energy) + 'GeV_*.slcio'
    targetFile  = 'output/' + particle + '_' + str(energy) + 'GeV/CaloSim_' + particle + '_' + str(energy) + 'GeV'

    os.system('lcio_merge_files ' + targetFile  + ' ' + sourceFiles)

    os.system('./writeCaloHit ' + targetFile + '.slcio > /dev/null')
    os.system('mv -f caloHits.root output/' + particle + '_' + str(energy) + 'GeV')

    print(str(energy) + ' GeV done.')
