GIT = git


submodules:
	@$(GIT) submodule init
	@$(GIT) submodule update

pull:
	@$(GIT) pull
	@$(GIT) submodule update

push:
	@$(GIT) push
	@cd libmaple && $(GIT) push
