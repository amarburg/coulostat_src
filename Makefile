GIT = git

PHONY: externs submodules pull push

submodules:
	@$(GIT) submodule init
	@$(GIT) submodule update

pull:
	@$(GIT) pull
	@$(GIT) submodule update

push:
	@$(GIT) push
	@cd libmaple && $(GIT) push

path:
	export PATH=$(LIB_MAPLE_HOME)/../arm/bin:$(PATH)


externs: chan-fat arm

CHAN_FAT_URL = http://elm-chan.org/fsw/ff/ff8a.zip

chan-fat:
	mkdir -p packages
	mkdir -p lib/fat/ff8a
	cd packages && wget -N ${CHAN_FAT_URL}
	unzip -od lib/fat/ff8a packages/${notdir ${CHAN_FAT_URL}}

CODE_SOURCERY_URL = http://static.leaflabs.com/pub/codesourcery/arm-2010q1-188-arm-none-eabi-toolchain-linux32.tar.gz

arm:
	mkdir -p packages
	cd packages && wget -N ${CODE_SOURCERY_URL}
	tar -xzvf packages/${notdir ${CODE_SOURCERY_URL}}


