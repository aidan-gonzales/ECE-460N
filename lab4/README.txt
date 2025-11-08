INT signal
The INT register contains a 1 if an interrupt has been requested, and a 0 if there is no interrupt waiting to be processed.
The INT register is within the CONTROL logic unit. When the timer interrupt occurs, the signal is set high and set directly 
into the CONTROL logic unit without accessing the bus.

TIMSR
The timer interrupt register tells us if the timer is requesting an interrupt. If bit 15 is set high, then an interrupt
is being requested. This register is connected directly to the the CONTROL logic unit, which decides whether or not to
set the INT signal.
