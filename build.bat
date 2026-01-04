set libs=-ltray -limgui -lglew32 -lglfw3 -lopengl32

windres resources.rc -o resources.o
g++ -Iinclude -Iinclude/tray resources.o src/main.cpp -mwindows  -pthread -Llib %libs% -o dexcher
dexcher