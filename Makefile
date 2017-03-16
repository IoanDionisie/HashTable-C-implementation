CFLAGS = /nologo /W3 /Za /D_CRT_SECURE_NO_DEPRECATE

build:tema1.exe

tema1.exe:tema1.obj hash.lib
	cl $(CFLAGS) /Fetema1.exe tema1.obj hash.lib

tema1.obj:Tema1.c
	cl $(CFLAGS) /Fotema1.obj /c Tema1.c

clean:
	del *.obj *.exe