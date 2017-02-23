# TRPI
Desarrollo de mi Trabajo de Final de Grado en la Universidad de La Laguna en un Algoritmo de trackign para Raspberry usando OpenCV
##Descarga e instalación
###Instalar dependencias
* Actualizamos la raspberry:  
`sudo apt-get update && sudo apt-get upgrade && sudo rpi-update`  
`sudo reboot`  
* Primero herramientas para la compilación:
`sudo apt-get install build-essential`  
* Luego las dependencias requeridas:
`sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev`
* *Librerías opcionales:  
`sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev`   
###Descargar y descomprime la última versión de OpenCV  
* Descarga OpenCV desde su repositorio  
`git clone https://github.com/Itseez/opencv.git`  
`cd opencv/`  
* Preinstalación  
`mkdir build`  
`cd build`   
`cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_QT=ON -D WITH_OPENGL=ON -D WITH_CUBLAS=ON -DCUDA_NVCC_FLAGS="-D_FORCE_INLINES" .. `  
`make -j $(($(nproc) + 1))`  
* Instalación  
`sudo make install`  
`sudo /bin/bash -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'`  
`sudo ldconfig`  
`sudo apt-get update` 
`sudo reboot`  

###Descarga TRPI y compílalo
`git clone https://github.com/Carlosmape/TRPI.git`   
`cd TRPI`     
`cmake .`    
`make`    
Si todo ha salido correctamente, podemos ejecutar el código con `./TRPI`    
##Configuración
En main.cpp puedes configurar algunos parámetros como el color del guía
