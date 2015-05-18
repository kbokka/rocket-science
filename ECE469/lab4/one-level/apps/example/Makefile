default:
	cd makeprocs; make
	cd hello_world; make
	cd spawn_me; make
	cd test1; make
	cd test2; make
	cd test3; make
	cd test5; make
	cd test4; make
	cd test6; make

clean:
	cd makeprocs; make clean
	cd hello_world; make clean
	cd spawn_me; make clean
	cd test1; make clean
	cd test2; make clean
	cd test3; make clean
	cd test4; make clean
	cd test5; make clean
	cd test6; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj; ee469_fixterminal
