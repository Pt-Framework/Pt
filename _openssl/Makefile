test1: test1.o SSLContext.o SSLConnector.o SSLMemoryConnector.o SSLMemoryServer.o SSLMemoryClient.o
	@echo Linking $@ ...
	@g++  $^ -lssl -o $@

%.o: *.h %.cpp
	@echo Compiling $@ ...
	@g++ -c $(subst .o,.cpp,$@) -o $@

clean:
	rm -f *.o test1
