default:
	cd makeprocs; make
	cd inject_n2; make
	cd inject_h20; make
	cd break_n2; make
	cd break_h2o; make
	cd make_no2; make

clean:
	cd makeprocs; make clean
	cd inject_n2; make clean
	cd inject_h20; make clean
	cd break_n2; make clean
	cd break_h2o; make clean
	cd make_no2; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 4 5; ee469_fixterminal