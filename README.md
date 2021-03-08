# MOSCONV
Converts Intel Hex Object code and converts it into they Sym-1 MOS paper tape standard

I wrote this with the intention of using the A65.COM 6502 assembler.

## Building
This program is formatted and written with the Aztec C compiler in mind. In order to
compile, send MOSCONV.C to your system.

Next, change the '#DEFINES' to 'AZTEC_C' and 'CPM'.

Compile with the flags:

'cc/cz mosconv'
'as mosconv'
'ln mosconv.o c.lib'


##Using
The proper syntax is:
'mosconv hexfile.hex file.ptp'

Then, you can use your program of choice to pipe out 'file.ptp' as a paper tape to your SYM-1.

##MAXRC
The default standard for the SYM-1 is to allow up to 16 (10 hex) data bytes per line in paper tape.
You can change this by changing the MAXRC byte in RAM ($A658) to the length you wish. Be sure
to change the MAXRC '#DEFINE' to match this.