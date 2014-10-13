all: s-build-mkfs s-build-ls s-build-mkdir s-build-cp

s-build-mkfs:
	cd mkfs.phobos && make

s-build-ls:
	cd ls.phobos && make

s-build-mkdir:
	cd mkdir.phobos && make

s-build-cp:
	cd cp.phobos && make	

clean: clean-bin clean-mkfs clean-ls clean-mkdir clean-cp

clean-bin:
	rm -r -f ./bin/*

clean-mkfs:
	cd mkfs.phobos && make clean

clean-ls:
	cd ls.phobos && make clean

clean-mkdir:
	cd mkdir.phobos && make clean

clean-cp:
	cd cp.phobos && make clean
	
	
