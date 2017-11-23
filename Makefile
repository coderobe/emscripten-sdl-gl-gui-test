CC := emcc
CXX := em++
JOBS := 4
INCLUDE += -I./glw_imgui/include
FLAGS += -s FULL_ES3=1
FLAGS += -s DEMANGLE_SUPPORT=1
FLAGS += -s USE_SDL=2
FLAGS += -g4
LIBS += glw_imgui/out/libglw_imgui.a

FILES += platform_sdl.cpp
FILES += main.cpp

EMBED += --embed-file assets/DroidSans.ttf@DroidSans.ttf

OUTDIR := out
OUT := main.js

all: $(OUTDIR) $(OUTDIR)/index.html $(OUTDIR)/$(OUT)

glw_imgui:
	git clone https://github.com/yak32/glw_imgui

glw_imgui/out/libglw_imgui.a: glw_imgui
	(cd glw_imgui; mkdir -p out; cd out; cmake -D CMAKE_C_COMPILER=$(CC) -D CMAKE_CXX_COMPILER=$(CXX) .. && make -j$(JOBS))

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/index.html:
	cp index.html $(OUTDIR)

$(OUTDIR)/$(OUT): $(FILES) glw_imgui/out/libglw_imgui.a
	$(CXX) $(INCLUDE) $(FLAGS) $(FILES) $(LIBS) -o $(OUTDIR)/$(OUT) $(EMBED)

clean:
	rm -rf $(OUTDIR)
	$(cd glw_imgui; rm -rf out)

.PHONY: all clean
