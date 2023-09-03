# CONVINCE

### Introduction
CONVINCE is the artifact of paper "Compositional Verification of Efficient Masking
Countermeasures against Side-Channel Attacks".


### Getting Started Guided

To build CONVINCE, run the following commands.
```
$ cd CONVINCE
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make
```
The executable is located in `build/tools/Main/convince`.

For example, to verify whether B1+Comp.cl is second order NI, we can run the following command.

```
$ ./build/tools/Main/convince ./benchmarks B1+Comp.cl 2 main ni 0
```
In this command, 
- `./build/tools/Main/convince` refers to the executable file of the CONVINCE artifact.
- `./benchmarks` represents the path to the directory containing the benchmarks.
- `B1+Comp.cl` is the name of the masked implementation we want to verify.
- `2` indicates the order (in this case, second-order).
- `main` specifies the function we want to verify.
- The final `0` is used for debugging purposes.
  
### Related Works

We have conducted a series of work on formal verification of masking countermeasures against side-channel attacks.
We provide a list of papers along with their corresponding artifacts
- Zhang, Jun, Pengfei Gao, Fu Song, and Chao Wang. "SCInfer: Refinement-based verification of software countermeasures against side-channel attacks." In International Conference on Computer Aided Verification, pp. 157-177. Cham: Springer International Publishing, 2018
  - [QMSInfer](https://github.com/S3L-official/QMSInfer)
- Gao, Pengfei, Jun Zhang, Fu Song, and Chao Wang. "Verifying and quantifying side-channel resistance of masked software implementations." ACM Transactions on Software Engineering and Methodology (TOSEM) 28, no. 3 (2019): 1-32
  - [QMSInfer](https://github.com/S3L-official/QMSInfer)
- Gao, Pengfei, Hongyi Xie, Jun Zhang, Fu Song, and Taolue Chen. "Quantitative verification of masked arithmetic programs against side-channel attacks." In International Conference on Tools and Algorithms for the Construction and Analysis of Systems, pp. 155-173. Cham: Springer International Publishing, 2019
  - [QMVerif](https://github.com/S3L-official/QMVerif)
- Gao, Pengfei, Hongyi Xie, Pu Sun, Jun Zhang, Fu Song, and Taolue Chen. "Formal Verification of Masking Countermeasures for Arithmetic Programs." IEEE Transactions on Software Engineering 48, no. 3 (2020): 973-1000
  - [QMVerif](https://github.com/S3L-official/QMVerif)







