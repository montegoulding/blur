NAME = blur
EXTERNAL = external/external.c
HEADERS = -I. -I external -I

blur-x86.so: $(NAME).cpp $(EXTERNAL)
	g++ -m32 -O $(HEADERS) -L.-D_LINUX -D_RELEASE -DNDEBUG -DRELEASE -Xlinker -no-undefined -fno-exceptions -Wl,-Bstatic -Wl,-Bdynamic -static -shared $(NAME).cpp $(EXTERNAL) -o ../binaries/$(NAME)-x86.so

blur-x64.so: $(NAME).cpp $(EXTERNAL)
	g++ -fPIC -m64 -O $(HEADERS) -L.-D_LINUX -D_RELEASE -DNDEBUG -DRELEASE -Xlinker -no-undefined -fno-exceptions -Wl,-Bstatic -Wl,-Bdynamic -shared $(NAME).cpp $(EXTERNAL) -o ../binaries/$(NAME)-x64.so

blur.dll: $(NAME).cpp $(EXTERNAL)
	g++ -m32 $(HEADERS) -D_WIN32 -D_RELEASE -DNDEBUG -DRELEASE -Xlinker -no-undefined -fno-exceptions -Wl,-Bstatic -Wl,-Bdynamic -static -shared $(NAME).cpp $(EXTERNAL) -o ../binaries/$(NAME).dll