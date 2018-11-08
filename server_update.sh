echo 'Conectado con turia.unizar.es...'
ssh -t teruel2@turia.unizar.es '
    echo 'Conectado con turia.unizar.es';
    echo 'Borrando versión antigua...';
    rm -rf ns-3-mobility-consumption-paper;
    echo 'Versión antigua borrada.';
    echo 'Copiando repositorio con la rama PAPER...';
    git clone --single-branch -b paper https://github.com/ssalvatella/ns-3-mobility-consumption.git ns-3-mobility-consumption-paper;
    echo 'Repositorio copiado con éxito.';
    echo ' ';
    cd ns-3-mobility-consumption-paper;
    bash
'