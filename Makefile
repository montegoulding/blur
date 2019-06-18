NAME = blur
EXTERNAL = $(NAME)/external/external.c
HEADERS = -I $(NAME) -I $(NAME)/external

$(info $(shell mkdir -p build))

external-x86: $(EXTERNAL)
	gcc -c -v -Wall -Wextra -fPIC -m32 -O $(HEADERS) -L. -D_LINUX -D_RELEASE -DNDEBUG -DRELEASE -Wl,-Bstatic -static $(EXTERNAL) -o build/libexternal.a

external-x64: $(EXTERNAL)
	gcc -c -v -Wall -Wextra -fPIC -m64 -O $(HEADERS) -L. -D_LINUX -D_RELEASE -DNDEBUG -DRELEASE -Wl,-Bstatic -static $(EXTERNAL) -o build/libexternal.a

$(NAME)-x86.so: $(NAME)/$(NAME).cpp external-x86
	g++ -v -Wall -Wextra -fPIC -m32 -O $(HEADERS) -L. -Lbuild -lexternal -D_LINUX -D_RELEASE -DNDEBUG -DRELEASE -Xlinker -no-undefined -fno-exceptions -Wl,-Bstatic -Wl,-Bdynamic -static -shared $(NAME)/$(NAME).cpp -o build/$(NAME)-x86.so

$(NAME)-x64.so: $(NAME)/$(NAME).cpp external-x64
	g++ -v -Wall -Wextra -fPIC -m64 -O $(HEADERS) -L. -lexternal -Lbuild -D_LINUX -D_RELEASE -DNDEBUG -DRELEASE -Xlinker -no-undefined -fno-exceptions -Wl,-Bstatic -Wl,-Bdynamic -shared $(NAME)/$(NAME).cpp -o build/$(NAME)-x64.so
