all:
	@if !(which cmake > /dev/null 2>&1);then echo "======== Need cmake! ========";exit 1;fi;if ! [ -d build ];then mkdir build;fi;cd build;cmake ../;make;

clean:
	@if [ -d build ];then rm -rf build;fi;
