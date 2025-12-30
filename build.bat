windres resources.rc -o resources.o
g++ -Iinclude -Iinclude/tray resources.o src/main.cpp -Llib -ltray -o dexcher
