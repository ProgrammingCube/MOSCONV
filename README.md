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
`mosconv -s/k [-b xx] hexfile.hex file.ptp [n]`

* -**s**/**k**\
This switch controls if your output paper tape is KIM or SYM compatible.
* -**b**\
This switch controls the number of bytes per record you write. The KIM-1 supports 24 bytes per record by default. The SYM-1 supports 16 bytes by default, but either can be changed with this switch.
* **hexfile.hex**\
Source Intel Hex Object code file.
* **file.ptp**\
Output paper tape file.

## NULL CHARACTERS
If the KIM-1 model is selected, you have an option to print out the 6 NULL characters per record line. By default, this is turned off as it is
largely seen as a waste of character space and physical tape. Normally, character and line delays are used now. But you can still turn this feature ON by setting the `NULLCHAR` define as 1.


Then, you can use your program of choice to pipe out `file.ptp` as a paper tape to your SYM-1/KIM-1.

## MAXRC
The SYM-1 supports data record lengths greater than 16 (10 hex) bytes.\
In order to enable this, you must change the MAXRC byte in memory by changing the contents of memory location $A658.