import os

for energy in range(50, 90, 10):
    sourceFiles = '/home/libo/workplace/CaloSim/script/output/gamma_' + str(energy) + 'GeV/CaloSim_gamma_' + str(energy) + 'GeV_*.slcio'
    targetFile  = '/home/libo/workplace/CaloSim/script/output/gamma_' + str(energy) + 'GeV/CaloSim_gamma_' + str(energy) + 'GeV'

    os.system('lcio_merge_files ' + targetFile  + ' ' + sourceFiles)

    #recofile = 'reco_gamma_' + str(energy) + 'GeV.root'
    os.system('./writeCaloHit ' + targetFile + '.slcio > /dev/null')
    os.system('mv -f caloHits.root /home/libo/workplace/CaloSim/script/output/gamma_' + str(energy) + 'GeV')

    print(str(energy) + ' GeV done.')
