echo 'Conectado con turia.unizar.es...'
ssh -t teruel2@turia.unizar.es '
    echo 'Conectado con turia.unizar.es';
    cd ns-3-mobility-consumption-paper;
    echo 'Actualizando versión...';
    git reset --hard HEAD;
    git pull;
    echo 'Iniciando compilación...';
    ./waf build
    bash
'