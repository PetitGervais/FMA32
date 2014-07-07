This project has the following structure :<br>
<br>
├── documentation<br>
│&nbsp;&nbsp; ├── Memory allocator.odt<br>
│&nbsp;&nbsp; └── Memory allocator.pdf<br>
├── src<br>
│&nbsp;&nbsp; ├── bitwise.h<br>
│&nbsp;&nbsp; ├── memory.c<br>
│&nbsp;&nbsp; └── memory.h<br>
└── testern&lt;br&gt;<br>
&nbsp;&nbsp;&nbsp; ├── Makefile<br>
&nbsp;&nbsp;&nbsp; └── src<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ├── AllocTest.cpp<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ├── Blocks.cpp<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ├── Blocks.h<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ├──MemoryAllocTest.cpp<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ├──MemoryAllocTest.h<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; └── test.h<br>
<br>
- documentation, contains document on how the FMA32 works<br>
- src, the FMA32 project files<br>
- tester, contains a mini project who allocate and free a lot of memory and check if the memory is ok<br>
<br>
If you want to use FMA32 in your project, you have to add the 3 following file : bitwise.h, memory.h, memory.c in your project<br>
Those files don't use any external library and could be compiled on every platform.<br>
<br>
FMA32 offers 3 public functions :<br>
memory_init : used to initialize the memory area that you want to use for allocation/de-allocation. This is mandatory to do before any call to alloc or free
function.<br>
memory_alloc : used to allocate a chunk of memory.<br>
memory_free  : used to free a chunck previously allocated by memory_alloc.<br>

TODO :<br> 
Add unitary test of each functions (Code is already written but needs to be refactored)<br>
Try to optimize some functions to get better performances (saw with the profiler gprof)

Have fun with FMA32 and if you have any questions or remarks, don't hesitate your are welcome !!!

Romain CARITEY
