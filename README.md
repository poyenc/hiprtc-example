### Following step below to build example

```console
$ mkdir -p build && cd build
$ CXX=amdclang++ cmake .. -DHIP_INSTALL_PREFIX:PATH="/opt/rocm/"
$ make && make install
$ ./bin/client 1 # '1' for element-wise copy; '2' for activation
```
