CAIRO_INC_FLAGS=`pkg-config --cflags cairo`
CAIRO_LD_FLAGS=`pkg-config --libs cairo`

CairoTest:
	@g++ -Wall -g -O2 -o CairoTest $(CAIRO_INC_FLAGS) main.cpp $(CAIRO_LD_FLAGS)
	@./CairoTest
	@rm -f CairoTest
