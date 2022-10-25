### The Problem

I was asked to provide a concrete example to showcase how we want to employ HIP RTC to compile
templated kernels & use them later.

### Build The Example

```console
$ mkdir -p build && cd build
$ CXX=amdclang++ cmake .. -DHIP_INSTALL_PREFIX:PATH="/opt/rocm/"
$ make && make install
$ ./bin/client 1 # '1' for element-wise copy; '2' for activation
```
