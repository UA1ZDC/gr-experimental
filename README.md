# GR-Experimental

GNU Radio blocks for converting demodulated data from MetOp and FengYun-3 satellites into CADU data without FEC.

### Source

Blocks are based on: [LeanHRPT-Demod](https://github.com/Xerbo/LeanHRPT-Demod)

### Installation

You will need GNU Radio 3.10+ to install these blocks.

```
sudo apt update && sudo apt upgrade
mkdir Build && cd Build
cmake ..
make
sudo make install
sudo ldconfig
```
### Blocks

- Viterbi decoder 3/4 (MetOp or FengYun-3 mode)
- Deframer
- Differential decoder (for FengYun-3 satellites)
- Derandomizer
- Reed-Solomon


