default:
	cd ostests; make

clean:
	cd ostests; make clean

run:
	@echo "------ Running the OS for once"
	cd ../../bin; dlxsim -x os.dlx.obj -a -D F -u ostests.dlx.obj; ee469_fixterminal
	@echo
	@echo
	@echo "****** Shutting down the OS to check for persistence of filesystem"
	@echo "------ Running the OS again"
	cd ../../bin; dlxsim -x os.dlx.obj -a -D F -u ostests.dlx.obj; ee469_fixterminal
