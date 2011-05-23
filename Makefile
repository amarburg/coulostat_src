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


# Used to include chan-fat here but found it required some work, so simply 
# included in the code...
externs: arm

CODE_SOURCERY_URL = http://static.leaflabs.com/pub/codesourcery/arm-2010q1-188-arm-none-eabi-toolchain-linux32.tar.gz

arm:
	mkdir -p packages
	cd packages && wget -N ${CODE_SOURCERY_URL}
	tar -xzvf packages/${notdir ${CODE_SOURCERY_URL}}


