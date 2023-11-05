Design a fast integer/floating-point parser for a memory-mapped file.

> **Note**
> You can just copy `main.sh` to your system and run it. \
> For the code, refer to `main.cxx`.

<br>

```bash
$ ./main.sh
# OMP_NUM_THREADS=64
# Finding byte sum of file /home/graphwork/Data/indochina-2004.mtx ...
# {adv=0, block=4096, mode=0} -> {0000532.8ms, sum=148985827434} byteSum
#
# OMP_NUM_THREADS=64
# Finding byte sum of file /home/graphwork/Data/uk-2002.mtx ...
# {adv=0, block=4096, mode=0} -> {0000880.9ms, sum=244964049087} byteSum
#
# ...
```

<br>

I tried a simple file bytes sum using `mmap()` - both sequential and parallel using openmp (64 threads on DGX). I adjust `madvise()`, `mmap()`, and per-thread block size to see which access pattern has the best performance. It seems to me using an early `madvice(MADV_WILLNEED)`, a per-thread block size of `256K` (`dynamic` schedule) is good. Below is a plot showing the time taken with this config for each graph.

[![](https://i.imgur.com/7RTWQUE.png)][sheets]

In parallel doing a file byte sum takes `~100ms` on `indochina-2004` graph. Note that [PIGO] takes `~650ms` to load the same graphs as CSR. Next i measure the read bandwith of each file, simply by dividing the size of each file by the time taken.

[![](https://i.imgur.com/kv27toT.png)][sheets]

We appear to be reaching a peak bandwidth of `~35GB/s`. The [KIOXIA KCM6DRUL7T68 7.68TB NVMe SSD] installed on DGX has a peack sequential read performance of `62GB/s` (we are close). Sequential approach can hit a max of only `6GB/s`.

There is a paper called "Efficient Memory Mapped File I/O for In-Memory File Systems" on the topic - where Choi et al. working at Samsung say that `mmap()` is a good interface for fast IO (in contrast to file streams) and propose async map-ahead based `madvise()` for low-latency NVM storage devices. The also have a good slides on this - where their (modified) extended madvice obtains `~2.5x` better performance than default `mmap()` by minimizing the number of page faults.

<br>
<br>


## References

- [Efficient Memory Mapped File I/O for In-Memory File Systems](https://www.usenix.org/system/files/conference/hotstorage17/hotstorage17-paper-choi.pdf)
- [Efficient Memory Mapped File I/O for In-Memory File Systems - Slides](https://www.usenix.org/sites/default/files/conference/protected-files/hotstorage17_slides_choi.pdf)
- [DI-MMAP—a scalable memory-map runtime for out-of-core data-intensive applications](https://link.springer.com/article/10.1007/s10586-013-0309-0)
- [Kioxia CD6 7.68TB NVMe PCIe4x4 2.5" U.3 15mm SIE 1DWPD - KCD6XLUL7T68](https://smicro.eu/kioxia-cd6-7-68tb-nvmepcie4x4-2-5-15mm-sie-1dwpd-kcd6xlul7t68-1)
- [Kioxia KCD6XLUL7T68 - 7.68TB SSD NVMe 2.5-inch 15mm CD6-R Series, SIE, PCIe 4.0 6200 MB/sec Read, BiCS FLASH™ TLC, 1 DWPD](https://www.acmemicro.com/Product/17847/Kioxia-KCD6XLUL7T68---7-68TB-SSD-NVMe-2-5-inch-15mm-CD6-R-Series-SIE-PCIe-4-0-6200-MB-sec-Read-BiCS-FLASH-TLC-1-DWPD)
- [Is mmap + madvise really a form of async I/O?](https://stackoverflow.com/questions/31215250/is-mmap-madvise-really-a-form-of-async-i-o)
- [Is there really no asynchronous block I/O on Linux?](https://stackoverflow.com/questions/13407542/is-there-really-no-asynchronous-block-i-o-on-linux)
- [Linux mmap() with MAP_POPULATE, man page seems to give wrong info](https://stackoverflow.com/questions/23502361/linux-mmap-with-map-populate-man-page-seems-to-give-wrong-info)
- [When would one use mmap MAP_FIXED?](https://stackoverflow.com/questions/28575893/when-would-one-use-mmap-map-fixed)
- [Overlapping pages with mmap (MAP_FIXED)](https://stackoverflow.com/questions/14943990/overlapping-pages-with-mmap-map-fixed)
- [Zero a large memory mapping with `madvise`](https://stackoverflow.com/questions/18595123/zero-a-large-memory-mapping-with-madvise)
- [convert string to size_t](https://stackoverflow.com/questions/34043894/convert-string-to-size-t)
- [mmap(2) — Linux manual page](https://man7.org/linux/man-pages/man2/mmap.2.html)
- [madvise(2) — Linux manual page](https://man7.org/linux/man-pages/man2/madvise.2.html)
- [msync(2) — Linux manual page](https://man7.org/linux/man-pages/man2/msync.2.html)
- [mincore(2) — Linux manual page](https://man7.org/linux/man-pages/man2/mincore.2.html)
- [Reference - cstdlib - atol](https://cplusplus.com/reference/cstdlib/atol/)
- [Air gap (networking)](https://en.wikipedia.org/wiki/Air_gap_(networking))
- [Stuxnet](https://en.wikipedia.org/wiki/Stuxnet)

<br>
<br>


[![](https://img.youtube.com/vi/yqO7wVBTuLw/maxresdefault.jpg)](https://www.youtube.com/watch?v=yqO7wVBTuLw)<br>
[![ORG](https://img.shields.io/badge/org-puzzlef-green?logo=Org)](https://puzzlef.github.io)


[PIGO]: https://github.com/GT-TDAlab/PIGO
[KIOXIA KCM6DRUL7T68 7.68TB NVMe SSD]: https://www.acmemicro.com/Product/17847/Kioxia-KCD6XLUL7T68---7-68TB-SSD-NVMe-2-5-inch-15mm-CD6-R-Series-SIE-PCIe-4-0-6200-MB-sec-Read-BiCS-FLASH-TLC-1-DWPD
[sheets]: https://docs.google.com/spreadsheets/d/14EYpIVwZfwMRrm0vleclr0TzzDsUdQztbG6CMDQQg_Q/edit?usp=sharing
