# SSAA in C with Pthreads

This code can resize a pnm image to apply Super Sampling Anti Aliasing with any even resize factor or the resize factor of 3.
It also uses the Pthreads API.

```bash
make
./ssaa <input.pnm> <output.pnm> <resizeFactor> <numberOfThreads>
```
