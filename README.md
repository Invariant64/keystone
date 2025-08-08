1. reorganize examples dir
Resnet50 and other are in ML/

```bash
$ tree -L 1
.
├── attestation
├── CMakeLists.txt
├── coremark
├── hello
├── hello-native
├── io
├── mem
├── ML
├── README.md
├── rv8-bench
├── stress_test
├── sysstat
└── tests

```


2. 

The large files like `examples/ML/resnet50/eapp/resnet50.c` are tracked using Git LFS.
To properly download these files, please run:

```bash
git lfs pull
```
