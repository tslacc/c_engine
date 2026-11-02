BUILDDIR = /tmp/build/
SRCDIR = ./src/
c_files = 
objects = $(BUILDDIR)

$(BUILDDIR)main.out: $(BUILDDIR) $(c_files) $(SRCDIR)main.c
	gcc -std=c23 -O3 -Wall -Wextra -o $(BUILDDIR)main.out $(SRCDIR)main.c $(srcfiles) 
	du -b $(BUILDDIR)main.out
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	rm main.out $(objects)
