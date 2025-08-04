1. Move examples to your keystone directory

2. Build keystone examples
```bash
make -j$(nproc)
```
3. run keystone
```bash
make run
```

4. copy run_stress_tests.sh to your keystone directory
```bash
scp -P 9821 -r examples/run_stress_tests.sh root@localhost:/usr/share/keystone/examples
```

5. run stress tests

I use the following commands to run stress tests:
```bash
./run_stress_tests.sh --cpu --mode iozone.ke
./run_stress_tests.sh --hdd --mode iozone.ke
./run_stress_tests.sh --cpu --mode dhrystone.ke
./run_stress_tests.sh --hdd --mode dhrystone.ke
```

In one command it is:
```bash
./run_stress_tests.sh --cpu --hdd --mode iozone.ke --mode dhrystone.ke
```

6. see the results in the results directory
```bash
scp -P 9821 -r root@localhost:/usr/share/keystone/examples/stress_results/ examples/
```

7. plot the results
```bash
cd examples
python3 plot_results.py
```

8. use sysstat to see the burdern of cpu
```bash
cd examples/sysstat

# look README.MD to compile sysstat

# copy sysstat to your keystone directory
scp -P 9821 -r examples/sysstat/exe/bin/* root@localhost:/usr/bin

# run sysstat
#open another terminal
mpstat -P ALL 5 1
```