NAME = blur
EXTERNAL = external/external.c
HEADERS = -I. -I external -I

linux: $(NAME).cpp $(EXTERNAL)
	g++ -m32 -O $(HEADERS) -L.-D_LINUX -D_RELEASE -DNDEBUG -DRELEASE -Xlinker -no-undefined -fno-exceptions -Wl,-Bstatic -Wl,-Bdynamic -static -shared $(NAME).cpp $(EXTERNAL) -o ../binaries/$(NAME).so

win32: $(NAME).cpp $(EXTERNAL)
	g++ -m32 $(HEADERS) -D_WIN32 -D_RELEASE -DNDEBUG -DRELEASE -Xlinker -no-undefined -fno-exceptions -Wl,-Bstatic -Wl,-Bdynamic -static -shared $(NAME).cpp $(EXTERNAL) -o ../binaries/$(NAME).dll

