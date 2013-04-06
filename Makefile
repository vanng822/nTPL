LIB_PREFIX = $(HOME)/.node_libraries
NODE = node

build:
	@echo "Building..."
	@node-gyp clean && node-gyp configure && node-gyp build
	@echo "Copy relase file ntpl.native.node to lib"
	@cp build/Release/ntpl.native.node lib/nTPL/ntpl.native.node

install:
	@echo "Installing..."
	@mkdir -p $(LIB_PREFIX)
	@cp -fr lib/ntpl/* $(LIB_PREFIX)/
	
uninstall:
	@echo "Uninstalling ..."
	@rm -f $(LIB_PREFIX)/ntpl.js
	@rm -f $(LIB_PREFIX)/ntpl.native.node
	@rm -f $(LIB_PREFIX)/ntpl.block.js
	@rm -f $(LIB_PREFIX)/ntpl.filter.js
	
test:
	@echo "Testing..."
	@cd ./tests && $(NODE) run.js && cd ..

clean:
	@echo "Cleaning directory"
	@node-gyp clean
	@echo "Remove ntpl.native.node from lib"
	@rm lib/nTPL/ntpl.native.node
	
all : uninstall clean build install

dev : uninstall clean build install test
	
.PHONY : build install uninstall test