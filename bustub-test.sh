cmake -DCMAKE_BUILD_TYPE=Debug ..

make -j`nproc`

./test/count_min_sketch_test