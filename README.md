# MOSCONV
A CP/M 2.2 program that converts Intel Hex Object code and converts it into they SYM-1/KIM-1 MOS paper tape standard

I wrote this with the intention of using the A65.COM 6502 assembler.

## Building
### CP/M
This program is formatted and written with the Aztec C compiler in mind. In order to
compile, send MOSCONV.C to your system.

Next, change the `#DEFINES` to `AZTEC_C` and `CPM`.

Compile with the flags:

`cc/cz mosconv`\
`as mosconv`\
`ln mosconv.o c.lib`

### *NIX/WINDOWS
As long as you have the C standard libraries (gcc/mingw), you're all set to build and use! No switches need to be
changed.

## Usage
The proper syntax is:
`mosconv -s/k [-b xx] hexfile.hex file.ptp`

* -s/k\
This switch controls if your output paper tape is KIM or SYM compatible.
* -b\
This switch controls the number of bytes per record you write. The KIM-1 supports 24 bytes per record by default. The SYM-1 suppors 16 bytes by default, but either can be changed with this switch.
* hexfile.hex\
Source Intel Hex Object code file.
* file.ptp\
Output paper tape file.

Then, you can use your program of choice to pipe out `file.ptp` as a paper tape to your SYM-1.

## MAXRC
The default standard for the SYM-1 is to allow up to 16 (10 hex) data bytes per line in paper tape.
You can change this by changing the MAXRC byte in RAM ($A658) to the length you wish. Be sure
to change the MAXRC `#DEFINE` to match this.