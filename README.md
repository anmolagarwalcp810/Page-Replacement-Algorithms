# Page-Replacement-Algorithms
Contains implementation of different page replacement algorithms, namely, FIFO (FF), LRU (LR), LFU (LF), Clock (CL) and N-Chance (SC).

[`README.pdf`](./README.pdf) contains detailed description of implementation of different page replacement algorithms.

## How to Run
```bash
make ARGS="./src/input.txt FF LF LR SC"
```

### Format of Input File
```text
<Number of Page Frames> <Buffer Size>
<Frame#1> <Frame#2> ... <Frame#<Number of Page Frames>>
```
