DEL /F /Q /S TACDev.egg-info > NUL
RMDIR /Q /S TACDev.egg-info

DEL /F /Q /S build > NUL
RMDIR /Q /S build

DEL /F /Q /S dist > NUL
RMDIR /Q /S dist

pip uninstall -y tacdev

DEL /F /Q /S EPMDev.egg-info > NUL
RMDIR /Q /S EPMDev.egg-info

DEL /F /Q /S build > NUL
RMDIR /Q /S build

DEL /F /Q /S dist > NUL
RMDIR /Q /S dist

pip uninstall -y epmdev

DEL /F /Q /S UDASDev.egg-info > NUL
RMDIR /Q /S UDASDev.egg-info

DEL /F /Q /S build > NUL
RMDIR /Q /S build

DEL /F /Q /S dist > NUL
RMDIR /Q /S dist

pip uninstall -y udasdev

python setup.py install
