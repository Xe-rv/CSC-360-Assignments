## CSC 360: Spring 2024

### Assignment #4 evaluation

* Student name: Rui Gong
* Student netlink: `xerv`
* Student number:  V01001589

---

### Virtual-memory page-replacement simulation

* `virtmem.c` submitted and compiles without errors or warnings: YES

If YES above, then the following sequence of tests were performed.

(Note: For all of the following, the sole statistic examined was `Page
faults`. It appears there was too much ambiguity with the assignment
specification of the dirty bit and the meaning of swapouts to use
these as a basis for evaluation.)

* `fifo` with `FRAMESIZE` of 10 and `NUMFRAMES` at 70, 30, and 10,
using `hello-out.txt`.  Expected behavior seen? YES

* `lru` with `FRAMESIZE` of 9 and `NUMFRAMES` at 40 and 20, using
`hello-out.txt.  Expected behavior seen? YES

* `clock` with `FRAMESIZE` of 11 and `NUMFRAMES` at 30 and 15, using
`ls-out.txt`.  Expected behavior seen? YES

* `fifo`, `clock`, and `lru` with `FRAMESIZE` of 10 and `NUMFRAMES` at
12, using `matrixmult-out.txt`. Expected behavior seen? YES

* If NO above, is expected behavior largely seen when the values `FRAMESIZE` and
`NUMFRAMES` are varied? YES

* Code is commented in a sensible way: YES

---

### Other evaluator comments

Good job!

---

### Assignment grade: A
