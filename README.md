Design of high-performance OpenMP-based parallel Graph CSR loader.

High-performance graph processing frameworks like [Gunrock], [Hornet], [Ligra], and [Galois] help accelerate graph analytics tasks. However, graph loading is a major bottleneck in these frameworks. Fast graph loading is crucial for improving response time and reducing system/cloud usage charges. To address this, we introduce **GVEL**, a highly optimized method for *reading Edgelists* from text files and *converting them into Compressed Sparse Row (CSR) format*.

Below we plot the time taken by Hornet, Gunrock, [PIGO], and GVEL for reading Edgelist and converting it to CSR on 13 different graphs. PIGO and GVEL are not visible on this scale - they are significantly faster than Hornet and Gunrock. The graph loading time for Hornet is not shown for `uk-2002`, `it-2004`, and `sk-2005` graphs as it crashed while loading. GVEL surpasses Hornet, Gunrock, and PIGO in CSR reading by `78×`, `112×`, and `1.8×`, respectively.

[![](https://i.imgur.com/MhLmthh.png)][sheets-compare]

Below we plot only the time taken by PIGO and GVEL for reading Edgelist and converting it to CSR.

[![](https://i.imgur.com/bCPxAhj.png)][sheets-compare]

Next, we plot the time taken by PIGO and GVEL for reading Edgelist. Here, GVEL outperforms PIGO by `2.6×`, achieving a read rate of `1.9 billion edges/s` with 64 threads.

[![](https://i.imgur.com/HAxVu53.png)][sheets-compare]

Finally, we plot the strong scaling behaviour of GVEL for reading Edgelist, and for reading CSR.

[![](https://i.imgur.com/Qn64Uhn.png)][sheets]

Refer to our technical report for more details: \
[GVEL: Fast Graph Loading in Edgelist and Compressed Sparse Row (CSR) formats][report].

<br>

> [!NOTE]
> You can just copy `main.sh` to your system and run it. \
> For the code, refer to `main.cxx`.

[sheets]: https://docs.google.com/spreadsheets/d/1aN2yq3XLOsUTnHTQ9NWo_xNqAuI_BA_alLI9Uy8B4QY/edit?usp=sharing
[sheets-compare]: https://docs.google.com/spreadsheets/d/1L8W9iqtDsrUb_ZfCuCD7hxSvzJOXZPkKfvhpE5aNcJ0/edit?usp=sharing
[report]: https://arxiv.org/abs/2311.14650
[Gunrock]: https://github.com/gunrock/gunrock
[Hornet]: https://github.com/rapidsai/cuhornet
[Ligra]: https://github.com/jshun/ligra
[Galois]: https://github.com/IntelligentSoftwareSystems/Galois
[PIGO]: https://github.com/GT-TDAlab/PIGO

<br>
<br>


### Code structure

The code structure of GVEL is as follows:

```bash
- inc/_cctype.hxx: Character classification/conversion
- inc/debug.hxx: Debugging macros (LOG, ASSERT, ...)
- inc/exception.hxx: Custom exception class (FormatError)
- inc/_mman.hxx: Memory mapping/allcation functions
- inc/_openmp.hxx: OpenMP utility functions
- inc/_string.hxx: Number parsing/string tokenization
- inc/_utility.hxx: Runtime measurement functions
- inc/_vector.hxx: Vector utility functions
- inc/io.hxx: COO/MTX file reading functions
- main.cxx: Experimentation code
- process.js: Node.js script for processing output logs
```

Note that each branch in this repository contains code for a specific experiment. The `main` branch contains code for the final experiment. If the intention of a branch in unclear, or if you have comments on our technical report, feel free to open an issue.

<br>
<br>


## References

### Graph formats

- [Best/more standard graph representation file format? (GraphSON, Gexf, GraphML?) - Stack Overflow](https://stackoverflow.com/questions/31321009/best-more-standard-graph-representation-file-format-graphson-gexf-graphml)
- [Is there documentation on the structure of MAT-files in MATLAB? - MATLAB Answers - MATLAB Central](https://www.mathworks.com/matlabcentral/answers/93868-is-there-documentation-on-the-structure-of-mat-files-in-matlab)
<!-- PARAGRAPH -->
- [Matrix Market: File Formats](https://math.nist.gov/MatrixMarket/formats.html#hb)
- [Matrix Market exchange formats - Wikipedia](https://en.wikipedia.org/wiki/Matrix_Market_exchange_formats)
- [Harwell-Boeing file format - Wikipedia](https://en.wikipedia.org/wiki/Harwell-Boeing_file_format)
- [RB Files - The Rutherford Boeing Sparse Matrix File Format](https://people.math.sc.edu/Burkardt/data/rb/rb.html)
- [HB Files - The Harwell Boeing Sparse Matrix File Format](https://people.math.sc.edu/Burkardt/data/hb/hb.html)
- [DIMACS graph format](http://lcs.ios.ac.cn/~caisw/Resource/about_DIMACS_graph_format.txt)
- [DIMACS-READER - Dimacs-reader](https://www.cs.utexas.edu/users/moore/acl2/manuals/current/manual/index-seo.php/DIMACS-READER____DIMACS-READER)


### Graph frameworks

- [GT-TDAlab/PIGO](https://github.com/GT-TDAlab/PIGO)
- [rapidsai/cuhornet](https://github.com/rapidsai/cuhornet)
- [gunrock/gunrock](https://github.com/gunrock/gunrock)
- [the-data-lab/GraphOne](https://github.com/the-data-lab/GraphOne)
- [rapidsai/cugraph](https://github.com/rapidsai/cugraph)
- [cmuparlay/pbbsbench](https://github.com/cmuparlay/pbbsbench)
- [KatanaGraph/katana](https://github.com/KatanaGraph/katana)
- [GraphIt-DSL/graphit](https://github.com/GraphIt-DSL/graphit)


### Memory mapping

- [Why mmap is faster than system calls | by Alexandra (Sasha) Fedorova | Medium](https://sasha-f.medium.com/why-mmap-is-faster-than-system-calls-24718e75ab37)
- [File Handling Through C++ | How to Open, Save, Read and Close](https://www.mygreatlearning.com/blog/file-handling-in-cpp/)
- [How to use mmap function in C language?](https://linuxhint.com/using_mmap_function_linux/)
<!-- PARAGRAPH -->
- [Efficient Memory Mapped File I/O for In-Memory File Systems](https://www.usenix.org/sites/default/files/conference/protected-files/hotstorage17_slides_choi.pdf)
- [DI-MMAP—a scalable memory-map runtime for out-of-core data-intensive applications | Cluster Computing](https://link.springer.com/article/10.1007/s10586-013-0309-0)
- [Optimizing Memory-Access Patterns for Deep Learning Accelerators](https://arxiv.org/pdf/2002.12798.pdf)
<!-- PARAGRAPH -->
- [malloc vs mmap in C - Stack Overflow](https://stackoverflow.com/questions/1739296/malloc-vs-mmap-in-c)
- [memory - Anonymous mmap zero-filled? - Stack Overflow](https://stackoverflow.com/questions/17542601/anonymous-mmap-zero-filled)
- [Linux mmap() with MAP_POPULATE, man page seems to give wrong info - Stack Overflow](https://stackoverflow.com/questions/23502361/linux-mmap-with-map-populate-man-page-seems-to-give-wrong-info)
- [Is there really no asynchronous block I/O on Linux? - Stack Overflow](https://stackoverflow.com/questions/13407542/is-there-really-no-asynchronous-block-i-o-on-linux)
- [c - How to use mmap to allocate a memory in heap? - Stack Overflow](https://stackoverflow.com/questions/4779188/how-to-use-mmap-to-allocate-a-memory-in-heap)
- [c - When should I use mmap for file access? - Stack Overflow](https://stackoverflow.com/questions/258091/when-should-i-use-mmap-for-file-access)
- [c - Zero a large memory mapping with `madvise` - Stack Overflow](https://stackoverflow.com/questions/18595123/zero-a-large-memory-mapping-with-madvise)
- [c - When would one use mmap MAP_FIXED? - Stack Overflow](https://stackoverflow.com/questions/28575893/when-would-one-use-mmap-map-fixed)
- [c - Overlapping pages with mmap (MAP_FIXED) - Stack Overflow](https://stackoverflow.com/questions/14943990/overlapping-pages-with-mmap-map-fixed)
- [c++ - Platform independent memory mapped [file] IO - Stack Overflow](https://stackoverflow.com/questions/8215823/platform-independent-memory-mapped-file-io)
- [c++ - mmap() vs. reading blocks - Stack Overflow](https://stackoverflow.com/questions/45972/mmap-vs-reading-blocks)
- [c++ - Is mmap + madvise really a form of async I/O? - Stack Overflow](https://stackoverflow.com/questions/31215250/is-mmap-madvise-really-a-form-of-async-i-o)
- [c++ - Does unique_ptr::release() call the destructor? - Stack Overflow](https://stackoverflow.com/questions/25609457/does-unique-ptrrelease-call-the-destructor)
<!-- PARAGRAPH -->
- [Does the OS dispatch to another process when there's a page fault on the current process? - Quora](https://www.quora.com/Does-the-OS-dispatch-to-another-process-when-theres-a-page-fault-on-the-current-process)
- [memory management - Does a page fault causes the faulting process to reschedule? - Stack Overflow](https://stackoverflow.com/questions/13441732/does-a-page-fault-causes-the-faulting-process-to-reschedule)
- [memory management - How to get page size programmatically within Linux kernel module code - Stack Overflow](https://stackoverflow.com/questions/4888067/how-to-get-page-size-programmatically-within-linux-kernel-module-code)
<!-- PARAGRAPH -->
- [Writing files using O_DIRECT in C](https://www.linuxquestions.org/questions/programming-9/writing-files-using-o_direct-in-c-792203/)
- [LKML: Linus Torvalds: Re: O_DIRECT question](https://lkml.org/lkml/2007/1/11/129)
- [file - Portability of open(...O_DIRECT) in C? - Stack Overflow](https://stackoverflow.com/questions/924900/portability-of-open-o-direct-in-c)
<!-- PARAGRAPH -->
- [Hugepages - Debian Wiki](https://wiki.debian.org/Hugepages)
- [open(2) - Linux manual page](https://man7.org/linux/man-pages/man2/open.2.html)
- [close(2): close file descriptor - Linux man page](https://linux.die.net/man/2/close)
- [mmap(3): map pages of memory - Linux man page](https://linux.die.net/man/3/mmap)
- [mmap(2) - Linux manual page](https://man7.org/linux/man-pages/man2/mmap.2.html)
- [madvise(2): give advice about use of memory - Linux man page](https://linux.die.net/man/2/madvise)
- [madvise(2) - Linux manual page](https://man7.org/linux/man-pages/man2/madvise.2.html)
- [msync(2) - Linux manual page](https://man7.org/linux/man-pages/man2/msync.2.html)
- [mincore(2) - Linux manual page](https://man7.org/linux/man-pages/man2/mincore.2.html)
- [<sys/mman.h>](https://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/mman.h.html)


### String tokenization

- [isspace() in C - GeeksforGeeks](https://www.geeksforgeeks.org/isspace-in-c/)
- [Three reasons to pass `std::string_view` by value – Arthur O'Dwyer – Stuff mostly about C++](https://quuxplusone.github.io/blog/2021/11/09/pass-string-view-by-value/)
<!-- PARAGRAPH -->
- [What is vertical tab, form feeds and backspace character? How to use them in JavaScript? - Stack Overflow](https://stackoverflow.com/questions/2377084/what-is-vertical-tab-form-feeds-and-backspace-character-how-to-use-them-in-jav)
- [c++ - How to efficiently get a `string_view` for a substring of `std::string` - Stack Overflow](https://stackoverflow.com/questions/46032307/how-to-efficiently-get-a-string-view-for-a-substring-of-stdstring)
- [c++ - Constructing istringstream with string_view doesn't compile - Stack Overflow](https://stackoverflow.com/questions/57259965/constructing-istringstream-with-string-view-doesnt-compile)
- [c++ - Safely convert std::string_view to int (like stoi or atoi) - Stack Overflow](https://stackoverflow.com/questions/56634507/safely-convert-stdstring-view-to-int-like-stoi-or-atoi)
- [c++ - Passing std::string_view by reference - Stack Overflow](https://stackoverflow.com/questions/58410793/passing-stdstring-view-by-reference)
- [c++ - Why is std::string_view faster than const char*? - Stack Overflow](https://stackoverflow.com/questions/60015661/why-is-stdstring-view-faster-than-const-char)
- [c++ - Why can't I construct a string_view from range iterators? - Stack Overflow](https://stackoverflow.com/questions/66691886/why-cant-i-construct-a-string-view-from-range-iterators)
- [c++ - How you convert a std::string_view to a const char*? - Stack Overflow](https://stackoverflow.com/questions/48081436/how-you-convert-a-stdstring-view-to-a-const-char)
- [c++17 - Why is there no implicit conversion from std::string_view to std::string? - Stack Overflow](https://stackoverflow.com/questions/47525238/why-is-there-no-implicit-conversion-from-stdstring-view-to-stdstring)
- [pointers - Meaning of *& and **& in C++ - Stack Overflow](https://stackoverflow.com/questions/5789806/meaning-of-and-in-c)
<!-- PARAGRAPH -->
- [std::basic_string_view - cppreference.com](https://en.cppreference.com/w/cpp/string/basic_string_view)
- [std::basic_string_view<CharT,Traits>::basic_string_view - cppreference.com](https://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view)
- [std::basic_string_view<CharT,Traits>::substr - cppreference.com](https://en.cppreference.com/w/cpp/string/basic_string_view/substr)
- [std::basic_string_view<CharT,Traits>::remove_prefix - cppreference.com](https://en.cppreference.com/w/cpp/string/basic_string_view/remove_prefix)
- [C++ named requirements: LegacyRandomAccessIterator - cppreference.com](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator)
- [memchr - cppreference.com](https://en.cppreference.com/w/c/string/byte/memchr)


### Parsing numbers

- [c - converting a string into a double - Stack Overflow](https://stackoverflow.com/questions/4308536/converting-a-string-into-a-double)
- [c++ - convert string to size_t - Stack Overflow](https://stackoverflow.com/questions/34043894/convert-string-to-size-t)
- [c++ - Undefined symbol std::from_chars(...) - Stack Overflow](https://stackoverflow.com/questions/73362643/undefined-symbol-stdfrom-chars)
- [C++ most efficient way to convert string to int (faster than atoi) - Stack Overflow](https://stackoverflow.com/questions/16826422/c-most-efficient-way-to-convert-string-to-int-faster-than-atoi)
- [c++ - optimal way to convert string to double? - Stack Overflow](https://stackoverflow.com/questions/11471661/optimal-way-to-convert-string-to-double)
- [c++ - What's the fastest or most efficient way to convert a double value to its most concise string representation? - Stack Overflow](https://stackoverflow.com/questions/49500969/whats-the-fastest-or-most-efficient-way-to-convert-a-double-value-to-its-most-c)
- [Fastest way to read numerical values from text file in C++ (double in this case) - Stack Overflow](https://stackoverflow.com/questions/5678932/fastest-way-to-read-numerical-values-from-text-file-in-c-double-in-this-case)
- [c++ - What is more efficient? Using pow to square or just multiply it with itself? - Stack Overflow](https://stackoverflow.com/questions/2940367/what-is-more-efficient-using-pow-to-square-or-just-multiply-it-with-itself)
<!-- PARAGRAPH -->
- [Standard library header <charconv> (C++17) - cppreference.com](https://en.cppreference.com/w/cpp/header/charconv)
- [std::chars_format - cppreference.com](https://en.cppreference.com/w/cpp/utility/chars_format)
- [std::from_chars - cppreference.com](https://en.cppreference.com/w/cpp/utility/from_chars)
- [std::is_integral - cppreference.com](https://en.cppreference.com/w/cpp/types/is_integral)
- [std::pow, std::powf, std::powl - cppreference.com](https://en.cppreference.com/w/cpp/numeric/math/pow)
- [strtoull - cplusplus.com](https://cplusplus.com/reference/cstdlib/strtoull/)


### Parsing numbers (SIMD)

- [Parsing integers quickly with AVX-512 – Daniel Lemire's blog](https://lemire.me/blog/2023/09/22/parsing-integers-quickly-with-avx-512/?unapproved=655966&moderation-hash=facfd7c3517c4062324e656ae5397261#comment-655966)
- [Faster Integer Parsing](https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html)
- [Fast float parsing in practice – Daniel Lemire's blog](https://lemire.me/blog/2020/03/10/fast-float-parsing-in-practice/)
- [AVX512/VBMI2: A Programmer’s Perspective | Hacker News](https://news.ycombinator.com/item?id=28179111)
<!-- PARAGRAPH -->
- [Code-used-on-Daniel-Lemire-s-blog/2023/09/22/src/parse_integer.cpp at master · lemire/Code-used-on-Daniel-Lemire-s-blog · GitHub](https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2023/09/22/src/parse_integer.cpp)
- [GitHub - lemire/fast_double_parser: Fast function to parse strings into double (binary64) floating-point values, enforces the RFC 7159 (JSON standard) grammar: 4x faster than strtod](https://github.com/lemire/fast_double_parser)
<!-- PARAGRAPH -->
- [visual c++ - How to write c++ code that the compiler can efficiently compile to SSE or AVX? - Stack Overflow](https://stackoverflow.com/questions/33504003/how-to-write-c-code-that-the-compiler-can-efficiently-compile-to-sse-or-avx)
- [Header files for x86 SIMD intrinsics - Stack Overflow](https://stackoverflow.com/questions/11228855/header-files-for-x86-simd-intrinsics)
- [simd - What exactly do the gcc compiler switches (-mavx -mavx2 -mavx512f) do? - Stack Overflow](https://stackoverflow.com/questions/71229343/what-exactly-do-the-gcc-compiler-switches-mavx-mavx2-mavx512f-do)
- [x86 Options (Using the GNU Compiler Collection (GCC))](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html)
- [Improving performance with SIMD intrinsics in three use cases - Stack Overflow](https://stackoverflow.blog/2020/07/08/improving-performance-with-simd-intrinsics-in-three-use-cases/)
- [How do I define a 512 bit integer in C++? - Stack Overflow](https://stackoverflow.com/questions/47898555/how-do-i-define-a-512-bit-integer-in-c)
- [c - Is there a 256-bit integer type? - Stack Overflow](https://stackoverflow.com/questions/55802309/is-there-a-256-bit-integer-type)
- [C++ 128/256-bit fixed size integer types - Stack Overflow](https://stackoverflow.com/questions/5242819/c-128-256-bit-fixed-size-integer-types)
- [c++ - How to create a left-packed vector of indices of the 0s in one SIMD vector? - Stack Overflow](https://stackoverflow.com/questions/72098296/how-to-create-a-left-packed-vector-of-indices-of-the-0s-in-one-simd-vector)
- [c++ - Can I use SIMD for speeding up string manipulation? - Stack Overflow](https://stackoverflow.com/questions/64975030/can-i-use-simd-for-speeding-up-string-manipulation)
- [c++ - SSE and AVX intrinsics mixture - Stack Overflow](https://stackoverflow.com/questions/18319488/sse-and-avx-intrinsics-mixture)
- [c++ - Why is transforming an array using AVX-512 instructions significantly slower when transforming it in batches of 8 compared to 7 or 9? - Stack Overflow](https://stackoverflow.com/questions/74069410/why-is-transforming-an-array-using-avx-512-instructions-significantly-slower-whe)
- [c++ - Compiling legacy GCC code with AVX vector warnings - Stack Overflow](https://stackoverflow.com/questions/39383193/compiling-legacy-gcc-code-with-avx-vector-warnings)
- [c - inlining failed in call to always_inline ‘_mm_mullo_epi32’: target specific option mismatch - Stack Overflow](https://stackoverflow.com/questions/43128698/inlining-failed-in-call-to-always-inline-mm-mullo-epi32-target-specific-opti)
- [android - What causes signal 'SIGILL'? - Stack Overflow](https://stackoverflow.com/questions/7901867/what-causes-signal-sigill)
<!-- PARAGRAPH -->
- [Intel® Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#!=undefined&expand=7&cats=Convert&text=_mm256_cvtepi16_epi8&ig_expand=1616)
- [EPYC Offers x86 Compatibility](https://www.amd.com/content/dam/amd/en/documents/epyc-business-docs/white-papers/EPYC-Offers-x86-Compatibility.pdf)
- [AVX-512 - Wikipedia](https://en.wikipedia.org/wiki/AVX-512)


### Stringifying numbers

- [Convert double to string in scientific notation faster than sprintf in c++ - Stack Overflow](https://stackoverflow.com/questions/28260412/convert-double-to-string-in-scientific-notation-faster-than-sprintf-in-c)
- [std::to_chars - cppreference.com](https://en.cppreference.com/w/cpp/utility/to_chars)


### Performance optimization

- [c++ - efficient push_back in std::vector - Stack Overflow](https://stackoverflow.com/questions/15411641/efficient-push-back-in-stdvector)
- [Optimizing C++ vector for 2x performance ! - DEV Community](https://dev.to/adwaitthattey/optimizing-c-vector-for-2x-performance--1691)
- [c++ - How to reserve a multi-dimensional Vector without increasing the vector size? - Stack Overflow](https://stackoverflow.com/questions/57542919/how-to-reserve-a-multi-dimensional-vector-without-increasing-the-vector-size)
- [c++ - What is gsl::multi_span to be used for? - Stack Overflow](https://stackoverflow.com/questions/45201524/what-is-gslmulti-span-to-be-used-for)
- [c++ - The difference between vector and deque - Stack Overflow](https://stackoverflow.com/questions/22068188/the-difference-between-vector-and-deque)
- [c++ - How is sort for std::deque implemented? - Stack Overflow](https://stackoverflow.com/questions/24199421/how-is-sort-for-stddeque-implemented)
- [performance - What does gcc's ffast-math actually do? - Stack Overflow](https://stackoverflow.com/questions/7420665/what-does-gccs-ffast-math-actually-do)
<!-- PARAGRAPH -->
- [std::deque<T,Allocator>::operator[] - cppreference.com](https://en.cppreference.com/w/cpp/container/deque/operator_at)
- [std::deque - cppreference.com](https://en.cppreference.com/w/cpp/container/deque)
<!-- PARAGRAPH -->
- [Compiler Explorer](https://godbolt.org/)


### Smart pointers

- [How to: Create and use unique_ptr instances | Microsoft Learn](https://learn.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-unique-ptr-instances?view=msvc-170)
- [Smart pointers (Modern C++) | Microsoft Learn](https://learn.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp?view=msvc-170)
- [c++ - Why can I not push_back a unique_ptr into a vector? - Stack Overflow](https://stackoverflow.com/questions/3283778/why-can-i-not-push-back-a-unique-ptr-into-a-vector)
<!-- PARAGRAPH -->
- [std::unique_ptr - cppreference.com](https://en.cppreference.com/w/cpp/memory/unique_ptr)
- [std::make_unique, std::make_unique_for_overwrite - cppreference.com](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique)


### Exception handling

- [What are the best practices when implementing C++ error handling? - Software Engineering Stack Exchange](https://softwareengineering.stackexchange.com/questions/430984/what-are-the-best-practices-when-implementing-c-error-handling)
- [How to throw dynamic string in custom exception? I can't throw string created on stack! : r/cpp_questions](https://www.reddit.com/r/cpp_questions/comments/pq0ezf/how_to_throw_dynamic_string_in_custom_exception_i/)
- [C++ Exception Handling (With Examples)](https://www.programiz.com/cpp-programming/exception-handling)
- [Modern C++ best practices for exceptions and error handling | Microsoft Learn](https://learn.microsoft.com/en-us/cpp/cpp/errors-and-exception-handling-modern-cpp?view=msvc-170)
- [Zero-cost exceptions aren't actually zero cost - The Old New Thing](https://devblogs.microsoft.com/oldnewthing/20220228-00/?p=106296)
<!-- PARAGRAPH -->
- [Should I return TRUE / FALSE values from a C function? - Stack Overflow](https://stackoverflow.com/questions/559061/should-i-return-true-false-values-from-a-c-function)
- [error handling - C++ custom exception class - Code Review Stack Exchange](https://codereview.stackexchange.com/questions/244124/c-custom-exception-class)
- [c++11 - Exception propagation and context - Stack Overflow](https://stackoverflow.com/questions/58208792/exception-propagation-and-context)
- [c++ - throw an exception from a lambda expression, bad habit? - Stack Overflow](https://stackoverflow.com/questions/37092596/throw-an-exception-from-a-lambda-expression-bad-habit)
- [c++ - Augment a thrown exception with some contextual information - Software Engineering Stack Exchange](https://softwareengineering.stackexchange.com/questions/349830/augment-a-thrown-exception-with-some-contextual-information)
- [c++ - throw new std::exception vs throw std::exception - Stack Overflow](https://stackoverflow.com/questions/10948316/throw-new-stdexception-vs-throw-stdexception)
- [c++ - Use throw_with_nested and catch nested exception - Stack Overflow](https://stackoverflow.com/questions/18108314/use-throw-with-nested-and-catch-nested-exception)
- [c++ - What is the correct way for exception handling with OpenMP tasks? - Stack Overflow](https://stackoverflow.com/questions/69889899/what-is-the-correct-way-for-exception-handling-with-openmp-tasks)
<!-- PARAGRAPH -->
- [Standard library header <exception> - cppreference.com](https://en.cppreference.com/w/cpp/header/exception)
- [noexcept specifier (since C++11) - cppreference.com](https://en.cppreference.com/w/cpp/language/noexcept_spec)
- [std::perror - cppreference.com](https://en.cppreference.com/w/cpp/io/c/perror)
- [std::exception::exception - cppreference.com](https://en.cppreference.com/w/cpp/error/exception/exception)
- [std::current_exception - cppreference.com](https://en.cppreference.com/w/cpp/error/current_exception)
- [std::nested_exception::nested_exception - cppreference.com](https://en.cppreference.com/w/cpp/error/nested_exception/nested_exception)
- [std::nested_exception - cppreference.com](https://en.cppreference.com/w/cpp/error/nested_exception)


### Basics

- [OpenMP - Scheduling(static, dynamic, guided, runtime, auto) - Yiling's Tech Zone](https://610yilingliu.github.io/2020/07/15/ScheduleinOpenMP/)
- [c++ - OPenMP loop increment - Stack Overflow](https://stackoverflow.com/questions/7448812/openmp-loop-increment)
- [c++ - Atomic Minimum on x86 using OpenMP - Stack Overflow](https://stackoverflow.com/questions/12255266/atomic-minimum-on-x86-using-openmp)
- [Class method and variable with same name, compile error in C++ not in Java? - Stack Overflow](https://stackoverflow.com/questions/467815/class-method-and-variable-with-same-name-compile-error-in-c-not-in-java)
- [c++ - Do the &= and |= operators for bool short-circuit? - Stack Overflow](https://stackoverflow.com/questions/23107162/do-the-and-operators-for-bool-short-circuit)
- [c++ - What is the purpose of std::make_pair vs the constructor of std::pair? - Stack Overflow](https://stackoverflow.com/questions/9270563/what-is-the-purpose-of-stdmake-pair-vs-the-constructor-of-stdpair)
- [c++ - What is the reason for `std::make_tuple`? - Stack Overflow](https://stackoverflow.com/questions/34180636/what-is-the-reason-for-stdmake-tuple)
- [c++ - Pass an array as template type - Stack Overflow](https://stackoverflow.com/questions/10128159/pass-an-array-as-template-type)
- [Multiple typename arguments in c++ template? (variadic templates) - Stack Overflow](https://stackoverflow.com/questions/19923353/multiple-typename-arguments-in-c-template-variadic-templates)
- [What does 'const static' mean in C and C++? - Stack Overflow](https://stackoverflow.com/questions/177437/what-does-const-static-mean-in-c-and-c)
- [c++ - Should I use size_t or ssize_t? - Stack Overflow](https://stackoverflow.com/questions/15739490/should-i-use-size-t-or-ssize-t)
<!-- PARAGRAPH -->
- [Parameter pack(since C++11) - cppreference.com](https://en.cppreference.com/w/cpp/language/parameter_pack)
- [std::tie - cppreference.com](https://en.cppreference.com/w/cpp/utility/tuple/tie)
- [std::pair - cplusplus.com](https://cplusplus.com/reference/utility/pair/pair/)
- [std::array - cppreference.com](https://en.cppreference.com/w/cpp/container/array)
- [std::size_t - cppreference.com](https://en.cppreference.com/w/cpp/types/size_t)
<!-- PARAGRAPH -->
- [Reduction Clauses and Directives](https://www.openmp.org/spec-html/5.0/openmpsu107.html)
- [ASCII Table - ASCII Character Codes, HTML, Octal, Hex, Decimal](https://www.asciitable.com/)


### Testing

- [Remove First n Lines of a Large Text File - Ask Ubuntu](https://askubuntu.com/questions/410196/remove-first-n-lines-of-a-large-text-file/410209#410209)
- [javascript - How to append to a file in Node? - Stack Overflow](https://stackoverflow.com/questions/3459476/how-to-append-to-a-file-in-node)
- [git - Pushing code from one branch of a repo to another branch of another repo - Stack Overflow](https://stackoverflow.com/questions/35939308/pushing-code-from-one-branch-of-a-repo-to-another-branch-of-another-repo)

<br>
<br>


[![](https://img.youtube.com/vi/yqO7wVBTuLw/maxresdefault.jpg)](https://www.youtube.com/watch?v=yqO7wVBTuLw)<br>
[![ORG](https://img.shields.io/badge/org-puzzlef-green?logo=Org)](https://puzzlef.github.io)
