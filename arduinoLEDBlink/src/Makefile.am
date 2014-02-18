
bin_PROGRAMS = arduinoLEDBlink
arduinoLEDBlink_SOURCES = arduinoLEDBlink.cpp main.cpp variant.cpp wiring_digital.c UtilTime.cpp trace.c interrupt.c WString.cpp mux.c sysfs.c wiring_analog.c RingBuffer.cpp TTYUART.cpp fast_gpio_sc.c Print.cpp

AM_CXXFLAGS = @arduinoLEDBlink_CFLAGS@
AM_LDFLAGS = @arduinoLEDBlink_LIBS@ -lpthread

CLEANFILES = *~ 
