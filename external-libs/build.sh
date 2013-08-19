# The path where following directories exist.
# archives - contains tarballs or zip files of open source projects on that Nanolat Database depends.
# lib - contains libraries built.
# include - contains header files of libraries.
EXT_LIB_HOME=`pwd`

rm -rf include lib build
mkdir include lib build

cd build

#########################################################
# build gtest
#########################################################
pushd .
unzip ${EXT_LIB_HOME}/archives/gtest-1.6.0.zip
cd gtest-1.6.0
./configure
make clean
make
cp -r include/* ${EXT_LIB_HOME}/include
cp lib/.libs/* ${EXT_LIB_HOME}/lib
popd

#########################################################
# build snappy (required by leveldb)
#########################################################
pushd .
tar xvfz ${EXT_LIB_HOME}/archives/snappy-1.1.0.tar.gz
cd snappy-1.1.0
./configure
make clean
make
make install DESTDIR=`pwd`/dest
cp dest/usr/local/include/* ${EXT_LIB_HOME}/include
cp dest/usr/local/lib/* ${EXT_LIB_HOME}/lib
popd

#########################################################
# build leveldb
#########################################################
pushd .
tar xvfz ${EXT_LIB_HOME}/archives/leveldb-1.10.0.tar.gz
cd leveldb-1.10.0
# no configure file exists for leveldb
make clean
make
cp -r include/* ${EXT_LIB_HOME}/include
cp libleveldb* ${EXT_LIB_HOME}/lib
popd

#########################################################
# build crossroads
#########################################################
pushd .
tar xvfz ${EXT_LIB_HOME}/archives/libxs-1.2.0.tar.gz
cd libxs-1.2.0
./configure
make clean
make
cp -r include/* ${EXT_LIB_HOME}/include
cp src/.libs/libxs.* ${EXT_LIB_HOME}/lib

popd

#########################################################
# build tokyocabinet
#########################################################
pushd .
tar xvfz ${EXT_LIB_HOME}/archives/tokyocabinet-1.4.48.tar.gz
cd tokyocabinet-1.4.48
./configure
make clean
make
cp *.h ${EXT_LIB_HOME}/include
cp libtokyocabinet* ${EXT_LIB_HOME}/lib

popd
