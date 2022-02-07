## Performance Profiling with `perf`

### Description 

A  _very_ simple introduction to performance profiling with `perf`. Also, covers the following

   * hardware performance counters and performance events  
   * relative performance and normalized performance   
   * Tools: `hwloc`  
   * Linux utilities: `awk`, `sed`  

### Outline 

  * [Environment Set-up](#env)
  * [Performance Measurement](#perf_measure)
  * [`perf` Basics](#perf)
  * [Normalized Performance](#norm_perf)

### <a name="env"></a>Environment Set-up
   

##### (i) Login to remote linux system


```bash
ssh minksy.cs.txstate.edu 
```

Set up a working directory for the experiments.


```bash
cd
mkdir perf_experiments 
cd perf_experiments 
```

##### (ii) Get familiar with the experimental environment

Get information about the OS and architecture.


```bash
uname -a
```

    Linux minksy.cs.txstate.edu 5.4.0-77-generic #86~18.04.1-Ubuntu SMP Fri Jun 18 01:23:22 UTC 2021 x86_64 x86_64 x86_64 GNU/Linux


See who else is logged on and what they are doing


```bash
w
```

     15:06:40 up 46 days, 23:53,  2 users,  load average: 0.04, 0.03, 0.00
    USER     TTY      FROM             LOGIN@   IDLE   JCPU   PCPU WHAT
    aq10     pts/0    147.26.156.10    14:53   10:29   4.40s  0.02s sshd: aq10 [pri
    aq10     pts/1    147.26.156.10    14:57    1:12   0.06s  0.06s -bash


Get CPU information


```bash
cat /proc/cpuinfo
```

Get memory information

```bash
cat /proc/meminfo
```

 The `hwloc` software package provides command-line tools and a C API to probe the system and get
a more detailed information of compute and memory resources. `hwloc` is usually not pre-installed. It
distributed with BSD licence and can be obtained from the [OpenMPI project
website](https://www.open-mpi.org/projects/hwloc/). `hwloc` has several command-line tools, the most
basic will give a hierarchical map of the compute elements and memory units.


```bash
hwloc-ls
```

    Machine (31GB)
      Package L#0 + L3 L#0 (20MB)
        L2 L#0 (256KB) + L1d L#0 (32KB) + L1i L#0 (32KB) + Core L#0
          PU L#0 (P#0)
          PU L#1 (P#8)
        L2 L#1 (256KB) + L1d L#1 (32KB) + L1i L#1 (32KB) + Core L#1
          PU L#2 (P#1)
          PU L#3 (P#9)
        L2 L#2 (256KB) + L1d L#2 (32KB) + L1i L#2 (32KB) + Core L#2
          PU L#4 (P#2)
          PU L#5 (P#10)
        L2 L#3 (256KB) + L1d L#3 (32KB) + L1i L#3 (32KB) + Core L#3
          PU L#6 (P#3)
          PU L#7 (P#11)
        L2 L#4 (256KB) + L1d L#4 (32KB) + L1i L#4 (32KB) + Core L#4
          PU L#8 (P#4)
          PU L#9 (P#12)
        L2 L#5 (256KB) + L1d L#5 (32KB) + L1i L#5 (32KB) + Core L#5
          PU L#10 (P#5)
          PU L#11 (P#13)
        L2 L#6 (256KB) + L1d L#6 (32KB) + L1i L#6 (32KB) + Core L#6
          PU L#12 (P#6)
          PU L#13 (P#14)
        L2 L#7 (256KB) + L1d L#7 (32KB) + L1i L#7 (32KB) + Core L#7
          PU L#14 (P#7)
          PU L#15 (P#15)
      HostBridge L#0
        PCIBridge
          PCI 1000:0097
            Block(Disk) L#0 "sdb"
            Block(Disk) L#1 "sda"
        PCIBridge
          PCI 10de:13bb
            GPU L#2 "renderD128"
            GPU L#3 "card0"
        PCIBridge
          PCI 10de:1024
            GPU L#4 "card1"
            GPU L#5 "renderD129"
        PCI 8086:153a
          Net L#6 "enp0s25"
        PCIBridge
          PCI 8086:1533
            Net L#7 "enp6s0"
        PCI 8086:8d02
          Block(Removable Media Device) L#8 "sr0"


##### (iii) Obtain code samples

Clone the course git repo on this server.

```bash
git clone https://git.txstate.edu/aq10/CS3339.git ~/CS3339.git
```

Copy the matrix-vector multiplication source file (`matvec.c`) to your working directory

```bash
cp ~/CS3339.git/code_samples/matvec.c .
```

##### (iv) Build and execute

Build the code.

```bash
g++ -o matvec matvec.c
```

Run the executable

```bash
./matvec 2000 200
```

    522738635.7446


### <a name="perf_measure"></a>Performance Measurement

**How do we measure the performance of a program?** 

We can use the `time` command to get a rough measure of the execution time. The terms _execution
time_ and _running time_ are synonymous. Runtime means something different!


```bash
time ./matvec 2000 200
```

    522738635.7446
    
    real	0m4.167s
    user	0m4.155s
    sys	    0m0.012s


The `time` command reports three numbers. `real` time is the time that has elapsed during the
execution of the program. `user` time is the actual time the program is running on the
processor. `sys` is the time when the _system_ is doing some work either on behalf of this program
or some program. Often `real` time is roughly equal to `user` time + `sys` time 

**Are we happy with this performance of matvec?**

#### Relative performance 

Login to another remote Linux system and create a working directory.


```bash
ssh capi.cs.txstate.edu 
mkdir perf_experiments 
cd perf_experiments 
```

Check out the environment.


```bash
uname -a 
cat /proc/cpuinfo 
hwloc-ls 
g++ --version 
```

Clone the course git repo on this server and copy the matrix-vector multiplication code to the working directory.


```bash
git clone https://git.txstate.edu/aq10/CS3339.git ~/CS3339.git
cp ~/CS3339.git/code_samples/matvec.c .
```

Build and run the `matvec` code with the same arguments and record the execution time.


```bash
g++ -o matvec matvec.c
time ./matvec 2000 200
```

**Which system is doing better? Do the results match your expectation?** 

Minimum execution time does not necessarily imply the best performance! There are many factors to
consider. 

### <a name="perf"></a>`perf` Basics

Let's go back to our first machine. 

We can check if `perf` is installed just by typing the `perf` command.


```bash
perf
```

    
     usage: perf [--version] [--help] [OPTIONS] COMMAND [ARGS]
    
     The most commonly used perf commands are:
       annotate        Read perf.data (created by perf record) and display annotated code
       archive         Create archive with object files with build-ids found in perf.data file
       bench           General framework for benchmark suites
       buildid-cache   Manage build-id cache.
       buildid-list    List the buildids in a perf.data file
       c2c             Shared Data C2C/HITM Analyzer.
       config          Get and set variables in a configuration file.
       data            Data file related processing
       diff            Read perf.data files and display the differential profile
       evlist          List the event names in a perf.data file
       ftrace          simple wrapper for kernel's ftrace functionality
       inject          Filter to augment the events stream with additional information
       kallsyms        Searches running kernel for symbols
       kmem            Tool to trace/measure kernel memory properties
       kvm             Tool to trace/measure kvm guest os
       list            List all symbolic event types
       lock            Analyze lock events
       mem             Profile memory accesses
       record          Run a command and record its profile into perf.data
       report          Read perf.data (created by perf record) and display the profile
       sched           Tool to trace/measure scheduler properties (latencies)
       script          Read perf.data (created by perf record) and display trace output
       stat            Run a command and gather performance counter statistics
       test            Runs sanity tests.
       timechart       Tool to visualize total system behavior during a workload
       top             System profiling tool.
       version         display the version of perf binary
       probe           Define new dynamic tracepoints
       trace           strace inspired tool
    
     See 'perf help COMMAND' for more information on a specific command.
    




Recent versions of Ubuntu is likely to have `perf` pre-installed. If `perf` is not installed we can
install it with the following.


```bash
sudo apt-get install linux-tools-common linux-tools-generic linux-tools-`uname -r`
```

Notice the use of back ticks in the above command. You need the `linux-tools-*` package that matches
your kernel. 

Get a basic profile of the `ls` command.


```bash
perf stat ls
```

    matvec	matvec.c
    
     Performance counter stats for 'ls':
    
                  1.39 msec task-clock                #    0.063 CPUs utilized          
                     0      context-switches          #    0.000 K/sec                  
                     0      cpu-migrations            #    0.000 K/sec                  
                    92      page-faults               #    0.066 M/sec                  
             1,913,340      cycles                    #    1.380 GHz                    
             1,758,159      instructions              #    0.92  insn per cycle         
               342,822      branches                  #  247.300 M/sec                  
                12,827      branch-misses             #    3.74% of all branches        
    
           0.022107278 seconds time elapsed
    
           0.001874000 seconds user
           0.000000000 seconds sys
    
    


Get a basic profile of the matrix-vector multiplication code


```bash
perf stat ./matvec 2000 200
```

    522738635.7446
    
     Performance counter stats for './matvec 2000 200':
    
              4,146.53 msec task-clock                #    1.000 CPUs utilized          
                     4      context-switches          #    0.001 K/sec                  
                     0      cpu-migrations            #    0.000 K/sec                  
                 7,882      page-faults               #    0.002 M/sec                  
         7,438,536,419      cycles                    #    1.794 GHz                    
        17,148,686,606      instructions              #    2.31  insn per cycle         
         1,683,384,671      branches                  #  405.974 M/sec                  
               590,791      branch-misses             #    0.04% of all branches        
    
           4.146975842 seconds time elapsed
    
           4.130880000 seconds user
           0.016011000 seconds sys
    
    


**Do we have any new insight about the performance of matvec?**

The set of performance metrics reported with `perf stat` are not the only ones we can get from
`perf`. The `perf` tool allows us to measure a measure a large number of program _events_. To find
the list of available performance events we can use the `perf list` command.


```bash
perf list
```

      branch-instructions OR branches                    [Hardware event]
      branch-misses                                      [Hardware event]
      bus-cycles                                         [Hardware event]
      cache-misses                                       [Hardware event]
      cache-references                                   [Hardware event]
      cpu-cycles OR cycles                               [Hardware event]
      instructions                                       [Hardware event]
      ref-cycles                                         [Hardware event]
      alignment-faults                                   [Software event]
      bpf-output                                         [Software event]
      context-switches OR cs                             [Software event]
      cpu-clock                                          [Software event]
      cpu-migrations OR migrations                       [Software event]
      dummy                                              [Software event]
      emulation-faults                                   [Software event]
      major-faults                                       [Software event]
      minor-faults                                       [Software event]
      page-faults OR faults                              [Software event]
      task-clock                                         [Software event]
      duration_time                                      [Tool event]
      L1-dcache-load-misses                              [Hardware cache event]
      L1-dcache-loads                                    [Hardware cache event]
      L1-dcache-stores                                   [Hardware cache event]
      L1-icache-load-misses                              [Hardware cache event]
      LLC-load-misses                                    [Hardware cache event]
      LLC-loads                                          [Hardware cache event]
      LLC-store-misses                                   [Hardware cache event]
      LLC-stores                                         [Hardware cache event]
      branch-load-misses                                 [Hardware cache event]
      branch-loads                                       [Hardware cache event]
      dTLB-load-misses                                   [Hardware cache event]
      dTLB-loads                                         [Hardware cache event]
      dTLB-store-misses                                  [Hardware cache event]
      dTLB-stores                                        [Hardware cache event]
      iTLB-load-misses                                   [Hardware cache event]
      iTLB-loads                                         [Hardware cache event]
      node-load-misses                                   [Hardware cache event]
      node-loads                                         [Hardware cache event]
      node-store-misses                                  [Hardware cache event]
      node-stores                                        [Hardware cache event]
      branch-instructions OR cpu/branch-instructions/    [Kernel PMU event]
      branch-misses OR cpu/branch-misses/                [Kernel PMU event]
      bus-cycles OR cpu/bus-cycles/                      [Kernel PMU event]
      cache-misses OR cpu/cache-misses/                  [Kernel PMU event]
      cache-references OR cpu/cache-references/          [Kernel PMU event]
      cpu-cycles OR cpu/cpu-cycles/                      [Kernel PMU event]
      cstate_core/c3-residency/                          [Kernel PMU event]
      cstate_core/c6-residency/                          [Kernel PMU event]
      cstate_core/c7-residency/                          [Kernel PMU event]
      cstate_pkg/c2-residency/                           [Kernel PMU event]
      cstate_pkg/c3-residency/                           [Kernel PMU event]
      cstate_pkg/c6-residency/                           [Kernel PMU event]
      cstate_pkg/c7-residency/                           [Kernel PMU event]
      instructions OR cpu/instructions/                  [Kernel PMU event]
      mem-loads OR cpu/mem-loads/                        [Kernel PMU event]
      mem-stores OR cpu/mem-stores/                      [Kernel PMU event]
      msr/aperf/                                         [Kernel PMU event]
      msr/cpu_thermal_margin/                            [Kernel PMU event]
      msr/mperf/                                         [Kernel PMU event]
      msr/smi/                                           [Kernel PMU event]
      msr/tsc/                                           [Kernel PMU event]
      power/energy-pkg/                                  [Kernel PMU event]
      power/energy-ram/                                  [Kernel PMU event]
      ref-cycles OR cpu/ref-cycles/                      [Kernel PMU event]
      topdown-fetch-bubbles OR cpu/topdown-fetch-bubbles/ [Kernel PMU event]
      topdown-recovery-bubbles OR cpu/topdown-recovery-bubbles/ [Kernel PMU event]
      topdown-slots-issued OR cpu/topdown-slots-issued/  [Kernel PMU event]
      topdown-slots-retired OR cpu/topdown-slots-retired/ [Kernel PMU event]
      topdown-total-slots OR cpu/topdown-total-slots/    [Kernel PMU event]
      uncore_imc_0/cas_count_read/                       [Kernel PMU event]
      uncore_imc_0/cas_count_write/                      [Kernel PMU event]
      uncore_imc_0/clockticks/                           [Kernel PMU event]
      uncore_imc_1/cas_count_read/                       [Kernel PMU event]
      uncore_imc_1/cas_count_write/                      [Kernel PMU event]
      uncore_imc_1/clockticks/                           [Kernel PMU event]
      uncore_imc_2/cas_count_read/                       [Kernel PMU event]
      uncore_imc_2/cas_count_write/                      [Kernel PMU event]
      uncore_imc_2/clockticks/                           [Kernel PMU event]
      uncore_imc_3/cas_count_read/                       [Kernel PMU event]
      uncore_imc_3/cas_count_write/                      [Kernel PMU event]
      uncore_imc_3/clockticks/                           [Kernel PMU event]
      uncore_imc_4/cas_count_read/                       [Kernel PMU event]
      uncore_imc_4/cas_count_write/                      [Kernel PMU event]
      uncore_imc_4/clockticks/                           [Kernel PMU event]
    
    cache:
      l1d.replacement                                   
           [L1D data line replacements]
      l1d_pend_miss.fb_full                             
           [Cycles a demand request was blocked due to Fill Buffers inavailability]
      l1d_pend_miss.pending                             
           [L1D miss oustandings duration in cycles]
      l1d_pend_miss.pending_cycles                      
           [Cycles with L1D load Misses outstanding]
      l1d_pend_miss.pending_cycles_any                  
           [Cycles with L1D load Misses outstanding from any thread on physical
            core]
      l1d_pend_miss.request_fb_full                     
           [Number of times a request needed a FB entry but there was no entry
            available for it. That is the FB unavailability was dominant reason
            for blocking the request. A request includes cacheable/uncacheable
            demands that is load, store or SW prefetch. HWP are e]
      l2_demand_rqsts.wb_hit                            
           [Not rejected writebacks that hit L2 cache]
      l2_lines_in.all                                   
           [L2 cache lines filling L2]
      l2_lines_in.e                                     
           [L2 cache lines in E state filling L2]
      l2_lines_in.i                                     
           [L2 cache lines in I state filling L2]
      l2_lines_in.s                                     
           [L2 cache lines in S state filling L2]
      l2_lines_out.demand_clean                         
           [Clean L2 cache lines evicted by demand]
      l2_lines_out.demand_dirty                         
           [Dirty L2 cache lines evicted by demand]
      l2_rqsts.all_code_rd                              
           [L2 code requests]
      l2_rqsts.all_demand_data_rd                       
           [Demand Data Read requests Spec update: HSD78]
      l2_rqsts.all_demand_miss                          
           [Demand requests that miss L2 cache Spec update: HSD78]
      l2_rqsts.all_demand_references                    
           [Demand requests to L2 cache Spec update: HSD78]
      l2_rqsts.all_pf                                   
           [Requests from L2 hardware prefetchers]
      l2_rqsts.all_rfo                                  
           [RFO requests to L2 cache]
      l2_rqsts.code_rd_hit                              
           [L2 cache hits when fetching instructions, code reads]
      l2_rqsts.code_rd_miss                             
           [L2 cache misses when fetching instructions]
      l2_rqsts.demand_data_rd_hit                       
           [Demand Data Read requests that hit L2 cache Spec update: HSD78]
      l2_rqsts.demand_data_rd_miss                      
           [Demand Data Read miss L2, no rejects Spec update: HSD78]
      l2_rqsts.l2_pf_hit                                
           [L2 prefetch requests that hit L2 cache]
      l2_rqsts.l2_pf_miss                               
           [L2 prefetch requests that miss L2 cache]
      l2_rqsts.miss                                     
           [All requests that miss L2 cache Spec update: HSD78]
      l2_rqsts.references                               
           [All L2 requests Spec update: HSD78]
      l2_rqsts.rfo_hit                                  
           [RFO requests that hit L2 cache]
      l2_rqsts.rfo_miss                                 
           [RFO requests that miss L2 cache]
      l2_trans.all_pf                                   
           [L2 or L3 HW prefetches that access L2 cache]
      l2_trans.all_requests                             
           [Transactions accessing L2 pipe]
      l2_trans.code_rd                                  
           [L2 cache accesses when fetching instructions]
      l2_trans.demand_data_rd                           
           [Demand Data Read requests that access L2 cache]
      l2_trans.l1d_wb                                   
           [L1D writebacks that access L2 cache]
      l2_trans.l2_fill                                  
           [L2 fill requests that access L2 cache]
      l2_trans.l2_wb                                    
           [L2 writebacks that access L2 cache]
      l2_trans.rfo                                      
           [RFO requests that access L2 cache]
      lock_cycles.cache_lock_duration                   
           [Cycles when L1D is locked]
      longest_lat_cache.miss                            
           [Core-originated cacheable demand requests missed L3]
      longest_lat_cache.reference                       
           [Core-originated cacheable demand requests that refer to L3]
      mem_load_uops_l3_hit_retired.xsnp_hit             
           [Retired load uops which data sources were L3 and cross-core snoop hits
            in on-pkg core cache Supports address when precise. Spec update:
            HSD29, HSD25, HSM26, HSM30 (Precise event)]
      mem_load_uops_l3_hit_retired.xsnp_hitm            
           [Retired load uops which data sources were HitM responses from shared
            L3 Supports address when precise. Spec update: HSD29, HSD25, HSM26,
            HSM30 (Precise event)]
      mem_load_uops_l3_hit_retired.xsnp_miss            
           [Retired load uops which data sources were L3 hit and cross-core snoop
            missed in on-pkg core cache Supports address when precise. Spec
            update: HSD29, HSD25, HSM26, HSM30 (Precise event)]
      mem_load_uops_l3_hit_retired.xsnp_none            
           [Retired load uops which data sources were hits in L3 without snoops
            required Supports address when precise. Spec update: HSD74, HSD29,
            HSD25, HSM26, HSM30 (Precise event)]
      mem_load_uops_l3_miss_retired.local_dram          
           [Data from local DRAM either Snoop not needed or Snoop Miss (RspI)
            Supports address when precise. Spec update: HSD74, HSD29, HSD25, HSM30
            (Precise event)]
      mem_load_uops_l3_miss_retired.remote_dram         
           [Retired load uop whose Data Source was: remote DRAM either Snoop not
            needed or Snoop Miss (RspI) Supports address when precise. Spec
            update: HSD29, HSM30 (Precise event)]
      mem_load_uops_l3_miss_retired.remote_fwd          
           [Retired load uop whose Data Source was: forwarded from remote cache
            Supports address when precise. Spec update: HSM30 (Precise event)]
      mem_load_uops_l3_miss_retired.remote_hitm         
           [Retired load uop whose Data Source was: Remote cache HITM Supports
            address when precise. Spec update: HSM30 (Precise event)]
      mem_load_uops_retired.hit_lfb                     
           [Retired load uops which data sources were load uops missed L1 but hit
            FB due to preceding miss to the same cache line with data not ready
            Supports address when precise. Spec update: HSM30 (Precise event)]
      mem_load_uops_retired.l1_hit                      
           [Retired load uops with L1 cache hits as data sources Supports address
            when precise. Spec update: HSD29, HSM30 (Precise event)]
      mem_load_uops_retired.l1_miss                     
           [Retired load uops misses in L1 cache as data sources Supports address
            when precise. Spec update: HSM30 (Precise event)]
      mem_load_uops_retired.l2_hit                      
           [Retired load uops with L2 cache hits as data sources Supports address
            when precise. Spec update: HSD76, HSD29, HSM30 (Precise event)]
      mem_load_uops_retired.l2_miss                     
           [Miss in mid-level (L2) cache. Excludes Unknown data-source Supports
            address when precise. Spec update: HSD29, HSM30 (Precise event)]
      mem_load_uops_retired.l3_hit                      
           [Retired load uops which data sources were data hits in L3 without
            snoops required Supports address when precise. Spec update: HSD74,
            HSD29, HSD25, HSM26, HSM30 (Precise event)]
      mem_load_uops_retired.l3_miss                     
           [Miss in last-level (L3) cache. Excludes Unknown data-source Supports
            address when precise. Spec update: HSD74, HSD29, HSD25, HSM26, HSM30
            (Precise event)]
      mem_uops_retired.all_loads                        
           [All retired load uops Supports address when precise. Spec update:
            HSD29, HSM30 (Precise event)]
      mem_uops_retired.all_stores                       
           [All retired store uops Supports address when precise. Spec update:
            HSD29, HSM30 (Precise event)]
      mem_uops_retired.lock_loads                       
           [Retired load uops with locked access Supports address when precise.
            Spec update: HSD76, HSD29, HSM30 (Precise event)]
      mem_uops_retired.split_loads                      
           [Retired load uops that split across a cacheline boundary Supports
            address when precise. Spec update: HSD29, HSM30 (Precise event)]
      mem_uops_retired.split_stores                     
           [Retired store uops that split across a cacheline boundary Supports
            address when precise. Spec update: HSD29, HSM30 (Precise event)]
      mem_uops_retired.stlb_miss_loads                  
           [Retired load uops that miss the STLB Supports address when precise.
            Spec update: HSD29, HSM30 (Precise event)]
      mem_uops_retired.stlb_miss_stores                 
           [Retired store uops that miss the STLB Supports address when precise.
            Spec update: HSD29, HSM30 (Precise event)]
      offcore_requests.all_data_rd                      
           [Demand and prefetch data reads]
      offcore_requests.demand_code_rd                   
           [Cacheable and noncachaeble code read requests]
      offcore_requests.demand_data_rd                   
           [Demand Data Read requests sent to uncore Spec update: HSD78]
      offcore_requests.demand_rfo                       
           [Demand RFO requests including regular RFOs, locks, ItoM]
      offcore_requests_buffer.sq_full                   
           [Offcore requests buffer cannot take more entries for this thread core]
      offcore_requests_outstanding.all_data_rd          
           [Offcore outstanding cacheable Core Data Read transactions in
            SuperQueue (SQ), queue to uncore Spec update: HSD62, HSD61]
      offcore_requests_outstanding.cycles_with_data_rd  
           [Cycles when offcore outstanding cacheable Core Data Read transactions
            are present in SuperQueue (SQ), queue to uncore Spec update: HSD62,
            HSD61]
      offcore_requests_outstanding.cycles_with_demand_data_rd
           [Cycles when offcore outstanding Demand Data Read transactions are
            present in SuperQueue (SQ), queue to uncore Spec update: HSD78, HSD62,
            HSD61]
      offcore_requests_outstanding.cycles_with_demand_rfo
           [Offcore outstanding demand rfo reads transactions in SuperQueue (SQ),
            queue to uncore, every cycle Spec update: HSD62, HSD61]
      offcore_requests_outstanding.demand_code_rd       
           [Offcore outstanding code reads transactions in SuperQueue (SQ), queue
            to uncore, every cycle Spec update: HSD62, HSD61]
      offcore_requests_outstanding.demand_data_rd       
           [Offcore outstanding Demand Data Read transactions in uncore queue Spec
            update: HSD78, HSD62, HSD61]
      offcore_requests_outstanding.demand_data_rd_ge_6  
           [Cycles with at least 6 offcore outstanding Demand Data Read
            transactions in uncore queue Spec update: HSD78, HSD62, HSD61]
      offcore_requests_outstanding.demand_rfo           
           [Offcore outstanding RFO store transactions in SuperQueue (SQ), queue
            to uncore Spec update: HSD62, HSD61]
      offcore_response                                  
           [Offcore response can be programmed only with a specific pair of event
            select and counter MSR, and with specific event codes and predefine
            mask bit value in a dedicated MSR to specify attributes of the offcore
            transaction]
      offcore_response.all_code_rd.llc_hit.hit_other_core_no_fwd
           [Counts all demand & prefetch code reads hit in the L3 and the snoops
            to sibling cores hit in either E/S state and the line is not forwarded]
      offcore_response.all_data_rd.llc_hit.hit_other_core_no_fwd
           [Counts all demand & prefetch data reads hit in the L3 and the snoops
            to sibling cores hit in either E/S state and the line is not forwarded]
      offcore_response.all_data_rd.llc_hit.hitm_other_core
           [Counts all demand & prefetch data reads hit in the L3 and the snoop to
            one of the sibling cores hits the line in M state and the line is
            forwarded]
      offcore_response.all_reads.llc_hit.hit_other_core_no_fwd
           [Counts all data/code/rfo reads (demand & prefetch) hit in the L3 and
            the snoops to sibling cores hit in either E/S state and the line is
            not forwarded]
      offcore_response.all_reads.llc_hit.hitm_other_core
           [Counts all data/code/rfo reads (demand & prefetch) hit in the L3 and
            the snoop to one of the sibling cores hits the line in M state and the
            line is forwarded]
      offcore_response.all_requests.llc_hit.any_response
           [Counts all requests hit in the L3]
      offcore_response.all_rfo.llc_hit.hit_other_core_no_fwd
           [Counts all demand & prefetch RFOs hit in the L3 and the snoops to
            sibling cores hit in either E/S state and the line is not forwarded]
      offcore_response.all_rfo.llc_hit.hitm_other_core  
           [Counts all demand & prefetch RFOs hit in the L3 and the snoop to one
            of the sibling cores hits the line in M state and the line is
            forwarded]
      offcore_response.demand_code_rd.llc_hit.hit_other_core_no_fwd
           [Counts all demand code reads hit in the L3 and the snoops to sibling
            cores hit in either E/S state and the line is not forwarded]
      offcore_response.demand_code_rd.llc_hit.hitm_other_core
           [Counts all demand code reads hit in the L3 and the snoop to one of the
            sibling cores hits the line in M state and the line is forwarded]
      offcore_response.demand_data_rd.llc_hit.hit_other_core_no_fwd
           [Counts demand data reads hit in the L3 and the snoops to sibling cores
            hit in either E/S state and the line is not forwarded]
      offcore_response.demand_data_rd.llc_hit.hitm_other_core
           [Counts demand data reads hit in the L3 and the snoop to one of the
            sibling cores hits the line in M state and the line is forwarded]
      offcore_response.demand_rfo.llc_hit.hit_other_core_no_fwd
           [Counts all demand data writes (RFOs) hit in the L3 and the snoops to
            sibling cores hit in either E/S state and the line is not forwarded]
      offcore_response.demand_rfo.llc_hit.hitm_other_core
           [Counts all demand data writes (RFOs) hit in the L3 and the snoop to
            one of the sibling cores hits the line in M state and the line is
            forwarded]
      offcore_response.pf_l2_code_rd.llc_hit.any_response
           [Counts all prefetch (that bring data to LLC only) code reads hit in
            the L3]
      offcore_response.pf_l2_data_rd.llc_hit.any_response
           [Counts prefetch (that bring data to L2) data reads hit in the L3]
      offcore_response.pf_l2_rfo.llc_hit.any_response   
           [Counts all prefetch (that bring data to L2) RFOs hit in the L3]
      offcore_response.pf_llc_code_rd.llc_hit.any_response
           [Counts prefetch (that bring data to LLC only) code reads hit in the L3]
      offcore_response.pf_llc_data_rd.llc_hit.any_response
           [Counts all prefetch (that bring data to LLC only) data reads hit in
            the L3]
      offcore_response.pf_llc_rfo.llc_hit.any_response  
           [Counts all prefetch (that bring data to LLC only) RFOs hit in the L3]
      sq_misc.split_lock                                
           [Split locks in SQ]
    
    floating point:
      avx_insts.all                                     
           [Approximate counts of AVX & AVX2 256-bit instructions, including
            non-arithmetic instructions, loads, and stores. May count non-AVX
            instructions that employ 256-bit operations, including (but not
            necessarily limited to) rep string instructions that use 256-bit loads
            and stores for optimized performance, XSAVE* and XRSTOR*, and
            operations that transition the x87 FPU data registers between x87 and
            MMX]
      fp_assist.any                                     
           [Cycles with any input/output SSE or FP assist]
      fp_assist.simd_input                              
           [Number of SIMD FP assists due to input values]
      fp_assist.simd_output                             
           [Number of SIMD FP assists due to Output values]
      fp_assist.x87_input                               
           [Number of X87 assists due to input value]
      fp_assist.x87_output                              
           [Number of X87 assists due to output value]
      other_assists.avx_to_sse                          
           [Number of transitions from AVX-256 to legacy SSE when penalty
            applicable Spec update: HSD56, HSM57]
      other_assists.sse_to_avx                          
           [Number of transitions from SSE to AVX-256 when penalty applicable Spec
            update: HSD56, HSM57]
    
    frontend:
      dsb2mite_switches.penalty_cycles                  
           [Decode Stream Buffer (DSB)-to-MITE switch true penalty cycles]
      icache.hit                                        
           [Number of Instruction Cache, Streaming Buffer and Victim Cache Reads.
            both cacheable and noncacheable, including UC fetches]
      icache.ifdata_stall                               
           [Cycles where a code fetch is stalled due to L1 instruction-cache miss]
      icache.ifetch_stall                               
           [Cycles where a code fetch is stalled due to L1 instruction-cache miss]
      icache.misses                                     
           [Number of Instruction Cache, Streaming Buffer and Victim Cache Misses.
            Includes Uncacheable accesses]
      idq.all_dsb_cycles_4_uops                         
           [Cycles Decode Stream Buffer (DSB) is delivering 4 Uops]
      idq.all_dsb_cycles_any_uops                       
           [Cycles Decode Stream Buffer (DSB) is delivering any Uop]
      idq.all_mite_cycles_4_uops                        
           [Cycles MITE is delivering 4 Uops]
      idq.all_mite_cycles_any_uops                      
           [Cycles MITE is delivering any Uop]
      idq.dsb_cycles                                    
           [Cycles when uops are being delivered to Instruction Decode Queue (IDQ)
            from Decode Stream Buffer (DSB) path]
      idq.dsb_uops                                      
           [Uops delivered to Instruction Decode Queue (IDQ) from the Decode
            Stream Buffer (DSB) path]
      idq.empty                                         
           [Instruction Decode Queue (IDQ) empty cycles Spec update: HSD135]
      idq.mite_all_uops                                 
           [Uops delivered to Instruction Decode Queue (IDQ) from MITE path]
      idq.mite_cycles                                   
           [Cycles when uops are being delivered to Instruction Decode Queue (IDQ)
            from MITE path]
      idq.mite_uops                                     
           [Uops delivered to Instruction Decode Queue (IDQ) from MITE path]
      idq.ms_cycles                                     
           [Cycles when uops are being delivered to Instruction Decode Queue (IDQ)
            while Microcode Sequenser (MS) is busy]
      idq.ms_dsb_cycles                                 
           [Cycles when uops initiated by Decode Stream Buffer (DSB) are being
            delivered to Instruction Decode Queue (IDQ) while Microcode Sequenser
            (MS) is busy]
      idq.ms_dsb_occur                                  
           [Deliveries to Instruction Decode Queue (IDQ) initiated by Decode
            Stream Buffer (DSB) while Microcode Sequenser (MS) is busy]
      idq.ms_dsb_uops                                   
           [Uops initiated by Decode Stream Buffer (DSB) that are being delivered
            to Instruction Decode Queue (IDQ) while Microcode Sequenser (MS) is
            busy]
      idq.ms_mite_uops                                  
           [Uops initiated by MITE and delivered to Instruction Decode Queue (IDQ)
            while Microcode Sequenser (MS) is busy]
      idq.ms_switches                                   
           [Number of switches from DSB (Decode Stream Buffer) or MITE (legacy
            decode pipeline) to the Microcode Sequencer]
      idq.ms_uops                                       
           [Uops delivered to Instruction Decode Queue (IDQ) while Microcode
            Sequenser (MS) is busy]
      idq_uops_not_delivered.core                       
           [Uops not delivered to Resource Allocation Table (RAT) per thread when
            backend of the machine is not stalled Spec update: HSD135]
      idq_uops_not_delivered.cycles_0_uops_deliv.core   
           [Cycles per thread when 4 or more uops are not delivered to Resource
            Allocation Table (RAT) when backend of the machine is not stalled Spec
            update: HSD135]
      idq_uops_not_delivered.cycles_fe_was_ok           
           [Counts cycles FE delivered 4 uops or Resource Allocation Table (RAT)
            was stalling FE Spec update: HSD135]
      idq_uops_not_delivered.cycles_le_1_uop_deliv.core 
           [Cycles per thread when 3 or more uops are not delivered to Resource
            Allocation Table (RAT) when backend of the machine is not stalled Spec
            update: HSD135]
      idq_uops_not_delivered.cycles_le_2_uop_deliv.core 
           [Cycles with less than 2 uops delivered by the front end Spec update:
            HSD135]
      idq_uops_not_delivered.cycles_le_3_uop_deliv.core 
           [Cycles with less than 3 uops delivered by the front end Spec update:
            HSD135]
    
    memory:
      hle_retired.aborted                               
           [Number of times an HLE execution aborted due to any reasons (multiple
            categories may count as one) (Precise event)]
      hle_retired.aborted_misc1                         
           [Number of times an HLE execution aborted due to various memory events
            (e.g., read/write capacity and conflicts)]
      hle_retired.aborted_misc2                         
           [Number of times an HLE execution aborted due to uncommon conditions]
      hle_retired.aborted_misc3                         
           [Number of times an HLE execution aborted due to HLE-unfriendly
            instructions]
      hle_retired.aborted_misc4                         
           [Number of times an HLE execution aborted due to incompatible memory
            type Spec update: HSD65]
      hle_retired.aborted_misc5                         
           [Number of times an HLE execution aborted due to none of the previous 4
            categories (e.g. interrupts)]
      hle_retired.commit                                
           [Number of times an HLE execution successfully committed]
      hle_retired.start                                 
           [Number of times an HLE execution started]
      machine_clears.memory_ordering                    
           [Counts the number of machine clears due to memory order conflicts]
      mem_trans_retired.load_latency_gt_128             
           [Randomly selected loads with latency value being above 128 Spec
            update: HSD76, HSD25, HSM26 (Must be precise)]
      mem_trans_retired.load_latency_gt_16              
           [Randomly selected loads with latency value being above 16 Spec update:
            HSD76, HSD25, HSM26 (Must be precise)]
      mem_trans_retired.load_latency_gt_256             
           [Randomly selected loads with latency value being above 256 Spec
            update: HSD76, HSD25, HSM26 (Must be precise)]
      mem_trans_retired.load_latency_gt_32              
           [Randomly selected loads with latency value being above 32 Spec update:
            HSD76, HSD25, HSM26 (Must be precise)]
      mem_trans_retired.load_latency_gt_4               
           [Randomly selected loads with latency value being above 4 Spec update:
            HSD76, HSD25, HSM26 (Must be precise)]
      mem_trans_retired.load_latency_gt_512             
           [Randomly selected loads with latency value being above 512 Spec
            update: HSD76, HSD25, HSM26 (Must be precise)]
      mem_trans_retired.load_latency_gt_64              
           [Randomly selected loads with latency value being above 64 Spec update:
            HSD76, HSD25, HSM26 (Must be precise)]
      mem_trans_retired.load_latency_gt_8               
           [Randomly selected loads with latency value being above 8 Spec update:
            HSD76, HSD25, HSM26 (Must be precise)]
      misalign_mem_ref.loads                            
           [Speculative cache line split load uops dispatched to L1 cache]
      misalign_mem_ref.stores                           
           [Speculative cache line split STA uops dispatched to L1 cache]
      offcore_response.all_code_rd.llc_miss.any_response
           [Counts all demand & prefetch code reads miss in the L3]
      offcore_response.all_code_rd.llc_miss.local_dram  
           [Counts all demand & prefetch code reads miss the L3 and the data is
            returned from local dram]
      offcore_response.all_data_rd.llc_miss.any_response
           [Counts all demand & prefetch data reads miss in the L3]
      offcore_response.all_data_rd.llc_miss.local_dram  
           [Counts all demand & prefetch data reads miss the L3 and the data is
            returned from local dram]
      offcore_response.all_data_rd.llc_miss.remote_dram 
           [Counts all demand & prefetch data reads miss the L3 and the data is
            returned from remote dram]
      offcore_response.all_data_rd.llc_miss.remote_hit_forward
           [Counts all demand & prefetch data reads miss the L3 and clean or
            shared data is transferred from remote cache]
      offcore_response.all_data_rd.llc_miss.remote_hitm 
           [Counts all demand & prefetch data reads miss the L3 and the modified
            data is transferred from remote cache]
      offcore_response.all_reads.llc_miss.any_response  
           [Counts all data/code/rfo reads (demand & prefetch) miss in the L3]
      offcore_response.all_reads.llc_miss.local_dram    
           [Counts all data/code/rfo reads (demand & prefetch) miss the L3 and the
            data is returned from local dram]
      offcore_response.all_reads.llc_miss.remote_dram   
           [Counts all data/code/rfo reads (demand & prefetch) miss the L3 and the
            data is returned from remote dram]
      offcore_response.all_reads.llc_miss.remote_hit_forward
           [Counts all data/code/rfo reads (demand & prefetch) miss the L3 and
            clean or shared data is transferred from remote cache]
      offcore_response.all_reads.llc_miss.remote_hitm   
           [Counts all data/code/rfo reads (demand & prefetch) miss the L3 and the
            modified data is transferred from remote cache]
      offcore_response.all_requests.llc_miss.any_response
           [Counts all requests miss in the L3]
      offcore_response.all_rfo.llc_miss.any_response    
           [Counts all demand & prefetch RFOs miss in the L3]
      offcore_response.all_rfo.llc_miss.local_dram      
           [Counts all demand & prefetch RFOs miss the L3 and the data is returned
            from local dram]
      offcore_response.demand_code_rd.llc_miss.any_response
           [Counts all demand code reads miss in the L3]
      offcore_response.demand_code_rd.llc_miss.local_dram
           [Counts all demand code reads miss the L3 and the data is returned from
            local dram]
      offcore_response.demand_data_rd.llc_miss.any_response
           [Counts demand data reads miss in the L3]
      offcore_response.demand_data_rd.llc_miss.local_dram
           [Counts demand data reads miss the L3 and the data is returned from
            local dram]
      offcore_response.demand_rfo.llc_miss.any_response 
           [Counts all demand data writes (RFOs) miss in the L3]
      offcore_response.demand_rfo.llc_miss.local_dram   
           [Counts all demand data writes (RFOs) miss the L3 and the data is
            returned from local dram]
      offcore_response.demand_rfo.llc_miss.remote_hitm  
           [Counts all demand data writes (RFOs) miss the L3 and the modified data
            is transferred from remote cache]
      offcore_response.pf_l2_code_rd.llc_miss.any_response
           [Counts all prefetch (that bring data to LLC only) code reads miss in
            the L3]
      offcore_response.pf_l2_data_rd.llc_miss.any_response
           [Counts prefetch (that bring data to L2) data reads miss in the L3]
      offcore_response.pf_l2_rfo.llc_miss.any_response  
           [Counts all prefetch (that bring data to L2) RFOs miss in the L3]
      offcore_response.pf_llc_code_rd.llc_miss.any_response
           [Counts prefetch (that bring data to LLC only) code reads miss in the
            L3]
      offcore_response.pf_llc_data_rd.llc_miss.any_response
           [Counts all prefetch (that bring data to LLC only) data reads miss in
            the L3]
      offcore_response.pf_llc_rfo.llc_miss.any_response 
           [Counts all prefetch (that bring data to LLC only) RFOs miss in the L3]
      rtm_retired.aborted                               
           [Number of times an RTM execution aborted due to any reasons (multiple
            categories may count as one) (Precise event)]
      rtm_retired.aborted_misc1                         
           [Number of times an RTM execution aborted due to various memory events
            (e.g. read/write capacity and conflicts)]
      rtm_retired.aborted_misc2                         
           [Number of times an RTM execution aborted due to various memory events
            (e.g., read/write capacity and conflicts)]
      rtm_retired.aborted_misc3                         
           [Number of times an RTM execution aborted due to HLE-unfriendly
            instructions]
      rtm_retired.aborted_misc4                         
           [Number of times an RTM execution aborted due to incompatible memory
            type Spec update: HSD65]
      rtm_retired.aborted_misc5                         
           [Number of times an RTM execution aborted due to none of the previous 4
            categories (e.g. interrupt)]
      rtm_retired.commit                                
           [Number of times an RTM execution successfully committed]
      rtm_retired.start                                 
           [Number of times an RTM execution started]
      tx_exec.misc1                                     
           [Counts the number of times a class of instructions that may cause a
            transactional abort was executed. Since this is the count of
            execution, it may not always cause a transactional abort]
      tx_exec.misc2                                     
           [Counts the number of times a class of instructions (e.g., vzeroupper)
            that may cause a transactional abort was executed inside a
            transactional region]
      tx_exec.misc3                                     
           [Counts the number of times an instruction execution caused the
            transactional nest count supported to be exceeded]
      tx_exec.misc4                                     
           [Counts the number of times a XBEGIN instruction was executed inside an
            HLE transactional region]
      tx_exec.misc5                                     
           [Counts the number of times an HLE XACQUIRE instruction was executed
            inside an RTM transactional region]
      tx_mem.abort_capacity_write                       
           [Number of times a transactional abort was signaled due to a data
            capacity limitation for transactional writes]
      tx_mem.abort_conflict                             
           [Number of times a transactional abort was signaled due to a data
            conflict on a transactionally accessed address]
      tx_mem.abort_hle_elision_buffer_mismatch          
           [Number of times an HLE transactional execution aborted due to XRELEASE
            lock not satisfying the address and value requirements in the elision
            buffer]
      tx_mem.abort_hle_elision_buffer_not_empty         
           [Number of times an HLE transactional execution aborted due to
            NoAllocatedElisionBuffer being non-zero]
      tx_mem.abort_hle_elision_buffer_unsupported_alignment
           [Number of times an HLE transactional execution aborted due to an
            unsupported read alignment from the elision buffer]
      tx_mem.abort_hle_store_to_elided_lock             
           [Number of times a HLE transactional region aborted due to a non
            XRELEASE prefixed instruction writing to an elided lock in the elision
            buffer]
      tx_mem.hle_elision_buffer_full                    
           [Number of times HLE lock could not be elided due to
            ElisionBufferAvailable being zero]
    
    other:
      cpl_cycles.ring0                                  
           [Unhalted core cycles when the thread is in ring 0]
      cpl_cycles.ring0_trans                            
           [Number of intervals between processor halts while thread is in ring 0]
      cpl_cycles.ring123                                
           [Unhalted core cycles when thread is in rings 1, 2, or 3]
      lock_cycles.split_lock_uc_lock_duration           
           [Cycles when L1 and L2 are locked due to UC or split lock]
    
    pipeline:
      arith.divider_uops                                
           [Any uop executed by the Divider. (This includes all divide uops, sqrt,
            ...)]
      baclears.any                                      
           [Counts the total number when the front end is resteered, mainly when
            the BPU cannot provide a correct prediction and this is corrected by
            other branch handling mechanisms at the front end]
      br_inst_exec.all_branches                         
           [Speculative and retired branches]
      br_inst_exec.all_conditional                      
           [Speculative and retired macro-conditional branches]
      br_inst_exec.all_direct_jmp                       
           [Speculative and retired macro-unconditional branches excluding calls
            and indirects]
      br_inst_exec.all_direct_near_call                 
           [Speculative and retired direct near calls]
      br_inst_exec.all_indirect_jump_non_call_ret       
           [Speculative and retired indirect branches excluding calls and returns]
      br_inst_exec.all_indirect_near_return             
           [Speculative and retired indirect return branches]
      br_inst_exec.nontaken_conditional                 
           [Not taken macro-conditional branches]
      br_inst_exec.taken_conditional                    
           [Taken speculative and retired macro-conditional branches]
      br_inst_exec.taken_direct_jump                    
           [Taken speculative and retired macro-conditional branch instructions
            excluding calls and indirects]
      br_inst_exec.taken_direct_near_call               
           [Taken speculative and retired direct near calls]
      br_inst_exec.taken_indirect_jump_non_call_ret     
           [Taken speculative and retired indirect branches excluding calls and
            returns]
      br_inst_exec.taken_indirect_near_call             
           [Taken speculative and retired indirect calls]
      br_inst_exec.taken_indirect_near_return           
           [Taken speculative and retired indirect branches with return mnemonic]
      br_inst_retired.all_branches                      
           [All (macro) branch instructions retired]
      br_inst_retired.all_branches_pebs                 
           [All (macro) branch instructions retired (Must be precise)]
      br_inst_retired.conditional                       
           [Conditional branch instructions retired (Precise event)]
      br_inst_retired.far_branch                        
           [Far branch instructions retired]
      br_inst_retired.near_call                         
           [Direct and indirect near call instructions retired (Precise event)]
      br_inst_retired.near_call_r3                      
           [Direct and indirect macro near call instructions retired (captured in
            ring 3) (Precise event)]
      br_inst_retired.near_return                       
           [Return instructions retired (Precise event)]
      br_inst_retired.near_taken                        
           [Taken branch instructions retired (Precise event)]
      br_inst_retired.not_taken                         
           [Not taken branch instructions retired]
      br_misp_exec.all_branches                         
           [Speculative and retired mispredicted macro conditional branches]
      br_misp_exec.all_conditional                      
           [Speculative and retired mispredicted macro conditional branches]
      br_misp_exec.all_indirect_jump_non_call_ret       
           [Mispredicted indirect branches excluding calls and returns]
      br_misp_exec.nontaken_conditional                 
           [Not taken speculative and retired mispredicted macro conditional
            branches]
      br_misp_exec.taken_conditional                    
           [Taken speculative and retired mispredicted macro conditional branches]
      br_misp_exec.taken_indirect_jump_non_call_ret     
           [Taken speculative and retired mispredicted indirect branches excluding
            calls and returns]
      br_misp_exec.taken_indirect_near_call             
           [Taken speculative and retired mispredicted indirect calls]
      br_misp_exec.taken_return_near                    
           [Taken speculative and retired mispredicted indirect branches with
            return mnemonic]
      br_misp_retired.all_branches                      
           [All mispredicted macro branch instructions retired]
      br_misp_retired.all_branches_pebs                 
           [Mispredicted macro branch instructions retired (Must be precise)]
      br_misp_retired.conditional                       
           [Mispredicted conditional branch instructions retired (Precise event)]
      br_misp_retired.near_taken                        
           [number of near branch instructions retired that were mispredicted and
            taken (Precise event)]
      cpu_clk_thread_unhalted.one_thread_active         
           [Count XClk pulses when this thread is unhalted and the other thread is
            halted]
      cpu_clk_thread_unhalted.ref_xclk                  
           [Reference cycles when the thread is unhalted (counts at 100 MHz rate)]
      cpu_clk_thread_unhalted.ref_xclk_any              
           [Reference cycles when the at least one thread on the physical core is
            unhalted (counts at 100 MHz rate)]
      cpu_clk_unhalted.one_thread_active                
           [Count XClk pulses when this thread is unhalted and the other thread is
            halted]
      cpu_clk_unhalted.ref_tsc                          
           [Reference cycles when the core is not in halt state]
      cpu_clk_unhalted.ref_xclk                         
           [Reference cycles when the thread is unhalted (counts at 100 MHz rate)]
      cpu_clk_unhalted.ref_xclk_any                     
           [Reference cycles when the at least one thread on the physical core is
            unhalted (counts at 100 MHz rate)]
      cpu_clk_unhalted.thread                           
           [Core cycles when the thread is not in halt state]
      cpu_clk_unhalted.thread_any                       
           [Core cycles when at least one thread on the physical core is not in
            halt state]
      cpu_clk_unhalted.thread_p                         
           [Thread cycles when thread is not in halt state]
      cpu_clk_unhalted.thread_p_any                     
           [Core cycles when at least one thread on the physical core is not in
            halt state]
      cycle_activity.cycles_l1d_pending                 
           [Cycles with pending L1 cache miss loads]
      cycle_activity.cycles_l2_pending                  
           [Cycles with pending L2 cache miss loads Spec update: HSD78]
      cycle_activity.cycles_ldm_pending                 
           [Cycles with pending memory loads]
      cycle_activity.cycles_no_execute                  
           [This event increments by 1 for every cycle where there was no execute
            for this thread]
      cycle_activity.stalls_l1d_pending                 
           [Execution stalls due to L1 data cache misses]
      cycle_activity.stalls_l2_pending                  
           [Execution stalls due to L2 cache misses]
      cycle_activity.stalls_ldm_pending                 
           [Execution stalls due to memory subsystem]
      ild_stall.iq_full                                 
           [Stall cycles because IQ is full]
      ild_stall.lcp                                     
           [Stalls caused by changing prefix length of the instruction]
      inst_retired.any                                  
           [Instructions retired from execution Spec update: HSD140, HSD143]
      inst_retired.any_p                                
           [Number of instructions retired. General Counter - architectural event
            Spec update: HSD11, HSD140]
      inst_retired.prec_dist                            
           [Precise instruction retired event with HW to reduce effect of PEBS
            shadow in IP distribution Spec update: HSD140 (Must be precise)]
      inst_retired.x87                                  
           [FP operations retired. X87 FP operations that have no exceptions:
            Counts also flows that have several X87 or flows that use X87 uops in
            the exception handling]
      int_misc.recovery_cycles                          
           [Core cycles the allocator was stalled due to recovery from earlier
            clear event for this thread (e.g. misprediction or memory nuke)]
      int_misc.recovery_cycles_any                      
           [Core cycles the allocator was stalled due to recovery from earlier
            clear event for any thread running on the physical core (e.g.
            misprediction or memory nuke)]
      ld_blocks.no_sr                                   
           [The number of times that split load operations are temporarily blocked
            because all resources for handling the split accesses are in use]
      ld_blocks.store_forward                           
           [loads blocked by overlapping with store buffer that cannot be
            forwarded]
      ld_blocks_partial.address_alias                   
           [False dependencies in MOB due to partial compare on address]
      load_hit_pre.hw_pf                                
           [Not software-prefetch load dispatches that hit FB allocated for
            hardware prefetch]
      load_hit_pre.sw_pf                                
           [Not software-prefetch load dispatches that hit FB allocated for
            software prefetch]
      lsd.cycles_4_uops                                 
           [Cycles 4 Uops delivered by the LSD, but didn't come from the decoder]
      lsd.cycles_active                                 
           [Cycles Uops delivered by the LSD, but didn't come from the decoder]
      lsd.uops                                          
           [Number of Uops delivered by the LSD]
      machine_clears.count                              
           [Number of machine clears (nukes) of any type]
      machine_clears.cycles                             
           [Cycles there was a Nuke. Account for both thread-specific and All
            Thread Nukes]
      machine_clears.maskmov                            
           [This event counts the number of executed Intel AVX masked load
            operations that refer to an illegal address range with the mask bits
            set to 0]
      machine_clears.smc                                
           [Self-modifying code (SMC) detected]
      move_elimination.int_eliminated                   
           [Number of integer Move Elimination candidate uops that were eliminated]
      move_elimination.int_not_eliminated               
           [Number of integer Move Elimination candidate uops that were not
            eliminated]
      move_elimination.simd_eliminated                  
           [Number of SIMD Move Elimination candidate uops that were eliminated]
      move_elimination.simd_not_eliminated              
           [Number of SIMD Move Elimination candidate uops that were not
            eliminated]
      other_assists.any_wb_assist                       
           [Number of times any microcode assist is invoked by HW upon uop
            writeback]
      resource_stalls.any                               
           [Resource-related stall cycles Spec update: HSD135]
      resource_stalls.rob                               
           [Cycles stalled due to re-order buffer full]
      resource_stalls.rs                                
           [Cycles stalled due to no eligible RS entry available]
      resource_stalls.sb                                
           [Cycles stalled due to no store buffers available. (not including
            draining form sync)]
      rob_misc_events.lbr_inserts                       
           [Count cases of saving new LBR]
      rs_events.empty_cycles                            
           [Cycles when Reservation Station (RS) is empty for the thread]
      rs_events.empty_end                               
           [Counts end of periods where the Reservation Station (RS) was empty.
            Could be useful to precisely locate Frontend Latency Bound issues]
      uops_dispatched_port.port_0                       
           [Cycles per thread when uops are executed in port 0]
      uops_dispatched_port.port_1                       
           [Cycles per thread when uops are executed in port 1]
      uops_dispatched_port.port_2                       
           [Cycles per thread when uops are executed in port 2]
      uops_dispatched_port.port_3                       
           [Cycles per thread when uops are executed in port 3]
      uops_dispatched_port.port_4                       
           [Cycles per thread when uops are executed in port 4]
      uops_dispatched_port.port_5                       
           [Cycles per thread when uops are executed in port 5]
      uops_dispatched_port.port_6                       
           [Cycles per thread when uops are executed in port 6]
      uops_dispatched_port.port_7                       
           [Cycles per thread when uops are executed in port 7]
      uops_executed.core                                
           [Number of uops executed on the core Spec update: HSD30, HSM31]
      uops_executed.core_cycles_ge_1                    
           [Cycles at least 1 micro-op is executed from any thread on physical
            core Spec update: HSD30, HSM31]
      uops_executed.core_cycles_ge_2                    
           [Cycles at least 2 micro-op is executed from any thread on physical
            core Spec update: HSD30, HSM31]
      uops_executed.core_cycles_ge_3                    
           [Cycles at least 3 micro-op is executed from any thread on physical
            core Spec update: HSD30, HSM31]
      uops_executed.core_cycles_ge_4                    
           [Cycles at least 4 micro-op is executed from any thread on physical
            core Spec update: HSD30, HSM31]
      uops_executed.core_cycles_none                    
           [Cycles with no micro-ops executed from any thread on physical core
            Spec update: HSD30, HSM31]
      uops_executed.cycles_ge_1_uop_exec                
           [Cycles where at least 1 uop was executed per-thread Spec update:
            HSD144, HSD30, HSM31]
      uops_executed.cycles_ge_2_uops_exec               
           [Cycles where at least 2 uops were executed per-thread Spec update:
            HSD144, HSD30, HSM31]
      uops_executed.cycles_ge_3_uops_exec               
           [Cycles where at least 3 uops were executed per-thread Spec update:
            HSD144, HSD30, HSM31]
      uops_executed.cycles_ge_4_uops_exec               
           [Cycles where at least 4 uops were executed per-thread Spec update:
            HSD144, HSD30, HSM31]
      uops_executed.stall_cycles                        
           [Counts number of cycles no uops were dispatched to be executed on this
            thread Spec update: HSD144, HSD30, HSM31]
      uops_executed_port.port_0                         
           [Cycles per thread when uops are executed in port 0]
      uops_executed_port.port_0_core                    
           [Cycles per core when uops are executed in port 0]
      uops_executed_port.port_1                         
           [Cycles per thread when uops are executed in port 1]
      uops_executed_port.port_1_core                    
           [Cycles per core when uops are executed in port 1]
      uops_executed_port.port_2                         
           [Cycles per thread when uops are executed in port 2]
      uops_executed_port.port_2_core                    
           [Cycles per core when uops are dispatched to port 2]
      uops_executed_port.port_3                         
           [Cycles per thread when uops are executed in port 3]
      uops_executed_port.port_3_core                    
           [Cycles per core when uops are dispatched to port 3]
      uops_executed_port.port_4                         
           [Cycles per thread when uops are executed in port 4]
      uops_executed_port.port_4_core                    
           [Cycles per core when uops are executed in port 4]
      uops_executed_port.port_5                         
           [Cycles per thread when uops are executed in port 5]
      uops_executed_port.port_5_core                    
           [Cycles per core when uops are executed in port 5]
      uops_executed_port.port_6                         
           [Cycles per thread when uops are executed in port 6]
      uops_executed_port.port_6_core                    
           [Cycles per core when uops are executed in port 6]
      uops_executed_port.port_7                         
           [Cycles per thread when uops are executed in port 7]
      uops_executed_port.port_7_core                    
           [Cycles per core when uops are dispatched to port 7]
      uops_issued.any                                   
           [Uops that Resource Allocation Table (RAT) issues to Reservation
            Station (RS)]
      uops_issued.core_stall_cycles                     
           [Cycles when Resource Allocation Table (RAT) does not issue Uops to
            Reservation Station (RS) for all threads]
      uops_issued.flags_merge                           
           [Number of flags-merge uops being allocated. Such uops considered perf
            sensitive; added by GSR u-arch]
      uops_issued.single_mul                            
           [Number of Multiply packed/scalar single precision uops allocated]
      uops_issued.slow_lea                              
           [Number of slow LEA uops being allocated. A uop is generally considered
            SlowLea if it has 3 sources (e.g. 2 sources + immediate) regardless if
            as a result of LEA instruction or not]
      uops_issued.stall_cycles                          
           [Cycles when Resource Allocation Table (RAT) does not issue Uops to
            Reservation Station (RS) for the thread]
      uops_retired.all                                  
           [Actually retired uops Supports address when precise (Precise event)]
      uops_retired.core_stall_cycles                    
           [Cycles without actually retired uops]
      uops_retired.retire_slots                         
           [Retirement slots used (Precise event)]
      uops_retired.stall_cycles                         
           [Cycles without actually retired uops]
      uops_retired.total_cycles                         
           [Cycles with less than 10 actually retired uops]
    
    uncore cache:
      llc_misses.code_llc_prefetch                      
           [LLC prefetch misses for code reads. Derived from
            unc_c_tor_inserts.miss_opcode. Unit: uncore_cbox]
      llc_misses.data_llc_prefetch                      
           [LLC prefetch misses for data reads. Derived from
            unc_c_tor_inserts.miss_opcode. Unit: uncore_cbox]
      llc_misses.data_read                              
           [LLC misses - demand and prefetch data reads - excludes LLC prefetches.
            Derived from unc_c_tor_inserts.miss_opcode. Unit: uncore_cbox]
      llc_misses.mmio_read                              
           [MMIO reads. Derived from unc_c_tor_inserts.miss_opcode. Unit:
            uncore_cbox]
      llc_misses.mmio_write                             
           [MMIO writes. Derived from unc_c_tor_inserts.miss_opcode. Unit:
            uncore_cbox]
      llc_misses.pcie_non_snoop_write                   
           [PCIe write misses (full cache line). Derived from
            unc_c_tor_inserts.miss_opcode. Unit: uncore_cbox]
      llc_misses.pcie_read                              
           [LLC misses for PCIe read current. Derived from
            unc_c_tor_inserts.miss_opcode. Unit: uncore_cbox]
      llc_misses.pcie_write                             
           [ItoM write misses (as part of fast string memcpy stores) + PCIe full
            line writes. Derived from unc_c_tor_inserts.miss_opcode. Unit:
            uncore_cbox]
      llc_misses.rfo_llc_prefetch                       
           [LLC prefetch misses for RFO. Derived from
            unc_c_tor_inserts.miss_opcode. Unit: uncore_cbox]
      llc_misses.uncacheable                            
           [LLC misses - Uncacheable reads (from cpu) . Derived from
            unc_c_tor_inserts.miss_opcode. Unit: uncore_cbox]
      llc_references.code_llc_prefetch                  
           [L2 demand and L2 prefetch code references to LLC. Derived from
            unc_c_tor_inserts.opcode. Unit: uncore_cbox]
      llc_references.pcie_ns_partial_write              
           [PCIe writes (partial cache line). Derived from
            unc_c_tor_inserts.opcode. Unit: uncore_cbox]
      llc_references.pcie_read                          
           [PCIe read current. Derived from unc_c_tor_inserts.opcode. Unit:
            uncore_cbox]
      llc_references.pcie_write                         
           [PCIe write references (full cache line). Derived from
            unc_c_tor_inserts.opcode. Unit: uncore_cbox]
      llc_references.streaming_full                     
           [Streaming stores (full cache line). Derived from
            unc_c_tor_inserts.opcode. Unit: uncore_cbox]
      llc_references.streaming_partial                  
           [Streaming stores (partial cache line). Derived from
            unc_c_tor_inserts.opcode. Unit: uncore_cbox]
      unc_c_clockticks                                  
           [Uncore cache clock ticks. Unit: uncore_cbox]
      unc_c_llc_lookup.any                              
           [All LLC Misses (code+ data rd + data wr - including demand and
            prefetch). Unit: uncore_cbox]
      unc_c_llc_victims.m_state                         
           [M line evictions from LLC (writebacks to memory). Unit: uncore_cbox]
      unc_c_tor_occupancy.llc_data_read                 
           [Occupancy counter for LLC data reads (demand and L2 prefetch). Derived
            from unc_c_tor_occupancy.miss_opcode. Unit: uncore_cbox]
      unc_h_requests.reads                              
           [read requests to home agent. Unit: uncore_ha]
      unc_h_requests.reads_local                        
           [read requests to local home agent. Unit: uncore_ha]
      unc_h_requests.reads_remote                       
           [read requests to remote home agent. Unit: uncore_ha]
      unc_h_requests.writes                             
           [write requests to home agent. Unit: uncore_ha]
      unc_h_requests.writes_local                       
           [write requests to local home agent. Unit: uncore_ha]
      unc_h_requests.writes_remote                      
           [write requests to remote home agent. Unit: uncore_ha]
      unc_h_snoop_resp.rsp_fwd_wb                       
           [M line forwarded from remote cache along with writeback to memory.
            Unit: uncore_ha]
      unc_h_snoop_resp.rspcnflct                        
           [Conflict requests (requests for same address from multiple agents
            simultaneously). Unit: uncore_ha]
      unc_h_snoop_resp.rspifwd                          
           [M line forwarded from remote cache with no writeback to memory. Unit:
            uncore_ha]
      unc_h_snoop_resp.rsps                             
           [Shared line response from remote cache. Unit: uncore_ha]
      unc_h_snoop_resp.rspsfwd                          
           [Shared line forwarded from remote cache. Unit: uncore_ha]
    
    uncore memory:
      llc_misses.mem_read                               
           [read requests to memory controller. Derived from unc_m_cas_count.rd.
            Unit: uncore_imc]
      llc_misses.mem_write                              
           [write requests to memory controller. Derived from unc_m_cas_count.wr.
            Unit: uncore_imc]
      unc_m_clockticks                                  
           [Memory controller clock ticks. Unit: uncore_imc]
      unc_m_power_channel_ppd                           
           [Cycles where DRAM ranks are in power down (CKE) mode. Unit: uncore_imc]
      unc_m_power_critical_throttle_cycles              
           [Cycles all ranks are in critical thermal throttle. Unit: uncore_imc]
      unc_m_power_self_refresh                          
           [Cycles Memory is in self refresh power mode. Unit: uncore_imc]
      unc_m_pre_count.page_miss                         
           [Pre-charges due to page misses. Unit: uncore_imc]
      unc_m_pre_count.rd                                
           [Pre-charge for reads. Unit: uncore_imc]
      unc_m_pre_count.wr                                
           [Pre-charge for writes. Unit: uncore_imc]
    
    uncore power:
      unc_p_clockticks                                  
           [PCU clock ticks. Use to get percentages of PCU cycles events. Unit:
            uncore_pcu]
      unc_p_freq_max_limit_thermal_cycles               
           [Counts the number of cycles when temperature is the upper limit on
            frequency. Unit: uncore_pcu]
      unc_p_freq_max_os_cycles                          
           [Counts the number of cycles when the OS is the upper limit on
            frequency. Unit: uncore_pcu]
      unc_p_freq_max_power_cycles                       
           [Counts the number of cycles when power is the upper limit on
            frequency. Unit: uncore_pcu]
      unc_p_freq_trans_cycles                           
           [Counts the number of cycles when current is the upper limit on
            frequency. Unit: uncore_pcu]
      unc_p_power_state_occupancy.cores_c0              
           [This is an occupancy event that tracks the number of cores that are in
            C0. It can be used by itself to get the average number of cores in C0,
            with threshholding to generate histograms, or with other PCU events
            and occupancy triggering to capture other details. Unit: uncore_pcu]
      unc_p_power_state_occupancy.cores_c3              
           [This is an occupancy event that tracks the number of cores that are in
            C3. It can be used by itself to get the average number of cores in C0,
            with threshholding to generate histograms, or with other PCU events
            and occupancy triggering to capture other details. Unit: uncore_pcu]
      unc_p_power_state_occupancy.cores_c6              
           [This is an occupancy event that tracks the number of cores that are in
            C6. It can be used by itself to get the average number of cores in C0,
            with threshholding to generate histograms, or with other PCU events .
            Unit: uncore_pcu]
      unc_p_prochot_external_cycles                     
           [Counts the number of cycles that we are in external PROCHOT mode. This
            mode is triggered when a sensor off the die determines that something
            off-die (like DRAM) is too hot and must throttle to avoid damaging the
            chip. Unit: uncore_pcu]
    
    virtual memory:
      dtlb_load_misses.miss_causes_a_walk               
           [Load misses in all DTLB levels that cause page walks]
      dtlb_load_misses.pde_cache_miss                   
           [DTLB demand load misses with low part of linear-to-physical address
            translation missed]
      dtlb_load_misses.stlb_hit                         
           [Load operations that miss the first DTLB level but hit the second and
            do not cause page walks]
      dtlb_load_misses.stlb_hit_2m                      
           [Load misses that miss the DTLB and hit the STLB (2M)]
      dtlb_load_misses.stlb_hit_4k                      
           [Load misses that miss the DTLB and hit the STLB (4K)]
      dtlb_load_misses.walk_completed                   
           [Demand load Miss in all translation lookaside buffer (TLB) levels
            causes a page walk that completes of any page size]
      dtlb_load_misses.walk_completed_1g                
           [Load miss in all TLB levels causes a page walk that completes. (1G)]
      dtlb_load_misses.walk_completed_2m_4m             
           [Demand load Miss in all translation lookaside buffer (TLB) levels
            causes a page walk that completes (2M/4M)]
      dtlb_load_misses.walk_completed_4k                
           [Demand load Miss in all translation lookaside buffer (TLB) levels
            causes a page walk that completes (4K)]
      dtlb_load_misses.walk_duration                    
           [Cycles when PMH is busy with page walks]
      dtlb_store_misses.miss_causes_a_walk              
           [Store misses in all DTLB levels that cause page walks]
      dtlb_store_misses.pde_cache_miss                  
           [DTLB store misses with low part of linear-to-physical address
            translation missed]
      dtlb_store_misses.stlb_hit                        
           [Store operations that miss the first TLB level but hit the second and
            do not cause page walks]
      dtlb_store_misses.stlb_hit_2m                     
           [Store misses that miss the DTLB and hit the STLB (2M)]
      dtlb_store_misses.stlb_hit_4k                     
           [Store misses that miss the DTLB and hit the STLB (4K)]
      dtlb_store_misses.walk_completed                  
           [Store misses in all DTLB levels that cause completed page walks]
      dtlb_store_misses.walk_completed_1g               
           [Store misses in all DTLB levels that cause completed page walks. (1G)]
      dtlb_store_misses.walk_completed_2m_4m            
           [Store misses in all DTLB levels that cause completed page walks
            (2M/4M)]
      dtlb_store_misses.walk_completed_4k               
           [Store miss in all TLB levels causes a page walk that completes. (4K)]
      dtlb_store_misses.walk_duration                   
           [Cycles when PMH is busy with page walks]
      ept.walk_cycles                                   
           [Cycle count for an Extended Page table walk]
      itlb.itlb_flush                                   
           [Flushing of the Instruction TLB (ITLB) pages, includes 4k/2M/4M pages]
      itlb_misses.miss_causes_a_walk                    
           [Misses at all ITLB levels that cause page walks]
      itlb_misses.stlb_hit                              
           [Operations that miss the first ITLB level but hit the second and do
            not cause any page walks]
      itlb_misses.stlb_hit_2m                           
           [Code misses that miss the DTLB and hit the STLB (2M)]
      itlb_misses.stlb_hit_4k                           
           [Core misses that miss the DTLB and hit the STLB (4K)]
      itlb_misses.walk_completed                        
           [Misses in all ITLB levels that cause completed page walks]
      itlb_misses.walk_completed_1g                     
           [Store miss in all TLB levels causes a page walk that completes. (1G)]
      itlb_misses.walk_completed_2m_4m                  
           [Code miss in all TLB levels causes a page walk that completes. (2M/4M)]
      itlb_misses.walk_completed_4k                     
           [Code miss in all TLB levels causes a page walk that completes. (4K)]
      itlb_misses.walk_duration                         
           [Cycles when PMH is busy with page walks]
      page_walker_loads.dtlb_l1                         
           [Number of DTLB page walker hits in the L1+FB]
      page_walker_loads.dtlb_l2                         
           [Number of DTLB page walker hits in the L2]
      page_walker_loads.dtlb_l3                         
           [Number of DTLB page walker hits in the L3 + XSNP Spec update: HSD25]
      page_walker_loads.dtlb_memory                     
           [Number of DTLB page walker hits in Memory Spec update: HSD25]
      page_walker_loads.ept_dtlb_l1                     
           [Counts the number of Extended Page Table walks from the DTLB that hit
            in the L1 and FB]
      page_walker_loads.ept_dtlb_l2                     
           [Counts the number of Extended Page Table walks from the DTLB that hit
            in the L2]
      page_walker_loads.ept_dtlb_l3                     
           [Counts the number of Extended Page Table walks from the DTLB that hit
            in the L3]
      page_walker_loads.ept_dtlb_memory                 
           [Counts the number of Extended Page Table walks from the DTLB that hit
            in memory]
      page_walker_loads.ept_itlb_l1                     
           [Counts the number of Extended Page Table walks from the ITLB that hit
            in the L1 and FB]
      page_walker_loads.ept_itlb_l2                     
           [Counts the number of Extended Page Table walks from the ITLB that hit
            in the L2]
      page_walker_loads.ept_itlb_l3                     
           [Counts the number of Extended Page Table walks from the ITLB that hit
            in the L2]
      page_walker_loads.ept_itlb_memory                 
           [Counts the number of Extended Page Table walks from the ITLB that hit
            in memory]
      page_walker_loads.itlb_l1                         
           [Number of ITLB page walker hits in the L1+FB]
      page_walker_loads.itlb_l2                         
           [Number of ITLB page walker hits in the L2]
      page_walker_loads.itlb_l3                         
           [Number of ITLB page walker hits in the L3 + XSNP Spec update: HSD25]
      page_walker_loads.itlb_memory                     
           [Number of ITLB page walker hits in Memory Spec update: HSD25]
      tlb_flush.dtlb_thread                             
           [DTLB flush attempts of the thread-specific entries]
      tlb_flush.stlb_any                                
           [STLB flush attempts]
      rNNN                                               [Raw hardware event descriptor]
      cpu/t1=v1[,t2=v2,t3 ...]/modifier                  [Raw hardware event descriptor]
      mem:<addr>[/len][:access]                          [Hardware breakpoint]
      alarmtimer:alarmtimer_cancel                       [Tracepoint event]
      alarmtimer:alarmtimer_fired                        [Tracepoint event]
      alarmtimer:alarmtimer_start                        [Tracepoint event]
      alarmtimer:alarmtimer_suspend                      [Tracepoint event]
      block:block_bio_backmerge                          [Tracepoint event]
      block:block_bio_bounce                             [Tracepoint event]
      block:block_bio_complete                           [Tracepoint event]
      block:block_bio_frontmerge                         [Tracepoint event]
      block:block_bio_queue                              [Tracepoint event]
      block:block_bio_remap                              [Tracepoint event]
      block:block_dirty_buffer                           [Tracepoint event]
      block:block_getrq                                  [Tracepoint event]
      block:block_plug                                   [Tracepoint event]
      block:block_rq_complete                            [Tracepoint event]
      block:block_rq_insert                              [Tracepoint event]
      block:block_rq_issue                               [Tracepoint event]
      block:block_rq_remap                               [Tracepoint event]
      block:block_rq_requeue                             [Tracepoint event]
      block:block_sleeprq                                [Tracepoint event]
      block:block_split                                  [Tracepoint event]
      block:block_touch_buffer                           [Tracepoint event]
      block:block_unplug                                 [Tracepoint event]
      bpf_test_run:bpf_test_finish                       [Tracepoint event]
      bridge:br_fdb_add                                  [Tracepoint event]
      bridge:br_fdb_external_learn_add                   [Tracepoint event]
      bridge:br_fdb_update                               [Tracepoint event]
      bridge:fdb_delete                                  [Tracepoint event]
      btrfs:__extent_writepage                           [Tracepoint event]
      btrfs:add_delayed_data_ref                         [Tracepoint event]
      btrfs:add_delayed_ref_head                         [Tracepoint event]
      btrfs:add_delayed_tree_ref                         [Tracepoint event]
      btrfs:alloc_extent_state                           [Tracepoint event]
      btrfs:btrfs_add_block_group                        [Tracepoint event]
      btrfs:btrfs_add_unused_block_group                 [Tracepoint event]
      btrfs:btrfs_all_work_done                          [Tracepoint event]
      btrfs:btrfs_chunk_alloc                            [Tracepoint event]
      btrfs:btrfs_chunk_free                             [Tracepoint event]
      btrfs:btrfs_clear_extent_bit                       [Tracepoint event]
      btrfs:btrfs_convert_extent_bit                     [Tracepoint event]
      btrfs:btrfs_cow_block                              [Tracepoint event]
      btrfs:btrfs_failed_cluster_setup                   [Tracepoint event]
      btrfs:btrfs_find_cluster                           [Tracepoint event]
      btrfs:btrfs_flush_space                            [Tracepoint event]
      btrfs:btrfs_get_extent                             [Tracepoint event]
      btrfs:btrfs_get_extent_show_fi_inline              [Tracepoint event]
      btrfs:btrfs_get_extent_show_fi_regular             [Tracepoint event]
      btrfs:btrfs_handle_em_exist                        [Tracepoint event]
      btrfs:btrfs_inode_evict                            [Tracepoint event]
      btrfs:btrfs_inode_mod_outstanding_extents          [Tracepoint event]
      btrfs:btrfs_inode_new                              [Tracepoint event]
      btrfs:btrfs_inode_request                          [Tracepoint event]
      btrfs:btrfs_ordered_extent_add                     [Tracepoint event]
      btrfs:btrfs_ordered_extent_put                     [Tracepoint event]
      btrfs:btrfs_ordered_extent_remove                  [Tracepoint event]
      btrfs:btrfs_ordered_extent_start                   [Tracepoint event]
      btrfs:btrfs_ordered_sched                          [Tracepoint event]
      btrfs:btrfs_prelim_ref_insert                      [Tracepoint event]
      btrfs:btrfs_prelim_ref_merge                       [Tracepoint event]
      btrfs:btrfs_qgroup_account_extent                  [Tracepoint event]
      btrfs:btrfs_qgroup_account_extents                 [Tracepoint event]
      btrfs:btrfs_qgroup_release_data                    [Tracepoint event]
      btrfs:btrfs_qgroup_reserve_data                    [Tracepoint event]
      btrfs:btrfs_qgroup_trace_extent                    [Tracepoint event]
      btrfs:btrfs_remove_block_group                     [Tracepoint event]
      btrfs:btrfs_reserve_extent                         [Tracepoint event]
      btrfs:btrfs_reserve_extent_cluster                 [Tracepoint event]
      btrfs:btrfs_reserved_extent_alloc                  [Tracepoint event]
      btrfs:btrfs_reserved_extent_free                   [Tracepoint event]
      btrfs:btrfs_set_extent_bit                         [Tracepoint event]
      btrfs:btrfs_set_lock_blocking_read                 [Tracepoint event]
      btrfs:btrfs_set_lock_blocking_write                [Tracepoint event]
      btrfs:btrfs_setup_cluster                          [Tracepoint event]
      btrfs:btrfs_skip_unused_block_group                [Tracepoint event]
      btrfs:btrfs_space_reservation                      [Tracepoint event]
      btrfs:btrfs_sync_file                              [Tracepoint event]
      btrfs:btrfs_sync_fs                                [Tracepoint event]
      btrfs:btrfs_transaction_commit                     [Tracepoint event]
      btrfs:btrfs_tree_lock                              [Tracepoint event]
      btrfs:btrfs_tree_read_lock                         [Tracepoint event]
      btrfs:btrfs_tree_read_lock_atomic                  [Tracepoint event]
      btrfs:btrfs_tree_read_unlock                       [Tracepoint event]
      btrfs:btrfs_tree_read_unlock_blocking              [Tracepoint event]
      btrfs:btrfs_tree_unlock                            [Tracepoint event]
      btrfs:btrfs_trigger_flush                          [Tracepoint event]
      btrfs:btrfs_truncate_show_fi_inline                [Tracepoint event]
      btrfs:btrfs_truncate_show_fi_regular               [Tracepoint event]
      btrfs:btrfs_try_tree_read_lock                     [Tracepoint event]
      btrfs:btrfs_try_tree_write_lock                    [Tracepoint event]
      btrfs:btrfs_work_queued                            [Tracepoint event]
      btrfs:btrfs_work_sched                             [Tracepoint event]
      btrfs:btrfs_workqueue_alloc                        [Tracepoint event]
      btrfs:btrfs_workqueue_destroy                      [Tracepoint event]
      btrfs:btrfs_writepage_end_io_hook                  [Tracepoint event]
      btrfs:find_free_extent                             [Tracepoint event]
      btrfs:free_extent_state                            [Tracepoint event]
      btrfs:qgroup_meta_convert                          [Tracepoint event]
      btrfs:qgroup_meta_free_all_pertrans                [Tracepoint event]
      btrfs:qgroup_meta_reserve                          [Tracepoint event]
      btrfs:qgroup_num_dirty_extents                     [Tracepoint event]
      btrfs:qgroup_update_counters                       [Tracepoint event]
      btrfs:qgroup_update_reserve                        [Tracepoint event]
      btrfs:run_delayed_data_ref                         [Tracepoint event]
      btrfs:run_delayed_ref_head                         [Tracepoint event]
      btrfs:run_delayed_tree_ref                         [Tracepoint event]
      btrfs:update_bytes_may_use                         [Tracepoint event]
      btrfs:update_bytes_pinned                          [Tracepoint event]
      cgroup:cgroup_attach_task                          [Tracepoint event]
      cgroup:cgroup_destroy_root                         [Tracepoint event]
      cgroup:cgroup_freeze                               [Tracepoint event]
      cgroup:cgroup_mkdir                                [Tracepoint event]
      cgroup:cgroup_notify_frozen                        [Tracepoint event]
      cgroup:cgroup_notify_populated                     [Tracepoint event]
      cgroup:cgroup_release                              [Tracepoint event]
      cgroup:cgroup_remount                              [Tracepoint event]
      cgroup:cgroup_rename                               [Tracepoint event]
      cgroup:cgroup_rmdir                                [Tracepoint event]
      cgroup:cgroup_setup_root                           [Tracepoint event]
      cgroup:cgroup_transfer_tasks                       [Tracepoint event]
      cgroup:cgroup_unfreeze                             [Tracepoint event]
      clk:clk_disable                                    [Tracepoint event]
      clk:clk_disable_complete                           [Tracepoint event]
      clk:clk_enable                                     [Tracepoint event]
      clk:clk_enable_complete                            [Tracepoint event]
      clk:clk_prepare                                    [Tracepoint event]
      clk:clk_prepare_complete                           [Tracepoint event]
      clk:clk_set_duty_cycle                             [Tracepoint event]
      clk:clk_set_duty_cycle_complete                    [Tracepoint event]
      clk:clk_set_parent                                 [Tracepoint event]
      clk:clk_set_parent_complete                        [Tracepoint event]
      clk:clk_set_phase                                  [Tracepoint event]
      clk:clk_set_phase_complete                         [Tracepoint event]
      clk:clk_set_rate                                   [Tracepoint event]
      clk:clk_set_rate_complete                          [Tracepoint event]
      clk:clk_unprepare                                  [Tracepoint event]
      clk:clk_unprepare_complete                         [Tracepoint event]
      cma:cma_alloc                                      [Tracepoint event]
      cma:cma_release                                    [Tracepoint event]
      compaction:mm_compaction_begin                     [Tracepoint event]
      compaction:mm_compaction_defer_compaction          [Tracepoint event]
      compaction:mm_compaction_defer_reset               [Tracepoint event]
      compaction:mm_compaction_deferred                  [Tracepoint event]
      compaction:mm_compaction_end                       [Tracepoint event]
      compaction:mm_compaction_finished                  [Tracepoint event]
      compaction:mm_compaction_isolate_freepages         [Tracepoint event]
      compaction:mm_compaction_isolate_migratepages      [Tracepoint event]
      compaction:mm_compaction_kcompactd_sleep           [Tracepoint event]
      compaction:mm_compaction_kcompactd_wake            [Tracepoint event]
      compaction:mm_compaction_migratepages              [Tracepoint event]
      compaction:mm_compaction_suitable                  [Tracepoint event]
      compaction:mm_compaction_try_to_compact_pages      [Tracepoint event]
      compaction:mm_compaction_wakeup_kcompactd          [Tracepoint event]
      cpuhp:cpuhp_enter                                  [Tracepoint event]
      cpuhp:cpuhp_exit                                   [Tracepoint event]
      cpuhp:cpuhp_multi_enter                            [Tracepoint event]
      cros_ec:cros_ec_cmd                                [Tracepoint event]
      devfreq:devfreq_monitor                            [Tracepoint event]
      devlink:devlink_health_recover_aborted             [Tracepoint event]
      devlink:devlink_health_report                      [Tracepoint event]
      devlink:devlink_health_reporter_state_update       [Tracepoint event]
      devlink:devlink_hwerr                              [Tracepoint event]
      devlink:devlink_hwmsg                              [Tracepoint event]
      dma_fence:dma_fence_destroy                        [Tracepoint event]
      dma_fence:dma_fence_emit                           [Tracepoint event]
      dma_fence:dma_fence_enable_signal                  [Tracepoint event]
      dma_fence:dma_fence_init                           [Tracepoint event]
      dma_fence:dma_fence_signaled                       [Tracepoint event]
      dma_fence:dma_fence_wait_end                       [Tracepoint event]
      dma_fence:dma_fence_wait_start                     [Tracepoint event]
      drm:drm_vblank_event                               [Tracepoint event]
      drm:drm_vblank_event_delivered                     [Tracepoint event]
      drm:drm_vblank_event_queued                        [Tracepoint event]
      exceptions:page_fault_kernel                       [Tracepoint event]
      exceptions:page_fault_user                         [Tracepoint event]
      ext4:ext4_alloc_da_blocks                          [Tracepoint event]
      ext4:ext4_allocate_blocks                          [Tracepoint event]
      ext4:ext4_allocate_inode                           [Tracepoint event]
      ext4:ext4_begin_ordered_truncate                   [Tracepoint event]
      ext4:ext4_collapse_range                           [Tracepoint event]
      ext4:ext4_da_release_space                         [Tracepoint event]
      ext4:ext4_da_reserve_space                         [Tracepoint event]
      ext4:ext4_da_update_reserve_space                  [Tracepoint event]
      ext4:ext4_da_write_begin                           [Tracepoint event]
      ext4:ext4_da_write_end                             [Tracepoint event]
      ext4:ext4_da_write_pages                           [Tracepoint event]
      ext4:ext4_da_write_pages_extent                    [Tracepoint event]
      ext4:ext4_direct_IO_enter                          [Tracepoint event]
      ext4:ext4_direct_IO_exit                           [Tracepoint event]
      ext4:ext4_discard_blocks                           [Tracepoint event]
      ext4:ext4_discard_preallocations                   [Tracepoint event]
      ext4:ext4_drop_inode                               [Tracepoint event]
      ext4:ext4_error                                    [Tracepoint event]
      ext4:ext4_es_cache_extent                          [Tracepoint event]
      ext4:ext4_es_find_extent_range_enter               [Tracepoint event]
      ext4:ext4_es_find_extent_range_exit                [Tracepoint event]
      ext4:ext4_es_insert_delayed_block                  [Tracepoint event]
      ext4:ext4_es_insert_extent                         [Tracepoint event]
      ext4:ext4_es_lookup_extent_enter                   [Tracepoint event]
      ext4:ext4_es_lookup_extent_exit                    [Tracepoint event]
      ext4:ext4_es_remove_extent                         [Tracepoint event]
      ext4:ext4_es_shrink                                [Tracepoint event]
      ext4:ext4_es_shrink_count                          [Tracepoint event]
      ext4:ext4_es_shrink_scan_enter                     [Tracepoint event]
      ext4:ext4_es_shrink_scan_exit                      [Tracepoint event]
      ext4:ext4_evict_inode                              [Tracepoint event]
      ext4:ext4_ext_convert_to_initialized_enter         [Tracepoint event]
      ext4:ext4_ext_convert_to_initialized_fastpath      [Tracepoint event]
      ext4:ext4_ext_handle_unwritten_extents             [Tracepoint event]
      ext4:ext4_ext_in_cache                             [Tracepoint event]
      ext4:ext4_ext_load_extent                          [Tracepoint event]
      ext4:ext4_ext_map_blocks_enter                     [Tracepoint event]
      ext4:ext4_ext_map_blocks_exit                      [Tracepoint event]
      ext4:ext4_ext_put_in_cache                         [Tracepoint event]
      ext4:ext4_ext_remove_space                         [Tracepoint event]
      ext4:ext4_ext_remove_space_done                    [Tracepoint event]
      ext4:ext4_ext_rm_idx                               [Tracepoint event]
      ext4:ext4_ext_rm_leaf                              [Tracepoint event]
      ext4:ext4_ext_show_extent                          [Tracepoint event]
      ext4:ext4_fallocate_enter                          [Tracepoint event]
      ext4:ext4_fallocate_exit                           [Tracepoint event]
      ext4:ext4_find_delalloc_range                      [Tracepoint event]
      ext4:ext4_forget                                   [Tracepoint event]
      ext4:ext4_free_blocks                              [Tracepoint event]
      ext4:ext4_free_inode                               [Tracepoint event]
      ext4:ext4_fsmap_high_key                           [Tracepoint event]
      ext4:ext4_fsmap_low_key                            [Tracepoint event]
      ext4:ext4_fsmap_mapping                            [Tracepoint event]
      ext4:ext4_get_implied_cluster_alloc_exit           [Tracepoint event]
      ext4:ext4_get_reserved_cluster_alloc               [Tracepoint event]
      ext4:ext4_getfsmap_high_key                        [Tracepoint event]
      ext4:ext4_getfsmap_low_key                         [Tracepoint event]
      ext4:ext4_getfsmap_mapping                         [Tracepoint event]
      ext4:ext4_ind_map_blocks_enter                     [Tracepoint event]
      ext4:ext4_ind_map_blocks_exit                      [Tracepoint event]
      ext4:ext4_insert_range                             [Tracepoint event]
      ext4:ext4_invalidatepage                           [Tracepoint event]
      ext4:ext4_journal_start                            [Tracepoint event]
      ext4:ext4_journal_start_reserved                   [Tracepoint event]
      ext4:ext4_journalled_invalidatepage                [Tracepoint event]
      ext4:ext4_journalled_write_end                     [Tracepoint event]
      ext4:ext4_load_inode                               [Tracepoint event]
      ext4:ext4_load_inode_bitmap                        [Tracepoint event]
      ext4:ext4_mark_inode_dirty                         [Tracepoint event]
      ext4:ext4_mb_bitmap_load                           [Tracepoint event]
      ext4:ext4_mb_buddy_bitmap_load                     [Tracepoint event]
      ext4:ext4_mb_discard_preallocations                [Tracepoint event]
      ext4:ext4_mb_new_group_pa                          [Tracepoint event]
      ext4:ext4_mb_new_inode_pa                          [Tracepoint event]
      ext4:ext4_mb_release_group_pa                      [Tracepoint event]
      ext4:ext4_mb_release_inode_pa                      [Tracepoint event]
      ext4:ext4_mballoc_alloc                            [Tracepoint event]
      ext4:ext4_mballoc_discard                          [Tracepoint event]
      ext4:ext4_mballoc_free                             [Tracepoint event]
      ext4:ext4_mballoc_prealloc                         [Tracepoint event]
      ext4:ext4_nfs_commit_metadata                      [Tracepoint event]
      ext4:ext4_other_inode_update_time                  [Tracepoint event]
      ext4:ext4_punch_hole                               [Tracepoint event]
      ext4:ext4_read_block_bitmap_load                   [Tracepoint event]
      ext4:ext4_readpage                                 [Tracepoint event]
      ext4:ext4_releasepage                              [Tracepoint event]
      ext4:ext4_remove_blocks                            [Tracepoint event]
      ext4:ext4_request_blocks                           [Tracepoint event]
      ext4:ext4_request_inode                            [Tracepoint event]
      ext4:ext4_shutdown                                 [Tracepoint event]
      ext4:ext4_sync_file_enter                          [Tracepoint event]
      ext4:ext4_sync_file_exit                           [Tracepoint event]
      ext4:ext4_sync_fs                                  [Tracepoint event]
      ext4:ext4_trim_all_free                            [Tracepoint event]
      ext4:ext4_trim_extent                              [Tracepoint event]
      ext4:ext4_truncate_enter                           [Tracepoint event]
      ext4:ext4_truncate_exit                            [Tracepoint event]
      ext4:ext4_unlink_enter                             [Tracepoint event]
      ext4:ext4_unlink_exit                              [Tracepoint event]
      ext4:ext4_write_begin                              [Tracepoint event]
      ext4:ext4_write_end                                [Tracepoint event]
      ext4:ext4_writepage                                [Tracepoint event]
      ext4:ext4_writepages                               [Tracepoint event]
      ext4:ext4_writepages_result                        [Tracepoint event]
      ext4:ext4_zero_range                               [Tracepoint event]
      fib6:fib6_table_lookup                             [Tracepoint event]
      fib:fib_table_lookup                               [Tracepoint event]
      filelock:break_lease_block                         [Tracepoint event]
      filelock:break_lease_noblock                       [Tracepoint event]
      filelock:break_lease_unblock                       [Tracepoint event]
      filelock:fcntl_setlk                               [Tracepoint event]
      filelock:flock_lock_inode                          [Tracepoint event]
      filelock:generic_add_lease                         [Tracepoint event]
      filelock:generic_delete_lease                      [Tracepoint event]
      filelock:leases_conflict                           [Tracepoint event]
      filelock:locks_get_lock_context                    [Tracepoint event]
      filelock:locks_remove_posix                        [Tracepoint event]
      filelock:posix_lock_inode                          [Tracepoint event]
      filelock:time_out_leases                           [Tracepoint event]
      filemap:file_check_and_advance_wb_err              [Tracepoint event]
      filemap:filemap_set_wb_err                         [Tracepoint event]
      filemap:mm_filemap_add_to_page_cache               [Tracepoint event]
      filemap:mm_filemap_delete_from_page_cache          [Tracepoint event]
      fs:do_sys_open                                     [Tracepoint event]
      fs:open_exec                                       [Tracepoint event]
      fs_dax:dax_insert_mapping                          [Tracepoint event]
      fs_dax:dax_insert_pfn_mkwrite                      [Tracepoint event]
      fs_dax:dax_insert_pfn_mkwrite_no_entry             [Tracepoint event]
      fs_dax:dax_load_hole                               [Tracepoint event]
      fs_dax:dax_pmd_fault                               [Tracepoint event]
      fs_dax:dax_pmd_fault_done                          [Tracepoint event]
      fs_dax:dax_pmd_insert_mapping                      [Tracepoint event]
      fs_dax:dax_pmd_load_hole                           [Tracepoint event]
      fs_dax:dax_pmd_load_hole_fallback                  [Tracepoint event]
      fs_dax:dax_pte_fault                               [Tracepoint event]
      fs_dax:dax_pte_fault_done                          [Tracepoint event]
      fs_dax:dax_writeback_one                           [Tracepoint event]
      fs_dax:dax_writeback_range                         [Tracepoint event]
      fs_dax:dax_writeback_range_done                    [Tracepoint event]
      ftrace:function                                    [Tracepoint event]
      ftrace:print                                       [Tracepoint event]
      gpio:gpio_direction                                [Tracepoint event]
      gpio:gpio_value                                    [Tracepoint event]
      hda:hda_get_response                               [Tracepoint event]
      hda:hda_send_cmd                                   [Tracepoint event]
      hda:hda_unsol_event                                [Tracepoint event]
      hda:snd_hdac_stream_start                          [Tracepoint event]
      hda:snd_hdac_stream_stop                           [Tracepoint event]
      hda_controller:azx_get_position                    [Tracepoint event]
      hda_controller:azx_pcm_close                       [Tracepoint event]
      hda_controller:azx_pcm_hw_params                   [Tracepoint event]
      hda_controller:azx_pcm_open                        [Tracepoint event]
      hda_controller:azx_pcm_prepare                     [Tracepoint event]
      hda_controller:azx_pcm_trigger                     [Tracepoint event]
      hda_intel:azx_resume                               [Tracepoint event]
      hda_intel:azx_runtime_resume                       [Tracepoint event]
      hda_intel:azx_runtime_suspend                      [Tracepoint event]
      hda_intel:azx_suspend                              [Tracepoint event]
      huge_memory:mm_collapse_huge_page                  [Tracepoint event]
      huge_memory:mm_collapse_huge_page_isolate          [Tracepoint event]
      huge_memory:mm_collapse_huge_page_swapin           [Tracepoint event]
      huge_memory:mm_khugepaged_scan_pmd                 [Tracepoint event]
      hwmon:hwmon_attr_show                              [Tracepoint event]
      hwmon:hwmon_attr_show_string                       [Tracepoint event]
      hwmon:hwmon_attr_store                             [Tracepoint event]
      hyperv:hyperv_mmu_flush_tlb_others                 [Tracepoint event]
      hyperv:hyperv_nested_flush_guest_mapping           [Tracepoint event]
      hyperv:hyperv_nested_flush_guest_mapping_range     [Tracepoint event]
      hyperv:hyperv_send_ipi_mask                        [Tracepoint event]
      i2c:i2c_read                                       [Tracepoint event]
      i2c:i2c_reply                                      [Tracepoint event]
      i2c:i2c_result                                     [Tracepoint event]
      i2c:i2c_write                                      [Tracepoint event]
      initcall:initcall_finish                           [Tracepoint event]
      initcall:initcall_level                            [Tracepoint event]
      initcall:initcall_start                            [Tracepoint event]
      intel_iommu:bounce_map_single                      [Tracepoint event]
      intel_iommu:bounce_unmap_single                    [Tracepoint event]
      intel_iommu:map_sg                                 [Tracepoint event]
      intel_iommu:map_single                             [Tracepoint event]
      intel_iommu:unmap_sg                               [Tracepoint event]
      intel_iommu:unmap_single                           [Tracepoint event]
      iocost:iocost_inuse_giveaway                       [Tracepoint event]
      iocost:iocost_inuse_reset                          [Tracepoint event]
      iocost:iocost_inuse_takeback                       [Tracepoint event]
      iocost:iocost_ioc_vrate_adj                        [Tracepoint event]
      iocost:iocost_iocg_activate                        [Tracepoint event]
      iommu:add_device_to_group                          [Tracepoint event]
      iommu:attach_device_to_domain                      [Tracepoint event]
      iommu:detach_device_from_domain                    [Tracepoint event]
      iommu:io_page_fault                                [Tracepoint event]
      iommu:map                                          [Tracepoint event]
      iommu:remove_device_from_group                     [Tracepoint event]
      iommu:unmap                                        [Tracepoint event]
      irq:irq_handler_entry                              [Tracepoint event]
      irq:irq_handler_exit                               [Tracepoint event]
      irq:softirq_entry                                  [Tracepoint event]
      irq:softirq_exit                                   [Tracepoint event]
      irq:softirq_raise                                  [Tracepoint event]
      irq_matrix:irq_matrix_alloc                        [Tracepoint event]
      irq_matrix:irq_matrix_alloc_managed                [Tracepoint event]
      irq_matrix:irq_matrix_alloc_reserved               [Tracepoint event]
      irq_matrix:irq_matrix_assign                       [Tracepoint event]
      irq_matrix:irq_matrix_assign_system                [Tracepoint event]
      irq_matrix:irq_matrix_free                         [Tracepoint event]
      irq_matrix:irq_matrix_offline                      [Tracepoint event]
      irq_matrix:irq_matrix_online                       [Tracepoint event]
      irq_matrix:irq_matrix_remove_managed               [Tracepoint event]
      irq_matrix:irq_matrix_remove_reserved              [Tracepoint event]
      irq_matrix:irq_matrix_reserve                      [Tracepoint event]
      irq_matrix:irq_matrix_reserve_managed              [Tracepoint event]
      irq_vectors:call_function_entry                    [Tracepoint event]
      irq_vectors:call_function_exit                     [Tracepoint event]
      irq_vectors:call_function_single_entry             [Tracepoint event]
      irq_vectors:call_function_single_exit              [Tracepoint event]
      irq_vectors:deferred_error_apic_entry              [Tracepoint event]
      irq_vectors:deferred_error_apic_exit               [Tracepoint event]
      irq_vectors:error_apic_entry                       [Tracepoint event]
      irq_vectors:error_apic_exit                        [Tracepoint event]
      irq_vectors:irq_work_entry                         [Tracepoint event]
      irq_vectors:irq_work_exit                          [Tracepoint event]
      irq_vectors:local_timer_entry                      [Tracepoint event]
      irq_vectors:local_timer_exit                       [Tracepoint event]
      irq_vectors:reschedule_entry                       [Tracepoint event]
      irq_vectors:reschedule_exit                        [Tracepoint event]
      irq_vectors:spurious_apic_entry                    [Tracepoint event]
      irq_vectors:spurious_apic_exit                     [Tracepoint event]
      irq_vectors:thermal_apic_entry                     [Tracepoint event]
      irq_vectors:thermal_apic_exit                      [Tracepoint event]
      irq_vectors:threshold_apic_entry                   [Tracepoint event]
      irq_vectors:threshold_apic_exit                    [Tracepoint event]
      irq_vectors:vector_activate                        [Tracepoint event]
      irq_vectors:vector_alloc                           [Tracepoint event]
      irq_vectors:vector_alloc_managed                   [Tracepoint event]
      irq_vectors:vector_clear                           [Tracepoint event]
      irq_vectors:vector_config                          [Tracepoint event]
      irq_vectors:vector_deactivate                      [Tracepoint event]
      irq_vectors:vector_free_moved                      [Tracepoint event]
      irq_vectors:vector_reserve                         [Tracepoint event]
      irq_vectors:vector_reserve_managed                 [Tracepoint event]
      irq_vectors:vector_setup                           [Tracepoint event]
      irq_vectors:vector_teardown                        [Tracepoint event]
      irq_vectors:vector_update                          [Tracepoint event]
      irq_vectors:x86_platform_ipi_entry                 [Tracepoint event]
      irq_vectors:x86_platform_ipi_exit                  [Tracepoint event]
      jbd2:jbd2_checkpoint                               [Tracepoint event]
      jbd2:jbd2_checkpoint_stats                         [Tracepoint event]
      jbd2:jbd2_commit_flushing                          [Tracepoint event]
      jbd2:jbd2_commit_locking                           [Tracepoint event]
      jbd2:jbd2_commit_logging                           [Tracepoint event]
      jbd2:jbd2_drop_transaction                         [Tracepoint event]
      jbd2:jbd2_end_commit                               [Tracepoint event]
      jbd2:jbd2_handle_extend                            [Tracepoint event]
      jbd2:jbd2_handle_start                             [Tracepoint event]
      jbd2:jbd2_handle_stats                             [Tracepoint event]
      jbd2:jbd2_lock_buffer_stall                        [Tracepoint event]
      jbd2:jbd2_run_stats                                [Tracepoint event]
      jbd2:jbd2_start_commit                             [Tracepoint event]
      jbd2:jbd2_submit_inode_data                        [Tracepoint event]
      jbd2:jbd2_update_log_tail                          [Tracepoint event]
      jbd2:jbd2_write_superblock                         [Tracepoint event]
      kmem:kfree                                         [Tracepoint event]
      kmem:kmalloc                                       [Tracepoint event]
      kmem:kmalloc_node                                  [Tracepoint event]
      kmem:kmem_cache_alloc                              [Tracepoint event]
      kmem:kmem_cache_alloc_node                         [Tracepoint event]
      kmem:kmem_cache_free                               [Tracepoint event]
      kmem:mm_page_alloc                                 [Tracepoint event]
      kmem:mm_page_alloc_extfrag                         [Tracepoint event]
      kmem:mm_page_alloc_zone_locked                     [Tracepoint event]
      kmem:mm_page_free                                  [Tracepoint event]
      kmem:mm_page_free_batched                          [Tracepoint event]
      kmem:mm_page_pcpu_drain                            [Tracepoint event]
      kvm:kvm_ack_irq                                    [Tracepoint event]
      kvm:kvm_age_page                                   [Tracepoint event]
      kvm:kvm_apic                                       [Tracepoint event]
      kvm:kvm_apic_accept_irq                            [Tracepoint event]
      kvm:kvm_apic_ipi                                   [Tracepoint event]
      kvm:kvm_async_pf_completed                         [Tracepoint event]
      kvm:kvm_async_pf_doublefault                       [Tracepoint event]
      kvm:kvm_async_pf_not_present                       [Tracepoint event]
      kvm:kvm_async_pf_ready                             [Tracepoint event]
      kvm:kvm_avic_incomplete_ipi                        [Tracepoint event]
      kvm:kvm_avic_unaccelerated_access                  [Tracepoint event]
      kvm:kvm_cpuid                                      [Tracepoint event]
      kvm:kvm_cr                                         [Tracepoint event]
      kvm:kvm_emulate_insn                               [Tracepoint event]
      kvm:kvm_enter_smm                                  [Tracepoint event]
      kvm:kvm_entry                                      [Tracepoint event]
      kvm:kvm_eoi                                        [Tracepoint event]
      kvm:kvm_exit                                       [Tracepoint event]
      kvm:kvm_fast_mmio                                  [Tracepoint event]
      kvm:kvm_fpu                                        [Tracepoint event]
      kvm:kvm_halt_poll_ns                               [Tracepoint event]
      kvm:kvm_hv_flush_tlb                               [Tracepoint event]
      kvm:kvm_hv_flush_tlb_ex                            [Tracepoint event]
      kvm:kvm_hv_hypercall                               [Tracepoint event]
      kvm:kvm_hv_notify_acked_sint                       [Tracepoint event]
      kvm:kvm_hv_send_ipi                                [Tracepoint event]
      kvm:kvm_hv_send_ipi_ex                             [Tracepoint event]
      kvm:kvm_hv_stimer_callback                         [Tracepoint event]
      kvm:kvm_hv_stimer_cleanup                          [Tracepoint event]
      kvm:kvm_hv_stimer_expiration                       [Tracepoint event]
      kvm:kvm_hv_stimer_set_config                       [Tracepoint event]
      kvm:kvm_hv_stimer_set_count                        [Tracepoint event]
      kvm:kvm_hv_stimer_start_one_shot                   [Tracepoint event]
      kvm:kvm_hv_stimer_start_periodic                   [Tracepoint event]
      kvm:kvm_hv_synic_send_eoi                          [Tracepoint event]
      kvm:kvm_hv_synic_set_irq                           [Tracepoint event]
      kvm:kvm_hv_synic_set_msr                           [Tracepoint event]
      kvm:kvm_hv_timer_state                             [Tracepoint event]
      kvm:kvm_hypercall                                  [Tracepoint event]
      kvm:kvm_inj_exception                              [Tracepoint event]
      kvm:kvm_inj_virq                                   [Tracepoint event]
      kvm:kvm_invlpga                                    [Tracepoint event]
      kvm:kvm_ioapic_delayed_eoi_inj                     [Tracepoint event]
      kvm:kvm_ioapic_set_irq                             [Tracepoint event]
      kvm:kvm_mmio                                       [Tracepoint event]
      kvm:kvm_msi_set_irq                                [Tracepoint event]
      kvm:kvm_msr                                        [Tracepoint event]
      kvm:kvm_nested_intercepts                          [Tracepoint event]
      kvm:kvm_nested_intr_vmexit                         [Tracepoint event]
      kvm:kvm_nested_vmenter_failed                      [Tracepoint event]
      kvm:kvm_nested_vmexit                              [Tracepoint event]
      kvm:kvm_nested_vmexit_inject                       [Tracepoint event]
      kvm:kvm_nested_vmrun                               [Tracepoint event]
      kvm:kvm_page_fault                                 [Tracepoint event]
      kvm:kvm_pi_irte_update                             [Tracepoint event]
      kvm:kvm_pic_set_irq                                [Tracepoint event]
      kvm:kvm_pio                                        [Tracepoint event]
      kvm:kvm_ple_window_update                          [Tracepoint event]
      kvm:kvm_pml_full                                   [Tracepoint event]
      kvm:kvm_pv_eoi                                     [Tracepoint event]
      kvm:kvm_pv_tlb_flush                               [Tracepoint event]
      kvm:kvm_pvclock_update                             [Tracepoint event]
      kvm:kvm_set_irq                                    [Tracepoint event]
      kvm:kvm_skinit                                     [Tracepoint event]
      kvm:kvm_track_tsc                                  [Tracepoint event]
      kvm:kvm_try_async_get_page                         [Tracepoint event]
      kvm:kvm_update_master_clock                        [Tracepoint event]
      kvm:kvm_userspace_exit                             [Tracepoint event]
      kvm:kvm_vcpu_wakeup                                [Tracepoint event]
      kvm:kvm_wait_lapic_expire                          [Tracepoint event]
      kvm:kvm_write_tsc_offset                           [Tracepoint event]
      kvm:vcpu_match_mmio                                [Tracepoint event]
      kvmmmu:check_mmio_spte                             [Tracepoint event]
      kvmmmu:fast_page_fault                             [Tracepoint event]
      kvmmmu:handle_mmio_page_fault                      [Tracepoint event]
      kvmmmu:kvm_mmu_get_page                            [Tracepoint event]
      kvmmmu:kvm_mmu_pagetable_walk                      [Tracepoint event]
      kvmmmu:kvm_mmu_paging_element                      [Tracepoint event]
      kvmmmu:kvm_mmu_prepare_zap_page                    [Tracepoint event]
      kvmmmu:kvm_mmu_set_accessed_bit                    [Tracepoint event]
      kvmmmu:kvm_mmu_set_dirty_bit                       [Tracepoint event]
      kvmmmu:kvm_mmu_set_spte                            [Tracepoint event]
      kvmmmu:kvm_mmu_spte_requested                      [Tracepoint event]
      kvmmmu:kvm_mmu_sync_page                           [Tracepoint event]
      kvmmmu:kvm_mmu_unsync_page                         [Tracepoint event]
      kvmmmu:kvm_mmu_walker_error                        [Tracepoint event]
      kvmmmu:kvm_mmu_zap_all_fast                        [Tracepoint event]
      kvmmmu:mark_mmio_spte                              [Tracepoint event]
      libata:ata_eh_link_autopsy                         [Tracepoint event]
      libata:ata_eh_link_autopsy_qc                      [Tracepoint event]
      libata:ata_qc_complete_done                        [Tracepoint event]
      libata:ata_qc_complete_failed                      [Tracepoint event]
      libata:ata_qc_complete_internal                    [Tracepoint event]
      libata:ata_qc_issue                                [Tracepoint event]
      mce:mce_record                                     [Tracepoint event]
      mdio:mdio_access                                   [Tracepoint event]
      mei:mei_pci_cfg_read                               [Tracepoint event]
      mei:mei_reg_read                                   [Tracepoint event]
      mei:mei_reg_write                                  [Tracepoint event]
      migrate:mm_migrate_pages                           [Tracepoint event]
      mmc:mmc_request_done                               [Tracepoint event]
      mmc:mmc_request_start                              [Tracepoint event]
      module:module_free                                 [Tracepoint event]
      module:module_get                                  [Tracepoint event]
      module:module_load                                 [Tracepoint event]
      module:module_put                                  [Tracepoint event]
      module:module_request                              [Tracepoint event]
      mpx:bounds_exception_mpx                           [Tracepoint event]
      mpx:mpx_bounds_register_exception                  [Tracepoint event]
      mpx:mpx_new_bounds_table                           [Tracepoint event]
      mpx:mpx_unmap_search                               [Tracepoint event]
      mpx:mpx_unmap_zap                                  [Tracepoint event]
      msr:rdpmc                                          [Tracepoint event]
      msr:read_msr                                       [Tracepoint event]
      msr:write_msr                                      [Tracepoint event]
      napi:napi_poll                                     [Tracepoint event]
      neigh:neigh_cleanup_and_release                    [Tracepoint event]
      neigh:neigh_create                                 [Tracepoint event]
      neigh:neigh_event_send_dead                        [Tracepoint event]
      neigh:neigh_event_send_done                        [Tracepoint event]
      neigh:neigh_timer_handler                          [Tracepoint event]
      neigh:neigh_update                                 [Tracepoint event]
      neigh:neigh_update_done                            [Tracepoint event]
      net:napi_gro_frags_entry                           [Tracepoint event]
      net:napi_gro_frags_exit                            [Tracepoint event]
      net:napi_gro_receive_entry                         [Tracepoint event]
      net:napi_gro_receive_exit                          [Tracepoint event]
      net:net_dev_queue                                  [Tracepoint event]
      net:net_dev_start_xmit                             [Tracepoint event]
      net:net_dev_xmit                                   [Tracepoint event]
      net:net_dev_xmit_timeout                           [Tracepoint event]
      net:netif_receive_skb                              [Tracepoint event]
      net:netif_receive_skb_entry                        [Tracepoint event]
      net:netif_receive_skb_exit                         [Tracepoint event]
      net:netif_receive_skb_list_entry                   [Tracepoint event]
      net:netif_receive_skb_list_exit                    [Tracepoint event]
      net:netif_rx                                       [Tracepoint event]
      net:netif_rx_entry                                 [Tracepoint event]
      net:netif_rx_exit                                  [Tracepoint event]
      net:netif_rx_ni_entry                              [Tracepoint event]
      net:netif_rx_ni_exit                               [Tracepoint event]
      nmi:nmi_handler                                    [Tracepoint event]
      oom:compact_retry                                  [Tracepoint event]
      oom:finish_task_reaping                            [Tracepoint event]
      oom:mark_victim                                    [Tracepoint event]
      oom:oom_score_adj_update                           [Tracepoint event]
      oom:reclaim_retry_zone                             [Tracepoint event]
      oom:skip_task_reaping                              [Tracepoint event]
      oom:start_task_reaping                             [Tracepoint event]
      oom:wake_reaper                                    [Tracepoint event]
      page_isolation:test_pages_isolated                 [Tracepoint event]
      page_pool:page_pool_inflight                       [Tracepoint event]
      page_pool:page_pool_state_hold                     [Tracepoint event]
      page_pool:page_pool_state_release                  [Tracepoint event]
      pagemap:mm_lru_activate                            [Tracepoint event]
      pagemap:mm_lru_insertion                           [Tracepoint event]
      percpu:percpu_alloc_percpu                         [Tracepoint event]
      percpu:percpu_alloc_percpu_fail                    [Tracepoint event]
      percpu:percpu_create_chunk                         [Tracepoint event]
      percpu:percpu_destroy_chunk                        [Tracepoint event]
      percpu:percpu_free_percpu                          [Tracepoint event]
      power:clock_disable                                [Tracepoint event]
      power:clock_enable                                 [Tracepoint event]
      power:clock_set_rate                               [Tracepoint event]
      power:cpu_frequency                                [Tracepoint event]
      power:cpu_frequency_limits                         [Tracepoint event]
      power:cpu_idle                                     [Tracepoint event]
      power:dev_pm_qos_add_request                       [Tracepoint event]
      power:dev_pm_qos_remove_request                    [Tracepoint event]
      power:dev_pm_qos_update_request                    [Tracepoint event]
      power:device_pm_callback_end                       [Tracepoint event]
      power:device_pm_callback_start                     [Tracepoint event]
      power:pm_qos_add_request                           [Tracepoint event]
      power:pm_qos_remove_request                        [Tracepoint event]
      power:pm_qos_update_flags                          [Tracepoint event]
      power:pm_qos_update_request                        [Tracepoint event]
      power:pm_qos_update_request_timeout                [Tracepoint event]
      power:pm_qos_update_target                         [Tracepoint event]
      power:power_domain_target                          [Tracepoint event]
      power:powernv_throttle                             [Tracepoint event]
      power:pstate_sample                                [Tracepoint event]
      power:suspend_resume                               [Tracepoint event]
      power:wakeup_source_activate                       [Tracepoint event]
      power:wakeup_source_deactivate                     [Tracepoint event]
      printk:console                                     [Tracepoint event]
      qdisc:qdisc_dequeue                                [Tracepoint event]
      random:add_device_randomness                       [Tracepoint event]
      random:add_disk_randomness                         [Tracepoint event]
      random:add_input_randomness                        [Tracepoint event]
      random:credit_entropy_bits                         [Tracepoint event]
      random:debit_entropy                               [Tracepoint event]
      random:extract_entropy                             [Tracepoint event]
      random:extract_entropy_user                        [Tracepoint event]
      random:get_random_bytes                            [Tracepoint event]
      random:get_random_bytes_arch                       [Tracepoint event]
      random:mix_pool_bytes                              [Tracepoint event]
      random:mix_pool_bytes_nolock                       [Tracepoint event]
      random:push_to_pool                                [Tracepoint event]
      random:random_read                                 [Tracepoint event]
      random:urandom_read                                [Tracepoint event]
      random:xfer_secondary_pool                         [Tracepoint event]
      ras:aer_event                                      [Tracepoint event]
      ras:arm_event                                      [Tracepoint event]
      ras:extlog_mem_event                               [Tracepoint event]
      ras:mc_event                                       [Tracepoint event]
      ras:memory_failure_event                           [Tracepoint event]
      ras:non_standard_event                             [Tracepoint event]
      raw_syscalls:sys_enter                             [Tracepoint event]
      raw_syscalls:sys_exit                              [Tracepoint event]
      rcu:rcu_utilization                                [Tracepoint event]
      regmap:regcache_drop_region                        [Tracepoint event]
      regmap:regcache_sync                               [Tracepoint event]
      regmap:regmap_async_complete_done                  [Tracepoint event]
      regmap:regmap_async_complete_start                 [Tracepoint event]
      regmap:regmap_async_io_complete                    [Tracepoint event]
      regmap:regmap_async_write_start                    [Tracepoint event]
      regmap:regmap_cache_bypass                         [Tracepoint event]
      regmap:regmap_cache_only                           [Tracepoint event]
      regmap:regmap_hw_read_done                         [Tracepoint event]
      regmap:regmap_hw_read_start                        [Tracepoint event]
      regmap:regmap_hw_write_done                        [Tracepoint event]
      regmap:regmap_hw_write_start                       [Tracepoint event]
      regmap:regmap_reg_read                             [Tracepoint event]
      regmap:regmap_reg_read_cache                       [Tracepoint event]
      regmap:regmap_reg_write                            [Tracepoint event]
      regulator:regulator_disable                        [Tracepoint event]
      regulator:regulator_disable_complete               [Tracepoint event]
      regulator:regulator_enable                         [Tracepoint event]
      regulator:regulator_enable_complete                [Tracepoint event]
      regulator:regulator_enable_delay                   [Tracepoint event]
      regulator:regulator_set_voltage                    [Tracepoint event]
      regulator:regulator_set_voltage_complete           [Tracepoint event]
      resctrl:pseudo_lock_l2                             [Tracepoint event]
      resctrl:pseudo_lock_l3                             [Tracepoint event]
      resctrl:pseudo_lock_mem_latency                    [Tracepoint event]
      rpm:rpm_idle                                       [Tracepoint event]
      rpm:rpm_resume                                     [Tracepoint event]
      rpm:rpm_return_int                                 [Tracepoint event]
      rpm:rpm_suspend                                    [Tracepoint event]
      rseq:rseq_ip_fixup                                 [Tracepoint event]
      rseq:rseq_update                                   [Tracepoint event]
      rtc:rtc_alarm_irq_enable                           [Tracepoint event]
      rtc:rtc_irq_set_freq                               [Tracepoint event]
      rtc:rtc_irq_set_state                              [Tracepoint event]
      rtc:rtc_read_alarm                                 [Tracepoint event]
      rtc:rtc_read_offset                                [Tracepoint event]
      rtc:rtc_read_time                                  [Tracepoint event]
      rtc:rtc_set_alarm                                  [Tracepoint event]
      rtc:rtc_set_offset                                 [Tracepoint event]
      rtc:rtc_set_time                                   [Tracepoint event]
      rtc:rtc_timer_dequeue                              [Tracepoint event]
      rtc:rtc_timer_enqueue                              [Tracepoint event]
      rtc:rtc_timer_fired                                [Tracepoint event]
      sched:sched_kthread_stop                           [Tracepoint event]
      sched:sched_kthread_stop_ret                       [Tracepoint event]
      sched:sched_migrate_task                           [Tracepoint event]
      sched:sched_move_numa                              [Tracepoint event]
      sched:sched_pi_setprio                             [Tracepoint event]
      sched:sched_process_exec                           [Tracepoint event]
      sched:sched_process_exit                           [Tracepoint event]
      sched:sched_process_fork                           [Tracepoint event]
      sched:sched_process_free                           [Tracepoint event]
      sched:sched_process_hang                           [Tracepoint event]
      sched:sched_process_wait                           [Tracepoint event]
      sched:sched_stat_blocked                           [Tracepoint event]
      sched:sched_stat_iowait                            [Tracepoint event]
      sched:sched_stat_runtime                           [Tracepoint event]
      sched:sched_stat_sleep                             [Tracepoint event]
      sched:sched_stat_wait                              [Tracepoint event]
      sched:sched_stick_numa                             [Tracepoint event]
      sched:sched_swap_numa                              [Tracepoint event]
      sched:sched_switch                                 [Tracepoint event]
      sched:sched_wait_task                              [Tracepoint event]
      sched:sched_wake_idle_without_ipi                  [Tracepoint event]
      sched:sched_wakeup                                 [Tracepoint event]
      sched:sched_wakeup_new                             [Tracepoint event]
      sched:sched_waking                                 [Tracepoint event]
      scsi:scsi_dispatch_cmd_done                        [Tracepoint event]
      scsi:scsi_dispatch_cmd_error                       [Tracepoint event]
      scsi:scsi_dispatch_cmd_start                       [Tracepoint event]
      scsi:scsi_dispatch_cmd_timeout                     [Tracepoint event]
      scsi:scsi_eh_wakeup                                [Tracepoint event]
      signal:signal_deliver                              [Tracepoint event]
      signal:signal_generate                             [Tracepoint event]
      skb:consume_skb                                    [Tracepoint event]
      skb:kfree_skb                                      [Tracepoint event]
      skb:skb_copy_datagram_iovec                        [Tracepoint event]
      smbus:smbus_read                                   [Tracepoint event]
      smbus:smbus_reply                                  [Tracepoint event]
      smbus:smbus_result                                 [Tracepoint event]
      smbus:smbus_write                                  [Tracepoint event]
      sock:inet_sock_set_state                           [Tracepoint event]
      sock:sock_exceed_buf_limit                         [Tracepoint event]
      sock:sock_rcvqueue_full                            [Tracepoint event]
      spi:spi_controller_busy                            [Tracepoint event]
      spi:spi_controller_idle                            [Tracepoint event]
      spi:spi_message_done                               [Tracepoint event]
      spi:spi_message_start                              [Tracepoint event]
      spi:spi_message_submit                             [Tracepoint event]
      spi:spi_transfer_start                             [Tracepoint event]
      spi:spi_transfer_stop                              [Tracepoint event]
      sunrpc:rpc__auth_tooweak                           [Tracepoint event]
      sunrpc:rpc__bad_creds                              [Tracepoint event]
      sunrpc:rpc__garbage_args                           [Tracepoint event]
      sunrpc:rpc__mismatch                               [Tracepoint event]
      sunrpc:rpc__proc_unavail                           [Tracepoint event]
      sunrpc:rpc__prog_mismatch                          [Tracepoint event]
      sunrpc:rpc__prog_unavail                           [Tracepoint event]
      sunrpc:rpc__stale_creds                            [Tracepoint event]
      sunrpc:rpc__unparsable                             [Tracepoint event]
      sunrpc:rpc_bad_callhdr                             [Tracepoint event]
      sunrpc:rpc_bad_verifier                            [Tracepoint event]
      sunrpc:rpc_bind_status                             [Tracepoint event]
      sunrpc:rpc_call_status                             [Tracepoint event]
      sunrpc:rpc_connect_status                          [Tracepoint event]
      sunrpc:rpc_reply_pages                             [Tracepoint event]
      sunrpc:rpc_request                                 [Tracepoint event]
      sunrpc:rpc_socket_close                            [Tracepoint event]
      sunrpc:rpc_socket_connect                          [Tracepoint event]
      sunrpc:rpc_socket_error                            [Tracepoint event]
      sunrpc:rpc_socket_reset_connection                 [Tracepoint event]
      sunrpc:rpc_socket_shutdown                         [Tracepoint event]
      sunrpc:rpc_socket_state_change                     [Tracepoint event]
      sunrpc:rpc_stats_latency                           [Tracepoint event]
      sunrpc:rpc_task_begin                              [Tracepoint event]
      sunrpc:rpc_task_complete                           [Tracepoint event]
      sunrpc:rpc_task_end                                [Tracepoint event]
      sunrpc:rpc_task_run_action                         [Tracepoint event]
      sunrpc:rpc_task_sleep                              [Tracepoint event]
      sunrpc:rpc_task_wakeup                             [Tracepoint event]
      sunrpc:rpc_xdr_alignment                           [Tracepoint event]
      sunrpc:rpc_xdr_overflow                            [Tracepoint event]
      sunrpc:svc_defer                                   [Tracepoint event]
      sunrpc:svc_drop                                    [Tracepoint event]
      sunrpc:svc_drop_deferred                           [Tracepoint event]
      sunrpc:svc_handle_xprt                             [Tracepoint event]
      sunrpc:svc_process                                 [Tracepoint event]
      sunrpc:svc_recv                                    [Tracepoint event]
      sunrpc:svc_revisit_deferred                        [Tracepoint event]
      sunrpc:svc_send                                    [Tracepoint event]
      sunrpc:svc_stats_latency                           [Tracepoint event]
      sunrpc:svc_wake_up                                 [Tracepoint event]
      sunrpc:svc_xprt_dequeue                            [Tracepoint event]
      sunrpc:svc_xprt_do_enqueue                         [Tracepoint event]
      sunrpc:svc_xprt_no_write_space                     [Tracepoint event]
      sunrpc:xprt_complete_rqst                          [Tracepoint event]
      sunrpc:xprt_enq_xmit                               [Tracepoint event]
      sunrpc:xprt_lookup_rqst                            [Tracepoint event]
      sunrpc:xprt_ping                                   [Tracepoint event]
      sunrpc:xprt_timer                                  [Tracepoint event]
      sunrpc:xprt_transmit                               [Tracepoint event]
      sunrpc:xs_stream_read_data                         [Tracepoint event]
      sunrpc:xs_stream_read_request                      [Tracepoint event]
      swiotlb:swiotlb_bounced                            [Tracepoint event]
      sync_trace:sync_timeline                           [Tracepoint event]
      syscalls:sys_enter_accept                          [Tracepoint event]
      syscalls:sys_enter_accept4                         [Tracepoint event]
      syscalls:sys_enter_access                          [Tracepoint event]
      syscalls:sys_enter_acct                            [Tracepoint event]
      syscalls:sys_enter_add_key                         [Tracepoint event]
      syscalls:sys_enter_adjtimex                        [Tracepoint event]
      syscalls:sys_enter_alarm                           [Tracepoint event]
      syscalls:sys_enter_arch_prctl                      [Tracepoint event]
      syscalls:sys_enter_bind                            [Tracepoint event]
      syscalls:sys_enter_bpf                             [Tracepoint event]
      syscalls:sys_enter_brk                             [Tracepoint event]
      syscalls:sys_enter_capget                          [Tracepoint event]
      syscalls:sys_enter_capset                          [Tracepoint event]
      syscalls:sys_enter_chdir                           [Tracepoint event]
      syscalls:sys_enter_chmod                           [Tracepoint event]
      syscalls:sys_enter_chown                           [Tracepoint event]
      syscalls:sys_enter_chroot                          [Tracepoint event]
      syscalls:sys_enter_clock_adjtime                   [Tracepoint event]
      syscalls:sys_enter_clock_getres                    [Tracepoint event]
      syscalls:sys_enter_clock_gettime                   [Tracepoint event]
      syscalls:sys_enter_clock_nanosleep                 [Tracepoint event]
      syscalls:sys_enter_clock_settime                   [Tracepoint event]
      syscalls:sys_enter_clone                           [Tracepoint event]
      syscalls:sys_enter_clone3                          [Tracepoint event]
      syscalls:sys_enter_close                           [Tracepoint event]
      syscalls:sys_enter_connect                         [Tracepoint event]
      syscalls:sys_enter_copy_file_range                 [Tracepoint event]
      syscalls:sys_enter_creat                           [Tracepoint event]
      syscalls:sys_enter_delete_module                   [Tracepoint event]
      syscalls:sys_enter_dup                             [Tracepoint event]
      syscalls:sys_enter_dup2                            [Tracepoint event]
      syscalls:sys_enter_dup3                            [Tracepoint event]
      syscalls:sys_enter_epoll_create                    [Tracepoint event]
      syscalls:sys_enter_epoll_create1                   [Tracepoint event]
      syscalls:sys_enter_epoll_ctl                       [Tracepoint event]
      syscalls:sys_enter_epoll_pwait                     [Tracepoint event]
      syscalls:sys_enter_epoll_wait                      [Tracepoint event]
      syscalls:sys_enter_eventfd                         [Tracepoint event]
      syscalls:sys_enter_eventfd2                        [Tracepoint event]
      syscalls:sys_enter_execve                          [Tracepoint event]
      syscalls:sys_enter_execveat                        [Tracepoint event]
      syscalls:sys_enter_exit                            [Tracepoint event]
      syscalls:sys_enter_exit_group                      [Tracepoint event]
      syscalls:sys_enter_faccessat                       [Tracepoint event]
      syscalls:sys_enter_fadvise64                       [Tracepoint event]
      syscalls:sys_enter_fallocate                       [Tracepoint event]
      syscalls:sys_enter_fanotify_init                   [Tracepoint event]
      syscalls:sys_enter_fanotify_mark                   [Tracepoint event]
      syscalls:sys_enter_fchdir                          [Tracepoint event]
      syscalls:sys_enter_fchmod                          [Tracepoint event]
      syscalls:sys_enter_fchmodat                        [Tracepoint event]
      syscalls:sys_enter_fchown                          [Tracepoint event]
      syscalls:sys_enter_fchownat                        [Tracepoint event]
      syscalls:sys_enter_fcntl                           [Tracepoint event]
      syscalls:sys_enter_fdatasync                       [Tracepoint event]
      syscalls:sys_enter_fgetxattr                       [Tracepoint event]
      syscalls:sys_enter_finit_module                    [Tracepoint event]
      syscalls:sys_enter_flistxattr                      [Tracepoint event]
      syscalls:sys_enter_flock                           [Tracepoint event]
      syscalls:sys_enter_fork                            [Tracepoint event]
      syscalls:sys_enter_fremovexattr                    [Tracepoint event]
      syscalls:sys_enter_fsconfig                        [Tracepoint event]
      syscalls:sys_enter_fsetxattr                       [Tracepoint event]
      syscalls:sys_enter_fsmount                         [Tracepoint event]
      syscalls:sys_enter_fsopen                          [Tracepoint event]
      syscalls:sys_enter_fspick                          [Tracepoint event]
      syscalls:sys_enter_fstatfs                         [Tracepoint event]
      syscalls:sys_enter_fsync                           [Tracepoint event]
      syscalls:sys_enter_ftruncate                       [Tracepoint event]
      syscalls:sys_enter_futex                           [Tracepoint event]
      syscalls:sys_enter_futimesat                       [Tracepoint event]
      syscalls:sys_enter_get_mempolicy                   [Tracepoint event]
      syscalls:sys_enter_get_robust_list                 [Tracepoint event]
      syscalls:sys_enter_getcpu                          [Tracepoint event]
      syscalls:sys_enter_getcwd                          [Tracepoint event]
      syscalls:sys_enter_getdents                        [Tracepoint event]
      syscalls:sys_enter_getdents64                      [Tracepoint event]
      syscalls:sys_enter_getegid                         [Tracepoint event]
      syscalls:sys_enter_geteuid                         [Tracepoint event]
      syscalls:sys_enter_getgid                          [Tracepoint event]
      syscalls:sys_enter_getgroups                       [Tracepoint event]
      syscalls:sys_enter_getitimer                       [Tracepoint event]
      syscalls:sys_enter_getpeername                     [Tracepoint event]
      syscalls:sys_enter_getpgid                         [Tracepoint event]
      syscalls:sys_enter_getpgrp                         [Tracepoint event]
      syscalls:sys_enter_getpid                          [Tracepoint event]
      syscalls:sys_enter_getppid                         [Tracepoint event]
      syscalls:sys_enter_getpriority                     [Tracepoint event]
      syscalls:sys_enter_getrandom                       [Tracepoint event]
      syscalls:sys_enter_getresgid                       [Tracepoint event]
      syscalls:sys_enter_getresuid                       [Tracepoint event]
      syscalls:sys_enter_getrlimit                       [Tracepoint event]
      syscalls:sys_enter_getrusage                       [Tracepoint event]
      syscalls:sys_enter_getsid                          [Tracepoint event]
      syscalls:sys_enter_getsockname                     [Tracepoint event]
      syscalls:sys_enter_getsockopt                      [Tracepoint event]
      syscalls:sys_enter_gettid                          [Tracepoint event]
      syscalls:sys_enter_gettimeofday                    [Tracepoint event]
      syscalls:sys_enter_getuid                          [Tracepoint event]
      syscalls:sys_enter_getxattr                        [Tracepoint event]
      syscalls:sys_enter_init_module                     [Tracepoint event]
      syscalls:sys_enter_inotify_add_watch               [Tracepoint event]
      syscalls:sys_enter_inotify_init                    [Tracepoint event]
      syscalls:sys_enter_inotify_init1                   [Tracepoint event]
      syscalls:sys_enter_inotify_rm_watch                [Tracepoint event]
      syscalls:sys_enter_io_cancel                       [Tracepoint event]
      syscalls:sys_enter_io_destroy                      [Tracepoint event]
      syscalls:sys_enter_io_getevents                    [Tracepoint event]
      syscalls:sys_enter_io_pgetevents                   [Tracepoint event]
      syscalls:sys_enter_io_setup                        [Tracepoint event]
      syscalls:sys_enter_io_submit                       [Tracepoint event]
      syscalls:sys_enter_io_uring_enter                  [Tracepoint event]
      syscalls:sys_enter_io_uring_register               [Tracepoint event]
      syscalls:sys_enter_io_uring_setup                  [Tracepoint event]
      syscalls:sys_enter_ioctl                           [Tracepoint event]
      syscalls:sys_enter_ioperm                          [Tracepoint event]
      syscalls:sys_enter_iopl                            [Tracepoint event]
      syscalls:sys_enter_ioprio_get                      [Tracepoint event]
      syscalls:sys_enter_ioprio_set                      [Tracepoint event]
      syscalls:sys_enter_kcmp                            [Tracepoint event]
      syscalls:sys_enter_kexec_file_load                 [Tracepoint event]
      syscalls:sys_enter_kexec_load                      [Tracepoint event]
      syscalls:sys_enter_keyctl                          [Tracepoint event]
      syscalls:sys_enter_kill                            [Tracepoint event]
      syscalls:sys_enter_lchown                          [Tracepoint event]
      syscalls:sys_enter_lgetxattr                       [Tracepoint event]
      syscalls:sys_enter_link                            [Tracepoint event]
      syscalls:sys_enter_linkat                          [Tracepoint event]
      syscalls:sys_enter_listen                          [Tracepoint event]
      syscalls:sys_enter_listxattr                       [Tracepoint event]
      syscalls:sys_enter_llistxattr                      [Tracepoint event]
      syscalls:sys_enter_lookup_dcookie                  [Tracepoint event]
      syscalls:sys_enter_lremovexattr                    [Tracepoint event]
      syscalls:sys_enter_lseek                           [Tracepoint event]
      syscalls:sys_enter_lsetxattr                       [Tracepoint event]
      syscalls:sys_enter_madvise                         [Tracepoint event]
      syscalls:sys_enter_mbind                           [Tracepoint event]
      syscalls:sys_enter_membarrier                      [Tracepoint event]
      syscalls:sys_enter_memfd_create                    [Tracepoint event]
      syscalls:sys_enter_migrate_pages                   [Tracepoint event]
      syscalls:sys_enter_mincore                         [Tracepoint event]
      syscalls:sys_enter_mkdir                           [Tracepoint event]
      syscalls:sys_enter_mkdirat                         [Tracepoint event]
      syscalls:sys_enter_mknod                           [Tracepoint event]
      syscalls:sys_enter_mknodat                         [Tracepoint event]
      syscalls:sys_enter_mlock                           [Tracepoint event]
      syscalls:sys_enter_mlock2                          [Tracepoint event]
      syscalls:sys_enter_mlockall                        [Tracepoint event]
      syscalls:sys_enter_mmap                            [Tracepoint event]
      syscalls:sys_enter_modify_ldt                      [Tracepoint event]
      syscalls:sys_enter_mount                           [Tracepoint event]
      syscalls:sys_enter_move_mount                      [Tracepoint event]
      syscalls:sys_enter_move_pages                      [Tracepoint event]
      syscalls:sys_enter_mprotect                        [Tracepoint event]
      syscalls:sys_enter_mq_getsetattr                   [Tracepoint event]
      syscalls:sys_enter_mq_notify                       [Tracepoint event]
      syscalls:sys_enter_mq_open                         [Tracepoint event]
      syscalls:sys_enter_mq_timedreceive                 [Tracepoint event]
      syscalls:sys_enter_mq_timedsend                    [Tracepoint event]
      syscalls:sys_enter_mq_unlink                       [Tracepoint event]
      syscalls:sys_enter_mremap                          [Tracepoint event]
      syscalls:sys_enter_msgctl                          [Tracepoint event]
      syscalls:sys_enter_msgget                          [Tracepoint event]
      syscalls:sys_enter_msgrcv                          [Tracepoint event]
      syscalls:sys_enter_msgsnd                          [Tracepoint event]
      syscalls:sys_enter_msync                           [Tracepoint event]
      syscalls:sys_enter_munlock                         [Tracepoint event]
      syscalls:sys_enter_munlockall                      [Tracepoint event]
      syscalls:sys_enter_munmap                          [Tracepoint event]
      syscalls:sys_enter_name_to_handle_at               [Tracepoint event]
      syscalls:sys_enter_nanosleep                       [Tracepoint event]
      syscalls:sys_enter_newfstat                        [Tracepoint event]
      syscalls:sys_enter_newfstatat                      [Tracepoint event]
      syscalls:sys_enter_newlstat                        [Tracepoint event]
      syscalls:sys_enter_newstat                         [Tracepoint event]
      syscalls:sys_enter_newuname                        [Tracepoint event]
      syscalls:sys_enter_open                            [Tracepoint event]
      syscalls:sys_enter_open_by_handle_at               [Tracepoint event]
      syscalls:sys_enter_open_tree                       [Tracepoint event]
      syscalls:sys_enter_openat                          [Tracepoint event]
      syscalls:sys_enter_pause                           [Tracepoint event]
      syscalls:sys_enter_perf_event_open                 [Tracepoint event]
      syscalls:sys_enter_personality                     [Tracepoint event]
      syscalls:sys_enter_pidfd_open                      [Tracepoint event]
      syscalls:sys_enter_pidfd_send_signal               [Tracepoint event]
      syscalls:sys_enter_pipe                            [Tracepoint event]
      syscalls:sys_enter_pipe2                           [Tracepoint event]
      syscalls:sys_enter_pivot_root                      [Tracepoint event]
      syscalls:sys_enter_pkey_alloc                      [Tracepoint event]
      syscalls:sys_enter_pkey_free                       [Tracepoint event]
      syscalls:sys_enter_pkey_mprotect                   [Tracepoint event]
      syscalls:sys_enter_poll                            [Tracepoint event]
      syscalls:sys_enter_ppoll                           [Tracepoint event]
      syscalls:sys_enter_prctl                           [Tracepoint event]
      syscalls:sys_enter_pread64                         [Tracepoint event]
      syscalls:sys_enter_preadv                          [Tracepoint event]
      syscalls:sys_enter_preadv2                         [Tracepoint event]
      syscalls:sys_enter_prlimit64                       [Tracepoint event]
      syscalls:sys_enter_process_vm_readv                [Tracepoint event]
      syscalls:sys_enter_process_vm_writev               [Tracepoint event]
      syscalls:sys_enter_pselect6                        [Tracepoint event]
      syscalls:sys_enter_ptrace                          [Tracepoint event]
      syscalls:sys_enter_pwrite64                        [Tracepoint event]
      syscalls:sys_enter_pwritev                         [Tracepoint event]
      syscalls:sys_enter_pwritev2                        [Tracepoint event]
      syscalls:sys_enter_quotactl                        [Tracepoint event]
      syscalls:sys_enter_read                            [Tracepoint event]
      syscalls:sys_enter_readahead                       [Tracepoint event]
      syscalls:sys_enter_readlink                        [Tracepoint event]
      syscalls:sys_enter_readlinkat                      [Tracepoint event]
      syscalls:sys_enter_readv                           [Tracepoint event]
      syscalls:sys_enter_reboot                          [Tracepoint event]
      syscalls:sys_enter_recvfrom                        [Tracepoint event]
      syscalls:sys_enter_recvmmsg                        [Tracepoint event]
      syscalls:sys_enter_recvmsg                         [Tracepoint event]
      syscalls:sys_enter_remap_file_pages                [Tracepoint event]
      syscalls:sys_enter_removexattr                     [Tracepoint event]
      syscalls:sys_enter_rename                          [Tracepoint event]
      syscalls:sys_enter_renameat                        [Tracepoint event]
      syscalls:sys_enter_renameat2                       [Tracepoint event]
      syscalls:sys_enter_request_key                     [Tracepoint event]
      syscalls:sys_enter_restart_syscall                 [Tracepoint event]
      syscalls:sys_enter_rmdir                           [Tracepoint event]
      syscalls:sys_enter_rseq                            [Tracepoint event]
      syscalls:sys_enter_rt_sigaction                    [Tracepoint event]
      syscalls:sys_enter_rt_sigpending                   [Tracepoint event]
      syscalls:sys_enter_rt_sigprocmask                  [Tracepoint event]
      syscalls:sys_enter_rt_sigqueueinfo                 [Tracepoint event]
      syscalls:sys_enter_rt_sigreturn                    [Tracepoint event]
      syscalls:sys_enter_rt_sigsuspend                   [Tracepoint event]
      syscalls:sys_enter_rt_sigtimedwait                 [Tracepoint event]
      syscalls:sys_enter_rt_tgsigqueueinfo               [Tracepoint event]
      syscalls:sys_enter_sched_get_priority_max          [Tracepoint event]
      syscalls:sys_enter_sched_get_priority_min          [Tracepoint event]
      syscalls:sys_enter_sched_getaffinity               [Tracepoint event]
      syscalls:sys_enter_sched_getattr                   [Tracepoint event]
      syscalls:sys_enter_sched_getparam                  [Tracepoint event]
      syscalls:sys_enter_sched_getscheduler              [Tracepoint event]
      syscalls:sys_enter_sched_rr_get_interval           [Tracepoint event]
      syscalls:sys_enter_sched_setaffinity               [Tracepoint event]
      syscalls:sys_enter_sched_setattr                   [Tracepoint event]
      syscalls:sys_enter_sched_setparam                  [Tracepoint event]
      syscalls:sys_enter_sched_setscheduler              [Tracepoint event]
      syscalls:sys_enter_sched_yield                     [Tracepoint event]
      syscalls:sys_enter_seccomp                         [Tracepoint event]
      syscalls:sys_enter_select                          [Tracepoint event]
      syscalls:sys_enter_semctl                          [Tracepoint event]
      syscalls:sys_enter_semget                          [Tracepoint event]
      syscalls:sys_enter_semop                           [Tracepoint event]
      syscalls:sys_enter_semtimedop                      [Tracepoint event]
      syscalls:sys_enter_sendfile64                      [Tracepoint event]
      syscalls:sys_enter_sendmmsg                        [Tracepoint event]
      syscalls:sys_enter_sendmsg                         [Tracepoint event]
      syscalls:sys_enter_sendto                          [Tracepoint event]
      syscalls:sys_enter_set_mempolicy                   [Tracepoint event]
      syscalls:sys_enter_set_robust_list                 [Tracepoint event]
      syscalls:sys_enter_set_tid_address                 [Tracepoint event]
      syscalls:sys_enter_setdomainname                   [Tracepoint event]
      syscalls:sys_enter_setfsgid                        [Tracepoint event]
      syscalls:sys_enter_setfsuid                        [Tracepoint event]
      syscalls:sys_enter_setgid                          [Tracepoint event]
      syscalls:sys_enter_setgroups                       [Tracepoint event]
      syscalls:sys_enter_sethostname                     [Tracepoint event]
      syscalls:sys_enter_setitimer                       [Tracepoint event]
      syscalls:sys_enter_setns                           [Tracepoint event]
      syscalls:sys_enter_setpgid                         [Tracepoint event]
      syscalls:sys_enter_setpriority                     [Tracepoint event]
      syscalls:sys_enter_setregid                        [Tracepoint event]
      syscalls:sys_enter_setresgid                       [Tracepoint event]
      syscalls:sys_enter_setresuid                       [Tracepoint event]
      syscalls:sys_enter_setreuid                        [Tracepoint event]
      syscalls:sys_enter_setrlimit                       [Tracepoint event]
      syscalls:sys_enter_setsid                          [Tracepoint event]
      syscalls:sys_enter_setsockopt                      [Tracepoint event]
      syscalls:sys_enter_settimeofday                    [Tracepoint event]
      syscalls:sys_enter_setuid                          [Tracepoint event]
      syscalls:sys_enter_setxattr                        [Tracepoint event]
      syscalls:sys_enter_shmat                           [Tracepoint event]
      syscalls:sys_enter_shmctl                          [Tracepoint event]
      syscalls:sys_enter_shmdt                           [Tracepoint event]
      syscalls:sys_enter_shmget                          [Tracepoint event]
      syscalls:sys_enter_shutdown                        [Tracepoint event]
      syscalls:sys_enter_sigaltstack                     [Tracepoint event]
      syscalls:sys_enter_signalfd                        [Tracepoint event]
      syscalls:sys_enter_signalfd4                       [Tracepoint event]
      syscalls:sys_enter_socket                          [Tracepoint event]
      syscalls:sys_enter_socketpair                      [Tracepoint event]
      syscalls:sys_enter_splice                          [Tracepoint event]
      syscalls:sys_enter_statfs                          [Tracepoint event]
      syscalls:sys_enter_statx                           [Tracepoint event]
      syscalls:sys_enter_swapoff                         [Tracepoint event]
      syscalls:sys_enter_swapon                          [Tracepoint event]
      syscalls:sys_enter_symlink                         [Tracepoint event]
      syscalls:sys_enter_symlinkat                       [Tracepoint event]
      syscalls:sys_enter_sync                            [Tracepoint event]
      syscalls:sys_enter_sync_file_range                 [Tracepoint event]
      syscalls:sys_enter_syncfs                          [Tracepoint event]
      syscalls:sys_enter_sysctl                          [Tracepoint event]
      syscalls:sys_enter_sysfs                           [Tracepoint event]
      syscalls:sys_enter_sysinfo                         [Tracepoint event]
      syscalls:sys_enter_syslog                          [Tracepoint event]
      syscalls:sys_enter_tee                             [Tracepoint event]
      syscalls:sys_enter_tgkill                          [Tracepoint event]
      syscalls:sys_enter_time                            [Tracepoint event]
      syscalls:sys_enter_timer_create                    [Tracepoint event]
      syscalls:sys_enter_timer_delete                    [Tracepoint event]
      syscalls:sys_enter_timer_getoverrun                [Tracepoint event]
      syscalls:sys_enter_timer_gettime                   [Tracepoint event]
      syscalls:sys_enter_timer_settime                   [Tracepoint event]
      syscalls:sys_enter_timerfd_create                  [Tracepoint event]
      syscalls:sys_enter_timerfd_gettime                 [Tracepoint event]
      syscalls:sys_enter_timerfd_settime                 [Tracepoint event]
      syscalls:sys_enter_times                           [Tracepoint event]
      syscalls:sys_enter_tkill                           [Tracepoint event]
      syscalls:sys_enter_truncate                        [Tracepoint event]
      syscalls:sys_enter_umask                           [Tracepoint event]
      syscalls:sys_enter_umount                          [Tracepoint event]
      syscalls:sys_enter_unlink                          [Tracepoint event]
      syscalls:sys_enter_unlinkat                        [Tracepoint event]
      syscalls:sys_enter_unshare                         [Tracepoint event]
      syscalls:sys_enter_userfaultfd                     [Tracepoint event]
      syscalls:sys_enter_ustat                           [Tracepoint event]
      syscalls:sys_enter_utime                           [Tracepoint event]
      syscalls:sys_enter_utimensat                       [Tracepoint event]
      syscalls:sys_enter_utimes                          [Tracepoint event]
      syscalls:sys_enter_vfork                           [Tracepoint event]
      syscalls:sys_enter_vhangup                         [Tracepoint event]
      syscalls:sys_enter_vmsplice                        [Tracepoint event]
      syscalls:sys_enter_wait4                           [Tracepoint event]
      syscalls:sys_enter_waitid                          [Tracepoint event]
      syscalls:sys_enter_write                           [Tracepoint event]
      syscalls:sys_enter_writev                          [Tracepoint event]
      syscalls:sys_exit_accept                           [Tracepoint event]
      syscalls:sys_exit_accept4                          [Tracepoint event]
      syscalls:sys_exit_access                           [Tracepoint event]
      syscalls:sys_exit_acct                             [Tracepoint event]
      syscalls:sys_exit_add_key                          [Tracepoint event]
      syscalls:sys_exit_adjtimex                         [Tracepoint event]
      syscalls:sys_exit_alarm                            [Tracepoint event]
      syscalls:sys_exit_arch_prctl                       [Tracepoint event]
      syscalls:sys_exit_bind                             [Tracepoint event]
      syscalls:sys_exit_bpf                              [Tracepoint event]
      syscalls:sys_exit_brk                              [Tracepoint event]
      syscalls:sys_exit_capget                           [Tracepoint event]
      syscalls:sys_exit_capset                           [Tracepoint event]
      syscalls:sys_exit_chdir                            [Tracepoint event]
      syscalls:sys_exit_chmod                            [Tracepoint event]
      syscalls:sys_exit_chown                            [Tracepoint event]
      syscalls:sys_exit_chroot                           [Tracepoint event]
      syscalls:sys_exit_clock_adjtime                    [Tracepoint event]
      syscalls:sys_exit_clock_getres                     [Tracepoint event]
      syscalls:sys_exit_clock_gettime                    [Tracepoint event]
      syscalls:sys_exit_clock_nanosleep                  [Tracepoint event]
      syscalls:sys_exit_clock_settime                    [Tracepoint event]
      syscalls:sys_exit_clone                            [Tracepoint event]
      syscalls:sys_exit_clone3                           [Tracepoint event]
      syscalls:sys_exit_close                            [Tracepoint event]
      syscalls:sys_exit_connect                          [Tracepoint event]
      syscalls:sys_exit_copy_file_range                  [Tracepoint event]
      syscalls:sys_exit_creat                            [Tracepoint event]
      syscalls:sys_exit_delete_module                    [Tracepoint event]
      syscalls:sys_exit_dup                              [Tracepoint event]
      syscalls:sys_exit_dup2                             [Tracepoint event]
      syscalls:sys_exit_dup3                             [Tracepoint event]
      syscalls:sys_exit_epoll_create                     [Tracepoint event]
      syscalls:sys_exit_epoll_create1                    [Tracepoint event]
      syscalls:sys_exit_epoll_ctl                        [Tracepoint event]
      syscalls:sys_exit_epoll_pwait                      [Tracepoint event]
      syscalls:sys_exit_epoll_wait                       [Tracepoint event]
      syscalls:sys_exit_eventfd                          [Tracepoint event]
      syscalls:sys_exit_eventfd2                         [Tracepoint event]
      syscalls:sys_exit_execve                           [Tracepoint event]
      syscalls:sys_exit_execveat                         [Tracepoint event]
      syscalls:sys_exit_exit                             [Tracepoint event]
      syscalls:sys_exit_exit_group                       [Tracepoint event]
      syscalls:sys_exit_faccessat                        [Tracepoint event]
      syscalls:sys_exit_fadvise64                        [Tracepoint event]
      syscalls:sys_exit_fallocate                        [Tracepoint event]
      syscalls:sys_exit_fanotify_init                    [Tracepoint event]
      syscalls:sys_exit_fanotify_mark                    [Tracepoint event]
      syscalls:sys_exit_fchdir                           [Tracepoint event]
      syscalls:sys_exit_fchmod                           [Tracepoint event]
      syscalls:sys_exit_fchmodat                         [Tracepoint event]
      syscalls:sys_exit_fchown                           [Tracepoint event]
      syscalls:sys_exit_fchownat                         [Tracepoint event]
      syscalls:sys_exit_fcntl                            [Tracepoint event]
      syscalls:sys_exit_fdatasync                        [Tracepoint event]
      syscalls:sys_exit_fgetxattr                        [Tracepoint event]
      syscalls:sys_exit_finit_module                     [Tracepoint event]
      syscalls:sys_exit_flistxattr                       [Tracepoint event]
      syscalls:sys_exit_flock                            [Tracepoint event]
      syscalls:sys_exit_fork                             [Tracepoint event]
      syscalls:sys_exit_fremovexattr                     [Tracepoint event]
      syscalls:sys_exit_fsconfig                         [Tracepoint event]
      syscalls:sys_exit_fsetxattr                        [Tracepoint event]
      syscalls:sys_exit_fsmount                          [Tracepoint event]
      syscalls:sys_exit_fsopen                           [Tracepoint event]
      syscalls:sys_exit_fspick                           [Tracepoint event]
      syscalls:sys_exit_fstatfs                          [Tracepoint event]
      syscalls:sys_exit_fsync                            [Tracepoint event]
      syscalls:sys_exit_ftruncate                        [Tracepoint event]
      syscalls:sys_exit_futex                            [Tracepoint event]
      syscalls:sys_exit_futimesat                        [Tracepoint event]
      syscalls:sys_exit_get_mempolicy                    [Tracepoint event]
      syscalls:sys_exit_get_robust_list                  [Tracepoint event]
      syscalls:sys_exit_getcpu                           [Tracepoint event]
      syscalls:sys_exit_getcwd                           [Tracepoint event]
      syscalls:sys_exit_getdents                         [Tracepoint event]
      syscalls:sys_exit_getdents64                       [Tracepoint event]
      syscalls:sys_exit_getegid                          [Tracepoint event]
      syscalls:sys_exit_geteuid                          [Tracepoint event]
      syscalls:sys_exit_getgid                           [Tracepoint event]
      syscalls:sys_exit_getgroups                        [Tracepoint event]
      syscalls:sys_exit_getitimer                        [Tracepoint event]
      syscalls:sys_exit_getpeername                      [Tracepoint event]
      syscalls:sys_exit_getpgid                          [Tracepoint event]
      syscalls:sys_exit_getpgrp                          [Tracepoint event]
      syscalls:sys_exit_getpid                           [Tracepoint event]
      syscalls:sys_exit_getppid                          [Tracepoint event]
      syscalls:sys_exit_getpriority                      [Tracepoint event]
      syscalls:sys_exit_getrandom                        [Tracepoint event]
      syscalls:sys_exit_getresgid                        [Tracepoint event]
      syscalls:sys_exit_getresuid                        [Tracepoint event]
      syscalls:sys_exit_getrlimit                        [Tracepoint event]
      syscalls:sys_exit_getrusage                        [Tracepoint event]
      syscalls:sys_exit_getsid                           [Tracepoint event]
      syscalls:sys_exit_getsockname                      [Tracepoint event]
      syscalls:sys_exit_getsockopt                       [Tracepoint event]
      syscalls:sys_exit_gettid                           [Tracepoint event]
      syscalls:sys_exit_gettimeofday                     [Tracepoint event]
      syscalls:sys_exit_getuid                           [Tracepoint event]
      syscalls:sys_exit_getxattr                         [Tracepoint event]
      syscalls:sys_exit_init_module                      [Tracepoint event]
      syscalls:sys_exit_inotify_add_watch                [Tracepoint event]
      syscalls:sys_exit_inotify_init                     [Tracepoint event]
      syscalls:sys_exit_inotify_init1                    [Tracepoint event]
      syscalls:sys_exit_inotify_rm_watch                 [Tracepoint event]
      syscalls:sys_exit_io_cancel                        [Tracepoint event]
      syscalls:sys_exit_io_destroy                       [Tracepoint event]
      syscalls:sys_exit_io_getevents                     [Tracepoint event]
      syscalls:sys_exit_io_pgetevents                    [Tracepoint event]
      syscalls:sys_exit_io_setup                         [Tracepoint event]
      syscalls:sys_exit_io_submit                        [Tracepoint event]
      syscalls:sys_exit_io_uring_enter                   [Tracepoint event]
      syscalls:sys_exit_io_uring_register                [Tracepoint event]
      syscalls:sys_exit_io_uring_setup                   [Tracepoint event]
      syscalls:sys_exit_ioctl                            [Tracepoint event]
      syscalls:sys_exit_ioperm                           [Tracepoint event]
      syscalls:sys_exit_iopl                             [Tracepoint event]
      syscalls:sys_exit_ioprio_get                       [Tracepoint event]
      syscalls:sys_exit_ioprio_set                       [Tracepoint event]
      syscalls:sys_exit_kcmp                             [Tracepoint event]
      syscalls:sys_exit_kexec_file_load                  [Tracepoint event]
      syscalls:sys_exit_kexec_load                       [Tracepoint event]
      syscalls:sys_exit_keyctl                           [Tracepoint event]
      syscalls:sys_exit_kill                             [Tracepoint event]
      syscalls:sys_exit_lchown                           [Tracepoint event]
      syscalls:sys_exit_lgetxattr                        [Tracepoint event]
      syscalls:sys_exit_link                             [Tracepoint event]
      syscalls:sys_exit_linkat                           [Tracepoint event]
      syscalls:sys_exit_listen                           [Tracepoint event]
      syscalls:sys_exit_listxattr                        [Tracepoint event]
      syscalls:sys_exit_llistxattr                       [Tracepoint event]
      syscalls:sys_exit_lookup_dcookie                   [Tracepoint event]
      syscalls:sys_exit_lremovexattr                     [Tracepoint event]
      syscalls:sys_exit_lseek                            [Tracepoint event]
      syscalls:sys_exit_lsetxattr                        [Tracepoint event]
      syscalls:sys_exit_madvise                          [Tracepoint event]
      syscalls:sys_exit_mbind                            [Tracepoint event]
      syscalls:sys_exit_membarrier                       [Tracepoint event]
      syscalls:sys_exit_memfd_create                     [Tracepoint event]
      syscalls:sys_exit_migrate_pages                    [Tracepoint event]
      syscalls:sys_exit_mincore                          [Tracepoint event]
      syscalls:sys_exit_mkdir                            [Tracepoint event]
      syscalls:sys_exit_mkdirat                          [Tracepoint event]
      syscalls:sys_exit_mknod                            [Tracepoint event]
      syscalls:sys_exit_mknodat                          [Tracepoint event]
      syscalls:sys_exit_mlock                            [Tracepoint event]
      syscalls:sys_exit_mlock2                           [Tracepoint event]
      syscalls:sys_exit_mlockall                         [Tracepoint event]
      syscalls:sys_exit_mmap                             [Tracepoint event]
      syscalls:sys_exit_modify_ldt                       [Tracepoint event]
      syscalls:sys_exit_mount                            [Tracepoint event]
      syscalls:sys_exit_move_mount                       [Tracepoint event]
      syscalls:sys_exit_move_pages                       [Tracepoint event]
      syscalls:sys_exit_mprotect                         [Tracepoint event]
      syscalls:sys_exit_mq_getsetattr                    [Tracepoint event]
      syscalls:sys_exit_mq_notify                        [Tracepoint event]
      syscalls:sys_exit_mq_open                          [Tracepoint event]
      syscalls:sys_exit_mq_timedreceive                  [Tracepoint event]
      syscalls:sys_exit_mq_timedsend                     [Tracepoint event]
      syscalls:sys_exit_mq_unlink                        [Tracepoint event]
      syscalls:sys_exit_mremap                           [Tracepoint event]
      syscalls:sys_exit_msgctl                           [Tracepoint event]
      syscalls:sys_exit_msgget                           [Tracepoint event]
      syscalls:sys_exit_msgrcv                           [Tracepoint event]
      syscalls:sys_exit_msgsnd                           [Tracepoint event]
      syscalls:sys_exit_msync                            [Tracepoint event]
      syscalls:sys_exit_munlock                          [Tracepoint event]
      syscalls:sys_exit_munlockall                       [Tracepoint event]
      syscalls:sys_exit_munmap                           [Tracepoint event]
      syscalls:sys_exit_name_to_handle_at                [Tracepoint event]
      syscalls:sys_exit_nanosleep                        [Tracepoint event]
      syscalls:sys_exit_newfstat                         [Tracepoint event]
      syscalls:sys_exit_newfstatat                       [Tracepoint event]
      syscalls:sys_exit_newlstat                         [Tracepoint event]
      syscalls:sys_exit_newstat                          [Tracepoint event]
      syscalls:sys_exit_newuname                         [Tracepoint event]
      syscalls:sys_exit_open                             [Tracepoint event]
      syscalls:sys_exit_open_by_handle_at                [Tracepoint event]
      syscalls:sys_exit_open_tree                        [Tracepoint event]
      syscalls:sys_exit_openat                           [Tracepoint event]
      syscalls:sys_exit_pause                            [Tracepoint event]
      syscalls:sys_exit_perf_event_open                  [Tracepoint event]
      syscalls:sys_exit_personality                      [Tracepoint event]
      syscalls:sys_exit_pidfd_open                       [Tracepoint event]
      syscalls:sys_exit_pidfd_send_signal                [Tracepoint event]
      syscalls:sys_exit_pipe                             [Tracepoint event]
      syscalls:sys_exit_pipe2                            [Tracepoint event]
      syscalls:sys_exit_pivot_root                       [Tracepoint event]
      syscalls:sys_exit_pkey_alloc                       [Tracepoint event]
      syscalls:sys_exit_pkey_free                        [Tracepoint event]
      syscalls:sys_exit_pkey_mprotect                    [Tracepoint event]
      syscalls:sys_exit_poll                             [Tracepoint event]
      syscalls:sys_exit_ppoll                            [Tracepoint event]
      syscalls:sys_exit_prctl                            [Tracepoint event]
      syscalls:sys_exit_pread64                          [Tracepoint event]
      syscalls:sys_exit_preadv                           [Tracepoint event]
      syscalls:sys_exit_preadv2                          [Tracepoint event]
      syscalls:sys_exit_prlimit64                        [Tracepoint event]
      syscalls:sys_exit_process_vm_readv                 [Tracepoint event]
      syscalls:sys_exit_process_vm_writev                [Tracepoint event]
      syscalls:sys_exit_pselect6                         [Tracepoint event]
      syscalls:sys_exit_ptrace                           [Tracepoint event]
      syscalls:sys_exit_pwrite64                         [Tracepoint event]
      syscalls:sys_exit_pwritev                          [Tracepoint event]
      syscalls:sys_exit_pwritev2                         [Tracepoint event]
      syscalls:sys_exit_quotactl                         [Tracepoint event]
      syscalls:sys_exit_read                             [Tracepoint event]
      syscalls:sys_exit_readahead                        [Tracepoint event]
      syscalls:sys_exit_readlink                         [Tracepoint event]
      syscalls:sys_exit_readlinkat                       [Tracepoint event]
      syscalls:sys_exit_readv                            [Tracepoint event]
      syscalls:sys_exit_reboot                           [Tracepoint event]
      syscalls:sys_exit_recvfrom                         [Tracepoint event]
      syscalls:sys_exit_recvmmsg                         [Tracepoint event]
      syscalls:sys_exit_recvmsg                          [Tracepoint event]
      syscalls:sys_exit_remap_file_pages                 [Tracepoint event]
      syscalls:sys_exit_removexattr                      [Tracepoint event]
      syscalls:sys_exit_rename                           [Tracepoint event]
      syscalls:sys_exit_renameat                         [Tracepoint event]
      syscalls:sys_exit_renameat2                        [Tracepoint event]
      syscalls:sys_exit_request_key                      [Tracepoint event]
      syscalls:sys_exit_restart_syscall                  [Tracepoint event]
      syscalls:sys_exit_rmdir                            [Tracepoint event]
      syscalls:sys_exit_rseq                             [Tracepoint event]
      syscalls:sys_exit_rt_sigaction                     [Tracepoint event]
      syscalls:sys_exit_rt_sigpending                    [Tracepoint event]
      syscalls:sys_exit_rt_sigprocmask                   [Tracepoint event]
      syscalls:sys_exit_rt_sigqueueinfo                  [Tracepoint event]
      syscalls:sys_exit_rt_sigreturn                     [Tracepoint event]
      syscalls:sys_exit_rt_sigsuspend                    [Tracepoint event]
      syscalls:sys_exit_rt_sigtimedwait                  [Tracepoint event]
      syscalls:sys_exit_rt_tgsigqueueinfo                [Tracepoint event]
      syscalls:sys_exit_sched_get_priority_max           [Tracepoint event]
      syscalls:sys_exit_sched_get_priority_min           [Tracepoint event]
      syscalls:sys_exit_sched_getaffinity                [Tracepoint event]
      syscalls:sys_exit_sched_getattr                    [Tracepoint event]
      syscalls:sys_exit_sched_getparam                   [Tracepoint event]
      syscalls:sys_exit_sched_getscheduler               [Tracepoint event]
      syscalls:sys_exit_sched_rr_get_interval            [Tracepoint event]
      syscalls:sys_exit_sched_setaffinity                [Tracepoint event]
      syscalls:sys_exit_sched_setattr                    [Tracepoint event]
      syscalls:sys_exit_sched_setparam                   [Tracepoint event]
      syscalls:sys_exit_sched_setscheduler               [Tracepoint event]
      syscalls:sys_exit_sched_yield                      [Tracepoint event]
      syscalls:sys_exit_seccomp                          [Tracepoint event]
      syscalls:sys_exit_select                           [Tracepoint event]
      syscalls:sys_exit_semctl                           [Tracepoint event]
      syscalls:sys_exit_semget                           [Tracepoint event]
      syscalls:sys_exit_semop                            [Tracepoint event]
      syscalls:sys_exit_semtimedop                       [Tracepoint event]
      syscalls:sys_exit_sendfile64                       [Tracepoint event]
      syscalls:sys_exit_sendmmsg                         [Tracepoint event]
      syscalls:sys_exit_sendmsg                          [Tracepoint event]
      syscalls:sys_exit_sendto                           [Tracepoint event]
      syscalls:sys_exit_set_mempolicy                    [Tracepoint event]
      syscalls:sys_exit_set_robust_list                  [Tracepoint event]
      syscalls:sys_exit_set_tid_address                  [Tracepoint event]
      syscalls:sys_exit_setdomainname                    [Tracepoint event]
      syscalls:sys_exit_setfsgid                         [Tracepoint event]
      syscalls:sys_exit_setfsuid                         [Tracepoint event]
      syscalls:sys_exit_setgid                           [Tracepoint event]
      syscalls:sys_exit_setgroups                        [Tracepoint event]
      syscalls:sys_exit_sethostname                      [Tracepoint event]
      syscalls:sys_exit_setitimer                        [Tracepoint event]
      syscalls:sys_exit_setns                            [Tracepoint event]
      syscalls:sys_exit_setpgid                          [Tracepoint event]
      syscalls:sys_exit_setpriority                      [Tracepoint event]
      syscalls:sys_exit_setregid                         [Tracepoint event]
      syscalls:sys_exit_setresgid                        [Tracepoint event]
      syscalls:sys_exit_setresuid                        [Tracepoint event]
      syscalls:sys_exit_setreuid                         [Tracepoint event]
      syscalls:sys_exit_setrlimit                        [Tracepoint event]
      syscalls:sys_exit_setsid                           [Tracepoint event]
      syscalls:sys_exit_setsockopt                       [Tracepoint event]
      syscalls:sys_exit_settimeofday                     [Tracepoint event]
      syscalls:sys_exit_setuid                           [Tracepoint event]
      syscalls:sys_exit_setxattr                         [Tracepoint event]
      syscalls:sys_exit_shmat                            [Tracepoint event]
      syscalls:sys_exit_shmctl                           [Tracepoint event]
      syscalls:sys_exit_shmdt                            [Tracepoint event]
      syscalls:sys_exit_shmget                           [Tracepoint event]
      syscalls:sys_exit_shutdown                         [Tracepoint event]
      syscalls:sys_exit_sigaltstack                      [Tracepoint event]
      syscalls:sys_exit_signalfd                         [Tracepoint event]
      syscalls:sys_exit_signalfd4                        [Tracepoint event]
      syscalls:sys_exit_socket                           [Tracepoint event]
      syscalls:sys_exit_socketpair                       [Tracepoint event]
      syscalls:sys_exit_splice                           [Tracepoint event]
      syscalls:sys_exit_statfs                           [Tracepoint event]
      syscalls:sys_exit_statx                            [Tracepoint event]
      syscalls:sys_exit_swapoff                          [Tracepoint event]
      syscalls:sys_exit_swapon                           [Tracepoint event]
      syscalls:sys_exit_symlink                          [Tracepoint event]
      syscalls:sys_exit_symlinkat                        [Tracepoint event]
      syscalls:sys_exit_sync                             [Tracepoint event]
      syscalls:sys_exit_sync_file_range                  [Tracepoint event]
      syscalls:sys_exit_syncfs                           [Tracepoint event]
      syscalls:sys_exit_sysctl                           [Tracepoint event]
      syscalls:sys_exit_sysfs                            [Tracepoint event]
      syscalls:sys_exit_sysinfo                          [Tracepoint event]
      syscalls:sys_exit_syslog                           [Tracepoint event]
      syscalls:sys_exit_tee                              [Tracepoint event]
      syscalls:sys_exit_tgkill                           [Tracepoint event]
      syscalls:sys_exit_time                             [Tracepoint event]
      syscalls:sys_exit_timer_create                     [Tracepoint event]
      syscalls:sys_exit_timer_delete                     [Tracepoint event]
      syscalls:sys_exit_timer_getoverrun                 [Tracepoint event]
      syscalls:sys_exit_timer_gettime                    [Tracepoint event]
      syscalls:sys_exit_timer_settime                    [Tracepoint event]
      syscalls:sys_exit_timerfd_create                   [Tracepoint event]
      syscalls:sys_exit_timerfd_gettime                  [Tracepoint event]
      syscalls:sys_exit_timerfd_settime                  [Tracepoint event]
      syscalls:sys_exit_times                            [Tracepoint event]
      syscalls:sys_exit_tkill                            [Tracepoint event]
      syscalls:sys_exit_truncate                         [Tracepoint event]
      syscalls:sys_exit_umask                            [Tracepoint event]
      syscalls:sys_exit_umount                           [Tracepoint event]
      syscalls:sys_exit_unlink                           [Tracepoint event]
      syscalls:sys_exit_unlinkat                         [Tracepoint event]
      syscalls:sys_exit_unshare                          [Tracepoint event]
      syscalls:sys_exit_userfaultfd                      [Tracepoint event]
      syscalls:sys_exit_ustat                            [Tracepoint event]
      syscalls:sys_exit_utime                            [Tracepoint event]
      syscalls:sys_exit_utimensat                        [Tracepoint event]
      syscalls:sys_exit_utimes                           [Tracepoint event]
      syscalls:sys_exit_vfork                            [Tracepoint event]
      syscalls:sys_exit_vhangup                          [Tracepoint event]
      syscalls:sys_exit_vmsplice                         [Tracepoint event]
      syscalls:sys_exit_wait4                            [Tracepoint event]
      syscalls:sys_exit_waitid                           [Tracepoint event]
      syscalls:sys_exit_write                            [Tracepoint event]
      syscalls:sys_exit_writev                           [Tracepoint event]
      task:task_newtask                                  [Tracepoint event]
      task:task_rename                                   [Tracepoint event]
      tcp:tcp_destroy_sock                               [Tracepoint event]
      tcp:tcp_probe                                      [Tracepoint event]
      tcp:tcp_rcv_space_adjust                           [Tracepoint event]
      tcp:tcp_receive_reset                              [Tracepoint event]
      tcp:tcp_retransmit_skb                             [Tracepoint event]
      tcp:tcp_retransmit_synack                          [Tracepoint event]
      tcp:tcp_send_reset                                 [Tracepoint event]
      thermal:cdev_update                                [Tracepoint event]
      thermal:thermal_power_devfreq_get_power            [Tracepoint event]
      thermal:thermal_power_devfreq_limit                [Tracepoint event]
      thermal:thermal_temperature                        [Tracepoint event]
      thermal:thermal_zone_trip                          [Tracepoint event]
      thermal_power_allocator:thermal_power_allocator    [Tracepoint event]
      thermal_power_allocator:thermal_power_allocator_pid [Tracepoint event]
      timer:hrtimer_cancel                               [Tracepoint event]
      timer:hrtimer_expire_entry                         [Tracepoint event]
      timer:hrtimer_expire_exit                          [Tracepoint event]
      timer:hrtimer_init                                 [Tracepoint event]
      timer:hrtimer_start                                [Tracepoint event]
      timer:itimer_expire                                [Tracepoint event]
      timer:itimer_state                                 [Tracepoint event]
      timer:tick_stop                                    [Tracepoint event]
      timer:timer_cancel                                 [Tracepoint event]
      timer:timer_expire_entry                           [Tracepoint event]
      timer:timer_expire_exit                            [Tracepoint event]
      timer:timer_init                                   [Tracepoint event]
      timer:timer_start                                  [Tracepoint event]
      tlb:tlb_flush                                      [Tracepoint event]
      udp:udp_fail_queue_rcv_skb                         [Tracepoint event]
      vmscan:mm_shrink_slab_end                          [Tracepoint event]
      vmscan:mm_shrink_slab_start                        [Tracepoint event]
      vmscan:mm_vmscan_direct_reclaim_begin              [Tracepoint event]
      vmscan:mm_vmscan_direct_reclaim_end                [Tracepoint event]
      vmscan:mm_vmscan_inactive_list_is_low              [Tracepoint event]
      vmscan:mm_vmscan_kswapd_sleep                      [Tracepoint event]
      vmscan:mm_vmscan_kswapd_wake                       [Tracepoint event]
      vmscan:mm_vmscan_lru_isolate                       [Tracepoint event]
      vmscan:mm_vmscan_lru_shrink_active                 [Tracepoint event]
      vmscan:mm_vmscan_lru_shrink_inactive               [Tracepoint event]
      vmscan:mm_vmscan_memcg_reclaim_begin               [Tracepoint event]
      vmscan:mm_vmscan_memcg_reclaim_end                 [Tracepoint event]
      vmscan:mm_vmscan_memcg_softlimit_reclaim_begin     [Tracepoint event]
      vmscan:mm_vmscan_memcg_softlimit_reclaim_end       [Tracepoint event]
      vmscan:mm_vmscan_node_reclaim_begin                [Tracepoint event]
      vmscan:mm_vmscan_node_reclaim_end                  [Tracepoint event]
      vmscan:mm_vmscan_wakeup_kswapd                     [Tracepoint event]
      vmscan:mm_vmscan_writepage                         [Tracepoint event]
      vsyscall:emulate_vsyscall                          [Tracepoint event]
      wbt:wbt_lat                                        [Tracepoint event]
      wbt:wbt_stat                                       [Tracepoint event]
      wbt:wbt_step                                       [Tracepoint event]
      wbt:wbt_timer                                      [Tracepoint event]
      workqueue:workqueue_activate_work                  [Tracepoint event]
      workqueue:workqueue_execute_end                    [Tracepoint event]
      workqueue:workqueue_execute_start                  [Tracepoint event]
      workqueue:workqueue_queue_work                     [Tracepoint event]
      writeback:balance_dirty_pages                      [Tracepoint event]
      writeback:bdi_dirty_ratelimit                      [Tracepoint event]
      writeback:flush_foreign                            [Tracepoint event]
      writeback:global_dirty_state                       [Tracepoint event]
      writeback:inode_foreign_history                    [Tracepoint event]
      writeback:inode_switch_wbs                         [Tracepoint event]
      writeback:sb_clear_inode_writeback                 [Tracepoint event]
      writeback:sb_mark_inode_writeback                  [Tracepoint event]
      writeback:track_foreign_dirty                      [Tracepoint event]
      writeback:wait_on_page_writeback                   [Tracepoint event]
      writeback:wbc_writepage                            [Tracepoint event]
      writeback:writeback_bdi_register                   [Tracepoint event]
      writeback:writeback_congestion_wait                [Tracepoint event]
      writeback:writeback_dirty_inode                    [Tracepoint event]
      writeback:writeback_dirty_inode_enqueue            [Tracepoint event]
      writeback:writeback_dirty_inode_start              [Tracepoint event]
      writeback:writeback_dirty_page                     [Tracepoint event]
      writeback:writeback_exec                           [Tracepoint event]
      writeback:writeback_lazytime                       [Tracepoint event]
      writeback:writeback_lazytime_iput                  [Tracepoint event]
      writeback:writeback_mark_inode_dirty               [Tracepoint event]
      writeback:writeback_pages_written                  [Tracepoint event]
      writeback:writeback_queue                          [Tracepoint event]
      writeback:writeback_queue_io                       [Tracepoint event]
      writeback:writeback_sb_inodes_requeue              [Tracepoint event]
      writeback:writeback_single_inode                   [Tracepoint event]
      writeback:writeback_single_inode_start             [Tracepoint event]
      writeback:writeback_start                          [Tracepoint event]
      writeback:writeback_wait                           [Tracepoint event]
      writeback:writeback_wait_iff_congested             [Tracepoint event]
      writeback:writeback_wake_background                [Tracepoint event]
      writeback:writeback_write_inode                    [Tracepoint event]
      writeback:writeback_write_inode_start              [Tracepoint event]
      writeback:writeback_written                        [Tracepoint event]
      x86_fpu:x86_fpu_after_restore                      [Tracepoint event]
      x86_fpu:x86_fpu_after_save                         [Tracepoint event]
      x86_fpu:x86_fpu_before_restore                     [Tracepoint event]
      x86_fpu:x86_fpu_before_save                        [Tracepoint event]
      x86_fpu:x86_fpu_copy_dst                           [Tracepoint event]
      x86_fpu:x86_fpu_copy_src                           [Tracepoint event]
      x86_fpu:x86_fpu_dropped                            [Tracepoint event]
      x86_fpu:x86_fpu_init_state                         [Tracepoint event]
      x86_fpu:x86_fpu_regs_activated                     [Tracepoint event]
      x86_fpu:x86_fpu_regs_deactivated                   [Tracepoint event]
      x86_fpu:x86_fpu_xstate_check_failed                [Tracepoint event]
      xdp:mem_connect                                    [Tracepoint event]
      xdp:mem_disconnect                                 [Tracepoint event]
      xdp:mem_return_failed                              [Tracepoint event]
      xdp:xdp_bulk_tx                                    [Tracepoint event]
      xdp:xdp_cpumap_enqueue                             [Tracepoint event]
      xdp:xdp_cpumap_kthread                             [Tracepoint event]
      xdp:xdp_devmap_xmit                                [Tracepoint event]
      xdp:xdp_exception                                  [Tracepoint event]
      xdp:xdp_redirect                                   [Tracepoint event]
      xdp:xdp_redirect_err                               [Tracepoint event]
      xdp:xdp_redirect_map                               [Tracepoint event]
      xdp:xdp_redirect_map_err                           [Tracepoint event]
      xen:xen_cpu_load_idt                               [Tracepoint event]
      xen:xen_cpu_set_ldt                                [Tracepoint event]
      xen:xen_cpu_write_gdt_entry                        [Tracepoint event]
      xen:xen_cpu_write_idt_entry                        [Tracepoint event]
      xen:xen_cpu_write_ldt_entry                        [Tracepoint event]
      xen:xen_mc_batch                                   [Tracepoint event]
      xen:xen_mc_callback                                [Tracepoint event]
      xen:xen_mc_entry                                   [Tracepoint event]
      xen:xen_mc_entry_alloc                             [Tracepoint event]
      xen:xen_mc_extend_args                             [Tracepoint event]
      xen:xen_mc_flush                                   [Tracepoint event]
      xen:xen_mc_flush_reason                            [Tracepoint event]
      xen:xen_mc_issue                                   [Tracepoint event]
      xen:xen_mmu_alloc_ptpage                           [Tracepoint event]
      xen:xen_mmu_flush_tlb_one_user                     [Tracepoint event]
      xen:xen_mmu_flush_tlb_others                       [Tracepoint event]
      xen:xen_mmu_pgd_pin                                [Tracepoint event]
      xen:xen_mmu_pgd_unpin                              [Tracepoint event]
      xen:xen_mmu_ptep_modify_prot_commit                [Tracepoint event]
      xen:xen_mmu_ptep_modify_prot_start                 [Tracepoint event]
      xen:xen_mmu_release_ptpage                         [Tracepoint event]
      xen:xen_mmu_set_p4d                                [Tracepoint event]
      xen:xen_mmu_set_pmd                                [Tracepoint event]
      xen:xen_mmu_set_pte                                [Tracepoint event]
      xen:xen_mmu_set_pte_at                             [Tracepoint event]
      xen:xen_mmu_set_pud                                [Tracepoint event]
      xen:xen_mmu_write_cr3                              [Tracepoint event]
      xfs:kmem_alloc                                     [Tracepoint event]
      xfs:kmem_alloc_io                                  [Tracepoint event]
      xfs:kmem_alloc_large                               [Tracepoint event]
      xfs:kmem_realloc                                   [Tracepoint event]
      xfs:kmem_zone_alloc                                [Tracepoint event]
      xfs:xfs_ag_mark_healthy                            [Tracepoint event]
      xfs:xfs_ag_mark_sick                               [Tracepoint event]
      xfs:xfs_ag_resv_alloc_extent                       [Tracepoint event]
      xfs:xfs_ag_resv_critical                           [Tracepoint event]
      xfs:xfs_ag_resv_free                               [Tracepoint event]
      xfs:xfs_ag_resv_free_error                         [Tracepoint event]
      xfs:xfs_ag_resv_free_extent                        [Tracepoint event]
      xfs:xfs_ag_resv_init                               [Tracepoint event]
      xfs:xfs_ag_resv_init_error                         [Tracepoint event]
      xfs:xfs_ag_resv_needed                             [Tracepoint event]
      xfs:xfs_ag_unfixed_corruption                      [Tracepoint event]
      xfs:xfs_agf                                        [Tracepoint event]
      xfs:xfs_agfl_free_defer                            [Tracepoint event]
      xfs:xfs_agfl_free_deferred                         [Tracepoint event]
      xfs:xfs_agfl_reset                                 [Tracepoint event]
      xfs:xfs_ail_delete                                 [Tracepoint event]
      xfs:xfs_ail_flushing                               [Tracepoint event]
      xfs:xfs_ail_insert                                 [Tracepoint event]
      xfs:xfs_ail_locked                                 [Tracepoint event]
      xfs:xfs_ail_move                                   [Tracepoint event]
      xfs:xfs_ail_pinned                                 [Tracepoint event]
      xfs:xfs_ail_push                                   [Tracepoint event]
      xfs:xfs_alloc_exact_done                           [Tracepoint event]
      xfs:xfs_alloc_exact_error                          [Tracepoint event]
      xfs:xfs_alloc_exact_notfound                       [Tracepoint event]
      xfs:xfs_alloc_file_space                           [Tracepoint event]
      xfs:xfs_alloc_near_busy                            [Tracepoint event]
      xfs:xfs_alloc_near_error                           [Tracepoint event]
      xfs:xfs_alloc_near_first                           [Tracepoint event]
      xfs:xfs_alloc_near_greater                         [Tracepoint event]
      xfs:xfs_alloc_near_lesser                          [Tracepoint event]
      xfs:xfs_alloc_near_noentry                         [Tracepoint event]
      xfs:xfs_alloc_near_nominleft                       [Tracepoint event]
      xfs:xfs_alloc_read_agf                             [Tracepoint event]
      xfs:xfs_alloc_size_busy                            [Tracepoint event]
      xfs:xfs_alloc_size_done                            [Tracepoint event]
      xfs:xfs_alloc_size_error                           [Tracepoint event]
      xfs:xfs_alloc_size_neither                         [Tracepoint event]
      xfs:xfs_alloc_size_noentry                         [Tracepoint event]
      xfs:xfs_alloc_size_nominleft                       [Tracepoint event]
      xfs:xfs_alloc_small_done                           [Tracepoint event]
      xfs:xfs_alloc_small_error                          [Tracepoint event]
      xfs:xfs_alloc_small_freelist                       [Tracepoint event]
      xfs:xfs_alloc_small_notenough                      [Tracepoint event]
      xfs:xfs_alloc_vextent_allfailed                    [Tracepoint event]
      xfs:xfs_alloc_vextent_badargs                      [Tracepoint event]
      xfs:xfs_alloc_vextent_loopfailed                   [Tracepoint event]
      xfs:xfs_alloc_vextent_noagbp                       [Tracepoint event]
      xfs:xfs_alloc_vextent_nofix                        [Tracepoint event]
      xfs:xfs_attr_fillstate                             [Tracepoint event]
      xfs:xfs_attr_leaf_add                              [Tracepoint event]
      xfs:xfs_attr_leaf_add_new                          [Tracepoint event]
      xfs:xfs_attr_leaf_add_old                          [Tracepoint event]
      xfs:xfs_attr_leaf_add_work                         [Tracepoint event]
      xfs:xfs_attr_leaf_addname                          [Tracepoint event]
      xfs:xfs_attr_leaf_clearflag                        [Tracepoint event]
      xfs:xfs_attr_leaf_compact                          [Tracepoint event]
      xfs:xfs_attr_leaf_create                           [Tracepoint event]
      xfs:xfs_attr_leaf_flipflags                        [Tracepoint event]
      xfs:xfs_attr_leaf_get                              [Tracepoint event]
      xfs:xfs_attr_leaf_list                             [Tracepoint event]
      xfs:xfs_attr_leaf_lookup                           [Tracepoint event]
      xfs:xfs_attr_leaf_rebalance                        [Tracepoint event]
      xfs:xfs_attr_leaf_remove                           [Tracepoint event]
      xfs:xfs_attr_leaf_removename                       [Tracepoint event]
      xfs:xfs_attr_leaf_replace                          [Tracepoint event]
      xfs:xfs_attr_leaf_setflag                          [Tracepoint event]
      xfs:xfs_attr_leaf_split                            [Tracepoint event]
      xfs:xfs_attr_leaf_split_after                      [Tracepoint event]
      xfs:xfs_attr_leaf_split_before                     [Tracepoint event]
      xfs:xfs_attr_leaf_to_node                          [Tracepoint event]
      xfs:xfs_attr_leaf_to_sf                            [Tracepoint event]
      xfs:xfs_attr_leaf_toosmall                         [Tracepoint event]
      xfs:xfs_attr_leaf_unbalance                        [Tracepoint event]
      xfs:xfs_attr_list_add                              [Tracepoint event]
      xfs:xfs_attr_list_full                             [Tracepoint event]
      xfs:xfs_attr_list_leaf                             [Tracepoint event]
      xfs:xfs_attr_list_leaf_end                         [Tracepoint event]
      xfs:xfs_attr_list_node_descend                     [Tracepoint event]
      xfs:xfs_attr_list_notfound                         [Tracepoint event]
      xfs:xfs_attr_list_sf                               [Tracepoint event]
      xfs:xfs_attr_list_sf_all                           [Tracepoint event]
      xfs:xfs_attr_list_wrong_blk                        [Tracepoint event]
      xfs:xfs_attr_node_addname                          [Tracepoint event]
      xfs:xfs_attr_node_get                              [Tracepoint event]
      xfs:xfs_attr_node_list                             [Tracepoint event]
      xfs:xfs_attr_node_removename                       [Tracepoint event]
      xfs:xfs_attr_node_replace                          [Tracepoint event]
      xfs:xfs_attr_refillstate                           [Tracepoint event]
      xfs:xfs_attr_rmtval_get                            [Tracepoint event]
      xfs:xfs_attr_rmtval_remove                         [Tracepoint event]
      xfs:xfs_attr_rmtval_set                            [Tracepoint event]
      xfs:xfs_attr_sf_add                                [Tracepoint event]
      xfs:xfs_attr_sf_addname                            [Tracepoint event]
      xfs:xfs_attr_sf_create                             [Tracepoint event]
      xfs:xfs_attr_sf_lookup                             [Tracepoint event]
      xfs:xfs_attr_sf_remove                             [Tracepoint event]
      xfs:xfs_attr_sf_to_leaf                            [Tracepoint event]
      xfs:xfs_bmap_defer                                 [Tracepoint event]
      xfs:xfs_bmap_deferred                              [Tracepoint event]
      xfs:xfs_bmap_free_defer                            [Tracepoint event]
      xfs:xfs_bmap_free_deferred                         [Tracepoint event]
      xfs:xfs_bmap_post_update                           [Tracepoint event]
      xfs:xfs_bmap_pre_update                            [Tracepoint event]
      xfs:xfs_btree_corrupt                              [Tracepoint event]
      xfs:xfs_btree_overlapped_query_range               [Tracepoint event]
      xfs:xfs_btree_updkeys                              [Tracepoint event]
      xfs:xfs_buf_delwri_pushbuf                         [Tracepoint event]
      xfs:xfs_buf_delwri_queue                           [Tracepoint event]
      xfs:xfs_buf_delwri_queued                          [Tracepoint event]
      xfs:xfs_buf_delwri_split                           [Tracepoint event]
      xfs:xfs_buf_error_relse                            [Tracepoint event]
      xfs:xfs_buf_find                                   [Tracepoint event]
      xfs:xfs_buf_free                                   [Tracepoint event]
      xfs:xfs_buf_get                                    [Tracepoint event]
      xfs:xfs_buf_get_uncached                           [Tracepoint event]
      xfs:xfs_buf_hold                                   [Tracepoint event]
      xfs:xfs_buf_init                                   [Tracepoint event]
      xfs:xfs_buf_iodone                                 [Tracepoint event]
      xfs:xfs_buf_ioerror                                [Tracepoint event]
      xfs:xfs_buf_iowait                                 [Tracepoint event]
      xfs:xfs_buf_iowait_done                            [Tracepoint event]
      xfs:xfs_buf_item_committed                         [Tracepoint event]
      xfs:xfs_buf_item_format                            [Tracepoint event]
      xfs:xfs_buf_item_format_stale                      [Tracepoint event]
      xfs:xfs_buf_item_iodone_async                      [Tracepoint event]
      xfs:xfs_buf_item_ordered                           [Tracepoint event]
      xfs:xfs_buf_item_pin                               [Tracepoint event]
      xfs:xfs_buf_item_push                              [Tracepoint event]
      xfs:xfs_buf_item_release                           [Tracepoint event]
      xfs:xfs_buf_item_relse                             [Tracepoint event]
      xfs:xfs_buf_item_size                              [Tracepoint event]
      xfs:xfs_buf_item_size_ordered                      [Tracepoint event]
      xfs:xfs_buf_item_size_stale                        [Tracepoint event]
      xfs:xfs_buf_item_unpin                             [Tracepoint event]
      xfs:xfs_buf_item_unpin_stale                       [Tracepoint event]
      xfs:xfs_buf_lock                                   [Tracepoint event]
      xfs:xfs_buf_lock_done                              [Tracepoint event]
      xfs:xfs_buf_read                                   [Tracepoint event]
      xfs:xfs_buf_rele                                   [Tracepoint event]
      xfs:xfs_buf_submit                                 [Tracepoint event]
      xfs:xfs_buf_trylock                                [Tracepoint event]
      xfs:xfs_buf_trylock_fail                           [Tracepoint event]
      xfs:xfs_buf_unlock                                 [Tracepoint event]
      xfs:xfs_buf_wait_buftarg                           [Tracepoint event]
      xfs:xfs_bunmap                                     [Tracepoint event]
      xfs:xfs_collapse_file_space                        [Tracepoint event]
      xfs:xfs_create                                     [Tracepoint event]
      xfs:xfs_da_fixhashpath                             [Tracepoint event]
      xfs:xfs_da_grow_inode                              [Tracepoint event]
      xfs:xfs_da_join                                    [Tracepoint event]
      xfs:xfs_da_link_after                              [Tracepoint event]
      xfs:xfs_da_link_before                             [Tracepoint event]
      xfs:xfs_da_node_add                                [Tracepoint event]
      xfs:xfs_da_node_create                             [Tracepoint event]
      xfs:xfs_da_node_rebalance                          [Tracepoint event]
      xfs:xfs_da_node_remove                             [Tracepoint event]
      xfs:xfs_da_node_split                              [Tracepoint event]
      xfs:xfs_da_node_toosmall                           [Tracepoint event]
      xfs:xfs_da_node_unbalance                          [Tracepoint event]
      xfs:xfs_da_path_shift                              [Tracepoint event]
      xfs:xfs_da_root_join                               [Tracepoint event]
      xfs:xfs_da_root_split                              [Tracepoint event]
      xfs:xfs_da_shrink_inode                            [Tracepoint event]
      xfs:xfs_da_split                                   [Tracepoint event]
      xfs:xfs_da_swap_lastblock                          [Tracepoint event]
      xfs:xfs_da_unlink_back                             [Tracepoint event]
      xfs:xfs_da_unlink_forward                          [Tracepoint event]
      xfs:xfs_defer_cancel                               [Tracepoint event]
      xfs:xfs_defer_cancel_list                          [Tracepoint event]
      xfs:xfs_defer_create_intent                        [Tracepoint event]
      xfs:xfs_defer_finish                               [Tracepoint event]
      xfs:xfs_defer_finish_done                          [Tracepoint event]
      xfs:xfs_defer_finish_error                         [Tracepoint event]
      xfs:xfs_defer_pending_abort                        [Tracepoint event]
      xfs:xfs_defer_pending_finish                       [Tracepoint event]
      xfs:xfs_defer_trans_abort                          [Tracepoint event]
      xfs:xfs_defer_trans_roll                           [Tracepoint event]
      xfs:xfs_defer_trans_roll_error                     [Tracepoint event]
      xfs:xfs_delalloc_enospc                            [Tracepoint event]
      xfs:xfs_destroy_inode                              [Tracepoint event]
      xfs:xfs_dir2_block_addname                         [Tracepoint event]
      xfs:xfs_dir2_block_lookup                          [Tracepoint event]
      xfs:xfs_dir2_block_removename                      [Tracepoint event]
      xfs:xfs_dir2_block_replace                         [Tracepoint event]
      xfs:xfs_dir2_block_to_leaf                         [Tracepoint event]
      xfs:xfs_dir2_block_to_sf                           [Tracepoint event]
      xfs:xfs_dir2_grow_inode                            [Tracepoint event]
      xfs:xfs_dir2_leaf_addname                          [Tracepoint event]
      xfs:xfs_dir2_leaf_lookup                           [Tracepoint event]
      xfs:xfs_dir2_leaf_removename                       [Tracepoint event]
      xfs:xfs_dir2_leaf_replace                          [Tracepoint event]
      xfs:xfs_dir2_leaf_to_block                         [Tracepoint event]
      xfs:xfs_dir2_leaf_to_node                          [Tracepoint event]
      xfs:xfs_dir2_leafn_add                             [Tracepoint event]
      xfs:xfs_dir2_leafn_moveents                        [Tracepoint event]
      xfs:xfs_dir2_leafn_remove                          [Tracepoint event]
      xfs:xfs_dir2_node_addname                          [Tracepoint event]
      xfs:xfs_dir2_node_lookup                           [Tracepoint event]
      xfs:xfs_dir2_node_removename                       [Tracepoint event]
      xfs:xfs_dir2_node_replace                          [Tracepoint event]
      xfs:xfs_dir2_node_to_leaf                          [Tracepoint event]
      xfs:xfs_dir2_sf_addname                            [Tracepoint event]
      xfs:xfs_dir2_sf_create                             [Tracepoint event]
      xfs:xfs_dir2_sf_lookup                             [Tracepoint event]
      xfs:xfs_dir2_sf_removename                         [Tracepoint event]
      xfs:xfs_dir2_sf_replace                            [Tracepoint event]
      xfs:xfs_dir2_sf_to_block                           [Tracepoint event]
      xfs:xfs_dir2_sf_toino4                             [Tracepoint event]
      xfs:xfs_dir2_sf_toino8                             [Tracepoint event]
      xfs:xfs_dir2_shrink_inode                          [Tracepoint event]
      xfs:xfs_dir_fsync                                  [Tracepoint event]
      xfs:xfs_discard_busy                               [Tracepoint event]
      xfs:xfs_discard_exclude                            [Tracepoint event]
      xfs:xfs_discard_extent                             [Tracepoint event]
      xfs:xfs_discard_toosmall                           [Tracepoint event]
      xfs:xfs_dqadjust                                   [Tracepoint event]
      xfs:xfs_dqalloc                                    [Tracepoint event]
      xfs:xfs_dqattach_found                             [Tracepoint event]
      xfs:xfs_dqattach_get                               [Tracepoint event]
      xfs:xfs_dqflush                                    [Tracepoint event]
      xfs:xfs_dqflush_done                               [Tracepoint event]
      xfs:xfs_dqflush_force                              [Tracepoint event]
      xfs:xfs_dqget_dup                                  [Tracepoint event]
      xfs:xfs_dqget_freeing                              [Tracepoint event]
      xfs:xfs_dqget_hit                                  [Tracepoint event]
      xfs:xfs_dqget_miss                                 [Tracepoint event]
      xfs:xfs_dqput                                      [Tracepoint event]
      xfs:xfs_dqput_free                                 [Tracepoint event]
      xfs:xfs_dqread                                     [Tracepoint event]
      xfs:xfs_dqread_fail                                [Tracepoint event]
      xfs:xfs_dqreclaim_busy                             [Tracepoint event]
      xfs:xfs_dqreclaim_dirty                            [Tracepoint event]
      xfs:xfs_dqreclaim_done                             [Tracepoint event]
      xfs:xfs_dqreclaim_want                             [Tracepoint event]
      xfs:xfs_dqrele                                     [Tracepoint event]
      xfs:xfs_dqtobp_read                                [Tracepoint event]
      xfs:xfs_dquot_dqalloc                              [Tracepoint event]
      xfs:xfs_dquot_dqdetach                             [Tracepoint event]
      xfs:xfs_end_io_direct_write                        [Tracepoint event]
      xfs:xfs_end_io_direct_write_append                 [Tracepoint event]
      xfs:xfs_end_io_direct_write_unwritten              [Tracepoint event]
      xfs:xfs_extent_busy                                [Tracepoint event]
      xfs:xfs_extent_busy_clear                          [Tracepoint event]
      xfs:xfs_extent_busy_enomem                         [Tracepoint event]
      xfs:xfs_extent_busy_force                          [Tracepoint event]
      xfs:xfs_extent_busy_reuse                          [Tracepoint event]
      xfs:xfs_extent_busy_trim                           [Tracepoint event]
      xfs:xfs_file_buffered_read                         [Tracepoint event]
      xfs:xfs_file_buffered_write                        [Tracepoint event]
      xfs:xfs_file_compat_ioctl                          [Tracepoint event]
      xfs:xfs_file_dax_read                              [Tracepoint event]
      xfs:xfs_file_dax_write                             [Tracepoint event]
      xfs:xfs_file_direct_read                           [Tracepoint event]
      xfs:xfs_file_direct_write                          [Tracepoint event]
      xfs:xfs_file_fsync                                 [Tracepoint event]
      xfs:xfs_file_ioctl                                 [Tracepoint event]
      xfs:xfs_filemap_fault                              [Tracepoint event]
      xfs:xfs_filestream_free                            [Tracepoint event]
      xfs:xfs_filestream_lookup                          [Tracepoint event]
      xfs:xfs_filestream_pick                            [Tracepoint event]
      xfs:xfs_filestream_scan                            [Tracepoint event]
      xfs:xfs_free_extent                                [Tracepoint event]
      xfs:xfs_free_file_space                            [Tracepoint event]
      xfs:xfs_fs_mark_healthy                            [Tracepoint event]
      xfs:xfs_fs_mark_sick                               [Tracepoint event]
      xfs:xfs_fs_unfixed_corruption                      [Tracepoint event]
      xfs:xfs_fsmap_high_key                             [Tracepoint event]
      xfs:xfs_fsmap_low_key                              [Tracepoint event]
      xfs:xfs_fsmap_mapping                              [Tracepoint event]
      xfs:xfs_get_acl                                    [Tracepoint event]
      xfs:xfs_getattr                                    [Tracepoint event]
      xfs:xfs_getfsmap_high_key                          [Tracepoint event]
      xfs:xfs_getfsmap_low_key                           [Tracepoint event]
      xfs:xfs_getfsmap_mapping                           [Tracepoint event]
      xfs:xfs_ialloc_read_agi                            [Tracepoint event]
      xfs:xfs_iext_insert                                [Tracepoint event]
      xfs:xfs_iext_remove                                [Tracepoint event]
      xfs:xfs_iget_hit                                   [Tracepoint event]
      xfs:xfs_iget_miss                                  [Tracepoint event]
      xfs:xfs_iget_reclaim                               [Tracepoint event]
      xfs:xfs_iget_reclaim_fail                          [Tracepoint event]
      xfs:xfs_iget_skip                                  [Tracepoint event]
      xfs:xfs_ilock                                      [Tracepoint event]
      xfs:xfs_ilock_demote                               [Tracepoint event]
      xfs:xfs_ilock_nowait                               [Tracepoint event]
      xfs:xfs_inactive_symlink                           [Tracepoint event]
      xfs:xfs_inode_clear_cowblocks_tag                  [Tracepoint event]
      xfs:xfs_inode_clear_eofblocks_tag                  [Tracepoint event]
      xfs:xfs_inode_free_cowblocks_invalid               [Tracepoint event]
      xfs:xfs_inode_free_eofblocks_invalid               [Tracepoint event]
      xfs:xfs_inode_mark_healthy                         [Tracepoint event]
      xfs:xfs_inode_mark_sick                            [Tracepoint event]
      xfs:xfs_inode_pin                                  [Tracepoint event]
      xfs:xfs_inode_set_cowblocks_tag                    [Tracepoint event]
      xfs:xfs_inode_set_eofblocks_tag                    [Tracepoint event]
      xfs:xfs_inode_unpin                                [Tracepoint event]
      xfs:xfs_inode_unpin_nowait                         [Tracepoint event]
      xfs:xfs_insert_file_space                          [Tracepoint event]
      xfs:xfs_invalidatepage                             [Tracepoint event]
      xfs:xfs_ioctl_clone                                [Tracepoint event]
      xfs:xfs_ioctl_setattr                              [Tracepoint event]
      xfs:xfs_iomap_alloc                                [Tracepoint event]
      xfs:xfs_iomap_found                                [Tracepoint event]
      xfs:xfs_iomap_prealloc_size                        [Tracepoint event]
      xfs:xfs_irec_merge_post                            [Tracepoint event]
      xfs:xfs_irec_merge_pre                             [Tracepoint event]
      xfs:xfs_irele                                      [Tracepoint event]
      xfs:xfs_itruncate_extents_end                      [Tracepoint event]
      xfs:xfs_itruncate_extents_start                    [Tracepoint event]
      xfs:xfs_iunlink                                    [Tracepoint event]
      xfs:xfs_iunlink_map_prev_fallback                  [Tracepoint event]
      xfs:xfs_iunlink_remove                             [Tracepoint event]
      xfs:xfs_iunlink_update_bucket                      [Tracepoint event]
      xfs:xfs_iunlink_update_dinode                      [Tracepoint event]
      xfs:xfs_iunlock                                    [Tracepoint event]
      xfs:xfs_iwalk_ag                                   [Tracepoint event]
      xfs:xfs_iwalk_ag_rec                               [Tracepoint event]
      xfs:xfs_link                                       [Tracepoint event]
      xfs:xfs_log_assign_tail_lsn                        [Tracepoint event]
      xfs:xfs_log_done_nonperm                           [Tracepoint event]
      xfs:xfs_log_done_perm                              [Tracepoint event]
      xfs:xfs_log_force                                  [Tracepoint event]
      xfs:xfs_log_grant_sleep                            [Tracepoint event]
      xfs:xfs_log_grant_wake                             [Tracepoint event]
      xfs:xfs_log_grant_wake_up                          [Tracepoint event]
      xfs:xfs_log_recover                                [Tracepoint event]
      xfs:xfs_log_recover_buf_cancel                     [Tracepoint event]
      xfs:xfs_log_recover_buf_cancel_add                 [Tracepoint event]
      xfs:xfs_log_recover_buf_cancel_ref_inc             [Tracepoint event]
      xfs:xfs_log_recover_buf_dquot_buf                  [Tracepoint event]
      xfs:xfs_log_recover_buf_inode_buf                  [Tracepoint event]
      xfs:xfs_log_recover_buf_not_cancel                 [Tracepoint event]
      xfs:xfs_log_recover_buf_recover                    [Tracepoint event]
      xfs:xfs_log_recover_buf_reg_buf                    [Tracepoint event]
      xfs:xfs_log_recover_buf_skip                       [Tracepoint event]
      xfs:xfs_log_recover_icreate_cancel                 [Tracepoint event]
      xfs:xfs_log_recover_icreate_recover                [Tracepoint event]
      xfs:xfs_log_recover_inode_cancel                   [Tracepoint event]
      xfs:xfs_log_recover_inode_recover                  [Tracepoint event]
      xfs:xfs_log_recover_inode_skip                     [Tracepoint event]
      xfs:xfs_log_recover_item_add                       [Tracepoint event]
      xfs:xfs_log_recover_item_add_cont                  [Tracepoint event]
      xfs:xfs_log_recover_item_recover                   [Tracepoint event]
      xfs:xfs_log_recover_item_reorder_head              [Tracepoint event]
      xfs:xfs_log_recover_item_reorder_tail              [Tracepoint event]
      xfs:xfs_log_recover_record                         [Tracepoint event]
      xfs:xfs_log_regrant                                [Tracepoint event]
      xfs:xfs_log_regrant_exit                           [Tracepoint event]
      xfs:xfs_log_regrant_reserve_enter                  [Tracepoint event]
      xfs:xfs_log_regrant_reserve_exit                   [Tracepoint event]
      xfs:xfs_log_regrant_reserve_sub                    [Tracepoint event]
      xfs:xfs_log_reserve                                [Tracepoint event]
      xfs:xfs_log_reserve_exit                           [Tracepoint event]
      xfs:xfs_log_umount_write                           [Tracepoint event]
      xfs:xfs_log_ungrant_enter                          [Tracepoint event]
      xfs:xfs_log_ungrant_exit                           [Tracepoint event]
      xfs:xfs_log_ungrant_sub                            [Tracepoint event]
      xfs:xfs_lookup                                     [Tracepoint event]
      xfs:xfs_map_blocks_alloc                           [Tracepoint event]
      xfs:xfs_map_blocks_found                           [Tracepoint event]
      xfs:xfs_pagecache_inval                            [Tracepoint event]
      xfs:xfs_perag_clear_cowblocks                      [Tracepoint event]
      xfs:xfs_perag_clear_eofblocks                      [Tracepoint event]
      xfs:xfs_perag_clear_reclaim                        [Tracepoint event]
      xfs:xfs_perag_get                                  [Tracepoint event]
      xfs:xfs_perag_get_tag                              [Tracepoint event]
      xfs:xfs_perag_put                                  [Tracepoint event]
      xfs:xfs_perag_set_cowblocks                        [Tracepoint event]
      xfs:xfs_perag_set_eofblocks                        [Tracepoint event]
      xfs:xfs_perag_set_reclaim                          [Tracepoint event]
      xfs:xfs_pwork_init                                 [Tracepoint event]
      xfs:xfs_read_agf                                   [Tracepoint event]
      xfs:xfs_read_agi                                   [Tracepoint event]
      xfs:xfs_read_extent                                [Tracepoint event]
      xfs:xfs_readdir                                    [Tracepoint event]
      xfs:xfs_readlink                                   [Tracepoint event]
      xfs:xfs_refcount_adjust_cow_error                  [Tracepoint event]
      xfs:xfs_refcount_adjust_error                      [Tracepoint event]
      xfs:xfs_refcount_cow_decrease                      [Tracepoint event]
      xfs:xfs_refcount_cow_increase                      [Tracepoint event]
      xfs:xfs_refcount_decrease                          [Tracepoint event]
      xfs:xfs_refcount_defer                             [Tracepoint event]
      xfs:xfs_refcount_deferred                          [Tracepoint event]
      xfs:xfs_refcount_delete                            [Tracepoint event]
      xfs:xfs_refcount_delete_error                      [Tracepoint event]
      xfs:xfs_refcount_find_left_extent                  [Tracepoint event]
      xfs:xfs_refcount_find_left_extent_error            [Tracepoint event]
      xfs:xfs_refcount_find_right_extent                 [Tracepoint event]
      xfs:xfs_refcount_find_right_extent_error           [Tracepoint event]
      xfs:xfs_refcount_find_shared                       [Tracepoint event]
      xfs:xfs_refcount_find_shared_error                 [Tracepoint event]
      xfs:xfs_refcount_find_shared_result                [Tracepoint event]
      xfs:xfs_refcount_finish_one_leftover               [Tracepoint event]
      xfs:xfs_refcount_get                               [Tracepoint event]
      xfs:xfs_refcount_increase                          [Tracepoint event]
      xfs:xfs_refcount_insert                            [Tracepoint event]
      xfs:xfs_refcount_insert_error                      [Tracepoint event]
      xfs:xfs_refcount_lookup                            [Tracepoint event]
      xfs:xfs_refcount_merge_center_extents              [Tracepoint event]
      xfs:xfs_refcount_merge_center_extents_error        [Tracepoint event]
      xfs:xfs_refcount_merge_left_extent                 [Tracepoint event]
      xfs:xfs_refcount_merge_left_extent_error           [Tracepoint event]
      xfs:xfs_refcount_merge_right_extent                [Tracepoint event]
      xfs:xfs_refcount_merge_right_extent_error          [Tracepoint event]
      xfs:xfs_refcount_modify_extent                     [Tracepoint event]
      xfs:xfs_refcount_modify_extent_error               [Tracepoint event]
      xfs:xfs_refcount_recover_extent                    [Tracepoint event]
      xfs:xfs_refcount_split_extent                      [Tracepoint event]
      xfs:xfs_refcount_split_extent_error                [Tracepoint event]
      xfs:xfs_refcount_update                            [Tracepoint event]
      xfs:xfs_refcount_update_error                      [Tracepoint event]
      xfs:xfs_refcountbt_alloc_block                     [Tracepoint event]
      xfs:xfs_refcountbt_free_block                      [Tracepoint event]
      xfs:xfs_reflink_bounce_dio_write                   [Tracepoint event]
      xfs:xfs_reflink_cancel_cow                         [Tracepoint event]
      xfs:xfs_reflink_cancel_cow_range                   [Tracepoint event]
      xfs:xfs_reflink_cancel_cow_range_error             [Tracepoint event]
      xfs:xfs_reflink_compare_extents                    [Tracepoint event]
      xfs:xfs_reflink_compare_extents_error              [Tracepoint event]
      xfs:xfs_reflink_convert_cow                        [Tracepoint event]
      xfs:xfs_reflink_cow_enospc                         [Tracepoint event]
      xfs:xfs_reflink_cow_found                          [Tracepoint event]
      xfs:xfs_reflink_cow_remap                          [Tracepoint event]
      xfs:xfs_reflink_end_cow                            [Tracepoint event]
      xfs:xfs_reflink_end_cow_error                      [Tracepoint event]
      xfs:xfs_reflink_punch_range                        [Tracepoint event]
      xfs:xfs_reflink_remap                              [Tracepoint event]
      xfs:xfs_reflink_remap_blocks_error                 [Tracepoint event]
      xfs:xfs_reflink_remap_blocks_loop                  [Tracepoint event]
      xfs:xfs_reflink_remap_extent_error                 [Tracepoint event]
      xfs:xfs_reflink_remap_imap                         [Tracepoint event]
      xfs:xfs_reflink_remap_range                        [Tracepoint event]
      xfs:xfs_reflink_remap_range_error                  [Tracepoint event]
      xfs:xfs_reflink_set_inode_flag                     [Tracepoint event]
      xfs:xfs_reflink_set_inode_flag_error               [Tracepoint event]
      xfs:xfs_reflink_trim_around_shared                 [Tracepoint event]
      xfs:xfs_reflink_unset_inode_flag                   [Tracepoint event]
      xfs:xfs_reflink_unshare                            [Tracepoint event]
      xfs:xfs_reflink_unshare_error                      [Tracepoint event]
      xfs:xfs_reflink_update_inode_size                  [Tracepoint event]
      xfs:xfs_reflink_update_inode_size_error            [Tracepoint event]
      xfs:xfs_releasepage                                [Tracepoint event]
      xfs:xfs_remove                                     [Tracepoint event]
      xfs:xfs_rename                                     [Tracepoint event]
      xfs:xfs_reset_dqcounts                             [Tracepoint event]
      xfs:xfs_rmap_convert                               [Tracepoint event]
      xfs:xfs_rmap_convert_done                          [Tracepoint event]
      xfs:xfs_rmap_convert_error                         [Tracepoint event]
      xfs:xfs_rmap_convert_state                         [Tracepoint event]
      xfs:xfs_rmap_defer                                 [Tracepoint event]
      xfs:xfs_rmap_deferred                              [Tracepoint event]
      xfs:xfs_rmap_delete                                [Tracepoint event]
      xfs:xfs_rmap_delete_error                          [Tracepoint event]
      xfs:xfs_rmap_find_left_neighbor_candidate          [Tracepoint event]
      xfs:xfs_rmap_find_left_neighbor_query              [Tracepoint event]
      xfs:xfs_rmap_find_left_neighbor_result             [Tracepoint event]
      xfs:xfs_rmap_find_right_neighbor_result            [Tracepoint event]
      xfs:xfs_rmap_insert                                [Tracepoint event]
      xfs:xfs_rmap_insert_error                          [Tracepoint event]
      xfs:xfs_rmap_lookup_le_range                       [Tracepoint event]
      xfs:xfs_rmap_lookup_le_range_candidate             [Tracepoint event]
      xfs:xfs_rmap_lookup_le_range_result                [Tracepoint event]
      xfs:xfs_rmap_map                                   [Tracepoint event]
      xfs:xfs_rmap_map_done                              [Tracepoint event]
      xfs:xfs_rmap_map_error                             [Tracepoint event]
      xfs:xfs_rmap_unmap                                 [Tracepoint event]
      xfs:xfs_rmap_unmap_done                            [Tracepoint event]
      xfs:xfs_rmap_unmap_error                           [Tracepoint event]
      xfs:xfs_rmap_update                                [Tracepoint event]
      xfs:xfs_rmap_update_error                          [Tracepoint event]
      xfs:xfs_rmapbt_alloc_block                         [Tracepoint event]
      xfs:xfs_rmapbt_free_block                          [Tracepoint event]
      xfs:xfs_rt_mark_healthy                            [Tracepoint event]
      xfs:xfs_rt_mark_sick                               [Tracepoint event]
      xfs:xfs_rt_unfixed_corruption                      [Tracepoint event]
      xfs:xfs_setattr                                    [Tracepoint event]
      xfs:xfs_setfilesize                                [Tracepoint event]
      xfs:xfs_swap_extent_after                          [Tracepoint event]
      xfs:xfs_swap_extent_before                         [Tracepoint event]
      xfs:xfs_swap_extent_rmap_error                     [Tracepoint event]
      xfs:xfs_swap_extent_rmap_remap                     [Tracepoint event]
      xfs:xfs_swap_extent_rmap_remap_piece               [Tracepoint event]
      xfs:xfs_symlink                                    [Tracepoint event]
      xfs:xfs_trans_add_item                             [Tracepoint event]
      xfs:xfs_trans_alloc                                [Tracepoint event]
      xfs:xfs_trans_bhold                                [Tracepoint event]
      xfs:xfs_trans_bhold_release                        [Tracepoint event]
      xfs:xfs_trans_binval                               [Tracepoint event]
      xfs:xfs_trans_bjoin                                [Tracepoint event]
      xfs:xfs_trans_brelse                               [Tracepoint event]
      xfs:xfs_trans_cancel                               [Tracepoint event]
      xfs:xfs_trans_commit                               [Tracepoint event]
      xfs:xfs_trans_commit_items                         [Tracepoint event]
      xfs:xfs_trans_dup                                  [Tracepoint event]
      xfs:xfs_trans_free                                 [Tracepoint event]
      xfs:xfs_trans_free_items                           [Tracepoint event]
      xfs:xfs_trans_get_buf                              [Tracepoint event]
      xfs:xfs_trans_get_buf_recur                        [Tracepoint event]
      xfs:xfs_trans_getsb                                [Tracepoint event]
      xfs:xfs_trans_getsb_recur                          [Tracepoint event]
      xfs:xfs_trans_log_buf                              [Tracepoint event]
      xfs:xfs_trans_read_buf                             [Tracepoint event]
      xfs:xfs_trans_read_buf_recur                       [Tracepoint event]
      xfs:xfs_trans_read_buf_shut                        [Tracepoint event]
      xfs:xfs_trans_resv_calc                            [Tracepoint event]
      xfs:xfs_trans_roll                                 [Tracepoint event]
      xfs:xfs_unwritten_convert                          [Tracepoint event]
      xfs:xfs_update_time                                [Tracepoint event]
      xfs:xfs_vm_bmap                                    [Tracepoint event]
      xfs:xfs_vm_readpage                                [Tracepoint event]
      xfs:xfs_vm_readpages                               [Tracepoint event]
      xfs:xfs_write_extent                               [Tracepoint event]
      xfs:xfs_writepage                                  [Tracepoint event]
      xfs:xfs_zero_eof                                   [Tracepoint event]
      xfs:xfs_zero_file_space                            [Tracepoint event]
      xhci-hcd:xhci_add_endpoint                         [Tracepoint event]
      xhci-hcd:xhci_address_ctrl_ctx                     [Tracepoint event]
      xhci-hcd:xhci_address_ctx                          [Tracepoint event]
      xhci-hcd:xhci_alloc_dev                            [Tracepoint event]
      xhci-hcd:xhci_alloc_virt_device                    [Tracepoint event]
      xhci-hcd:xhci_configure_endpoint                   [Tracepoint event]
      xhci-hcd:xhci_configure_endpoint_ctrl_ctx          [Tracepoint event]
      xhci-hcd:xhci_dbc_alloc_request                    [Tracepoint event]
      xhci-hcd:xhci_dbc_free_request                     [Tracepoint event]
      xhci-hcd:xhci_dbc_gadget_ep_queue                  [Tracepoint event]
      xhci-hcd:xhci_dbc_giveback_request                 [Tracepoint event]
      xhci-hcd:xhci_dbc_handle_event                     [Tracepoint event]
      xhci-hcd:xhci_dbc_handle_transfer                  [Tracepoint event]
      xhci-hcd:xhci_dbc_queue_request                    [Tracepoint event]
      xhci-hcd:xhci_dbg_address                          [Tracepoint event]
      xhci-hcd:xhci_dbg_cancel_urb                       [Tracepoint event]
      xhci-hcd:xhci_dbg_context_change                   [Tracepoint event]
      xhci-hcd:xhci_dbg_init                             [Tracepoint event]
      xhci-hcd:xhci_dbg_quirks                           [Tracepoint event]
      xhci-hcd:xhci_dbg_reset_ep                         [Tracepoint event]
      xhci-hcd:xhci_dbg_ring_expansion                   [Tracepoint event]
      xhci-hcd:xhci_discover_or_reset_device             [Tracepoint event]
      xhci-hcd:xhci_free_dev                             [Tracepoint event]
      xhci-hcd:xhci_free_virt_device                     [Tracepoint event]
      xhci-hcd:xhci_get_port_status                      [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_addr_dev                  [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_config_ep                 [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_disable_slot              [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_reset_dev                 [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_reset_ep                  [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_set_deq                   [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_set_deq_ep                [Tracepoint event]
      xhci-hcd:xhci_handle_cmd_stop_ep                   [Tracepoint event]
      xhci-hcd:xhci_handle_command                       [Tracepoint event]
      xhci-hcd:xhci_handle_event                         [Tracepoint event]
      xhci-hcd:xhci_handle_port_status                   [Tracepoint event]
      xhci-hcd:xhci_handle_transfer                      [Tracepoint event]
      xhci-hcd:xhci_hub_status_data                      [Tracepoint event]
      xhci-hcd:xhci_inc_deq                              [Tracepoint event]
      xhci-hcd:xhci_inc_enq                              [Tracepoint event]
      xhci-hcd:xhci_queue_trb                            [Tracepoint event]
      xhci-hcd:xhci_ring_alloc                           [Tracepoint event]
      xhci-hcd:xhci_ring_expansion                       [Tracepoint event]
      xhci-hcd:xhci_ring_free                            [Tracepoint event]
      xhci-hcd:xhci_setup_addressable_virt_device        [Tracepoint event]
      xhci-hcd:xhci_setup_device                         [Tracepoint event]
      xhci-hcd:xhci_setup_device_slot                    [Tracepoint event]
      xhci-hcd:xhci_stop_device                          [Tracepoint event]
      xhci-hcd:xhci_urb_dequeue                          [Tracepoint event]
      xhci-hcd:xhci_urb_enqueue                          [Tracepoint event]
      xhci-hcd:xhci_urb_giveback                         [Tracepoint event]
    
    Metric Groups:
    
    Branch_Mispredicts:
      IpMispredict
           [Number of Instructions per non-speculative Branch Misprediction (JEClear)]
    Branches:
      BpTB
           [Branch instructions per taken branch]
      IpB
           [Instructions per Branch]
      IpCall
           [Instruction per (near) call]
      IpTB
           [Instruction per taken branch]
    Cache_Misses:
      L1MPKI
           [L1 cache true misses per kilo instruction for retired demand loads]
      L2HPKI_All
           [L2 cache hits per kilo instruction for all request types (including speculative)]
      L2MPKI
           [L2 cache true misses per kilo instruction for retired demand loads]
      L2MPKI_All
           [L2 cache misses per kilo instruction for all request types (including speculative)]
      L3MPKI
           [L3 cache true misses per kilo instruction for retired demand loads]
    DSB:
      DSB_Coverage
           [Fraction of Uops delivered by the DSB (aka Decoded ICache; or Uop Cache)]
    Frontend_Bandwidth:
      DSB_Coverage
           [Fraction of Uops delivered by the DSB (aka Decoded ICache; or Uop Cache)]
    Instruction_Type:
      IpB
           [Instructions per Branch]
      IpL
           [Instructions per Load (lower number means loads are more frequent)]
      IpS
           [Instructions per Store]
    L1_Bound:
      IpL
           [Instructions per Load (lower number means loads are more frequent)]
    Memory_BW:
      DRAM_BW_Use
           [Average external Memory Bandwidth Use for reads and writes [GB / sec]]
      DRAM_Parallel_Reads
           [Average number of parallel data read requests to external memory. Accounts for demand loads and L1/L2 prefetches]
      L1D_Cache_Fill_BW
           [Average data fill bandwidth to the L1 data cache [GB / sec]]
      L2_Cache_Fill_BW
           [Average data fill bandwidth to the L2 cache [GB / sec]]
      L3_Cache_Fill_BW
           [Average per-core data fill bandwidth to the L3 cache [GB / sec]]
      MLP
           [Memory-Level-Parallelism (average number of L1 miss demand load when there is at least one such miss. Per-thread)]
    Memory_Bound:
      Load_Miss_Real_Latency
           [Actual Average Latency for L1 data-cache miss demand loads (in core cycles)]
      MLP
           [Memory-Level-Parallelism (average number of L1 miss demand load when there is at least one such miss. Per-thread)]
    Memory_Lat:
      DRAM_Read_Latency
           [Average latency of data read request to external memory (in nanoseconds). Accounts for demand loads and L1/L2 prefetches]
      Load_Miss_Real_Latency
           [Actual Average Latency for L1 data-cache miss demand loads (in core cycles)]
    No_group:
      L1MPKI
           [L1 cache true misses per kilo instruction for retired demand loads]
      L2HPKI_All
           [L2 cache hits per kilo instruction for all request types (including speculative)]
      L2MPKI
           [L2 cache true misses per kilo instruction for retired demand loads]
      L2MPKI_All
           [L2 cache misses per kilo instruction for all request types (including speculative)]
      L3MPKI
           [L3 cache true misses per kilo instruction for retired demand loads]
      Socket_CLKS
           [Socket actual clocks when any core is active on that socket]
    PGO:
      BpTB
           [Branch instructions per taken branch]
      IFetch_Line_Utilization
           [Rough Estimation of fraction of fetched lines bytes that were likely (includes speculatively fetches) consumed by program instructions]
      IpTB
           [Instruction per taken branch]
    Pipeline:
      CPI
           [Cycles Per Instruction (threaded)]
      ILP
           [Instruction-Level-Parallelism (average number of uops executed when there is at least 1 uop executed)]
      UPI
           [Uops Per Instruction]
    Port_5:
      IpB
           [Instructions per Branch]
    Port_6:
      IpB
           [Instructions per Branch]
    Ports_Utilization:
      ILP
           [Instruction-Level-Parallelism (average number of uops executed when there is at least 1 uop executed)]
    Power:
      C2_Pkg_Residency
           [C2 residency percent per package]
      C3_Core_Residency
           [C3 residency percent per core]
      C3_Pkg_Residency
           [C3 residency percent per package]
      C6_Core_Residency
           [C6 residency percent per core]
      C6_Pkg_Residency
           [C6 residency percent per package]
      C7_Core_Residency
           [C7 residency percent per core]
      C7_Pkg_Residency
           [C7 residency percent per package]
      Turbo_Utilization
           [Average Frequency Utilization relative nominal frequency]
    Retiring:
      UPI
           [Uops Per Instruction]
    SMT:
      CORE_CLKS
           [Core actual clocks when any thread is active on the physical core]
      CoreIPC
           [Instructions Per Cycle (per physical core)]
      CoreIPC_SMT
           [Instructions Per Cycle (per physical core)]
      SMT_2T_Utilization
           [Fraction of cycles where both hardware threads were active]
    Store_Bound:
      IpS
           [Instructions per Store]
    Summary:
      CLKS
           [Per-thread actual clocks when the logical processor is active]
      CPI
           [Cycles Per Instruction (threaded)]
      CPU_Utilization
           [Average CPU Utilization]
      Instructions
           [Total number of retired Instructions]
      Kernel_Utilization
           [Fraction of cycles spent in Kernel mode]
      SMT_2T_Utilization
           [Fraction of cycles where both hardware threads were active]
    TLB:
      Page_Walks_Utilization
           [Utilization of the core's Page Walker(s) serving STLB misses triggered by instruction/Load/Store accesses]
    TLB_SMT:
      Page_Walks_Utilization_SMT
           [Utilization of the core's Page Walker(s) serving STLB misses triggered by instruction/Load/Store accesses]
    TopDownL1:
      IPC
           [Instructions Per Cycle (per logical thread)]
      SLOTS
           [Total issue-pipeline slots (per core)]
    TopDownL1_SMT:
      SLOTS_SMT
           [Total issue-pipeline slots (per core)]
    TopdownL1:
      Backend_Bound
           [This category represents fraction of slots where no uops are being delivered due to a lack of required resources for accepting new uops in the Backend]
      Bad_Speculation
           [This category represents fraction of slots wasted due to incorrect speculations]
      Frontend_Bound
           [This category represents fraction of slots where the processor's Frontend undersupplies its Backend]
      Retiring
           [This category represents fraction of slots utilized by useful work i.e. issued uops that eventually get retired]
    TopdownL1_SMT:
      Backend_Bound_SMT
           [This category represents fraction of slots where no uops are being delivered due to a lack of required resources for accepting new uops in the Backend. SMT version; use when SMT is enabled and measuring per logical CPU]
      Bad_Speculation_SMT
           [This category represents fraction of slots wasted due to incorrect speculations. SMT version; use when SMT is enabled and measuring per logical CPU]
      Frontend_Bound_SMT
           [This category represents fraction of slots where the processor's Frontend undersupplies its Backend. SMT version; use when SMT is enabled and measuring per logical CPU]
      Retiring_SMT
           [This category represents fraction of slots utilized by useful work i.e. issued uops that eventually get retired. SMT version; use when SMT is enabled and measuring per logical CPU]


The above only lists the _named_ events. Typically there are hundreds more on the system. We will see how
to access those other events later in the tutorial. 

To get the number of loads and stores that go to the last-level cache (LLC) we can use the following
command with the `--event` option


```bash
perf stat --event LLC-loads,LLC-stores ./matvec 2000 200
```

    522738635.7446
    
     Performance counter stats for './matvec 2000 200':
    
             3,118,067      LLC-loads                                                   
                88,376      LLC-stores                                                  
    
           2.379943299 seconds time elapsed
    
           2.367897000 seconds user
           0.011999000 seconds sys
    
    


**Do we have any new insight about the performance of matvec?**

### <a name="norm_perf"></a>Normalized performance

Execution time is not always a good measure of performance. Programs that execute more instructions
or those that process more data will have a longer execution time. That does not necessarily mean
that its performance is any worse that another program that executes fewer instructions. 

To get a better understanding of performance, we need a way to _normalize_ performance across
programs. One way to do this is to use a _throughput_ metrics. Throughput metrics measure
performance as a rate of _something_. For example, operating system performance may be measured in
number of tasks completed. FLOPs/sec counts the number of completed floating-operations per
second. This is the _de facto_ performance metric for HPC applications. This metric is also used to
rank the fastest supercomputers in the world by [top500.org](https://www.top500.org/). 

**How do we measure the FLOPS for `matvec`?**

We need to first find the event that corresponds to the execution of a floating-point
operation. This is not a named event. So we will need to dig up the hex-code. The code for the FP
event is `r538010`. We can now count the number of FP operations with perf


```bash
perf stat -e r538010 ./matvec 2000 200
```

    522738635.7446
    
     Performance counter stats for './matvec 2000 200':
    
                     0      r538010                                                     
    
           2.379882309 seconds time elapsed
    
           2.367778000 seconds user
           0.012019000 seconds sys
    
    


`perf` will not give you the FLOPS directly but we can write a short script to calculate it.


```bash
# dump perf out to tmp file 
perf stat -e r538010 ./matvec 2000 200 2> tmp.prof
       
# extract flop count
flops=`cat tmp.prof | grep r538010  | awk '{print $1}' | sed 's/,//g'`
     
# extract number of seconds 
secs=`cat tmp.prof | grep "elapsed"  | awk '{print $1}'`
  
# calculate FLOPS
FLOPS=`echo $flops $secs | awk '{printf "%3.2f", ($1/1e+09)/$2}'`
```

    522738635.7446
    0



```bash
#print the result
echo "Performance = $FLOPS GFLOPS/s"
```

    Performance = 0.00 GFLOPS/s



```bash

```
