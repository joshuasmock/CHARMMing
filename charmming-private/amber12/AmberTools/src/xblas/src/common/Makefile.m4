dnl
dnl
define(`if_f2c', `ifdef(`no_f2c', `$2', `$1')')dnl
dnl
dnl
include ../../make.conf
include ../../$(MAKEINC)

LIB_PATH = ../../$(OUTPUT_DIR)
HEADER_PATH = ..
COMM_OBJS = BLAS_error.o blas_malloc.o BLAS_fpinfo_x.o`'if_f2c(` BLAS_fpinfo_x-f2c.o')

all: $(COMM_OBJS)

lib: $(COMM_OBJS)
	$(ARCH) $(ARCHFLAGS) $(LIB_PATH)/$(XBLASLIB) $(COMM_OBJS)
	$(RANLIB) $(LIB_PATH)/$(XBLASLIB)

lib-amb: $(COMM_OBJS)
	$(ARCH) $(ARCHFLAGS) $(LIB_PATH)/$(XBLASLIB) $(COMM_OBJS)
	$(RANLIB) $(LIB_PATH)/$(XBLASLIB_AMB)

.c.o:
	$(CC) $(CFLAGS) -I$(HEADER_PATH) -c -o $@ $<

clean:
	rm -f *.o *~ core


