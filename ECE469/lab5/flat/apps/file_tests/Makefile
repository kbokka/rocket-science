default:
	cd file_read; make
	cd file_write; make

clean:
	cd file_read; make clean
	cd file_write; make clean

run:
	@echo "====== Going to run file_write, then file_read programs"
	cd ../../os; make; cd ../bin; dlxsim -x os.dlx.obj -a -D F -u file_write.dlx.obj; ee469_fixterminal
	@echo .
	@echo "====== Shutdown simulator to check persistence"
	cd ../../os; make; cd ../bin; dlxsim -x os.dlx.obj -a -D F -u file_read.dlx.obj; ee469_fixterminal
