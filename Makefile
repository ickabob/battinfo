# See LICENSE file for copyright and license details.

include config.mk

SRC = ${NAME}.c 
TEST_SRC = ${./tests/test_+.c}
OBJ = ${SRC:.c=.o}

all: options ${NAME}

options:
	@echo ${NAME} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $< ${CFLAGS}
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

${NAME}: ${OBJ}
	@echo CC -o $@ ${OBJ} ${LDFLAGS}
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

tests: sysctl_battery.c
	@echo "Making tests in ./tests ..."
	@echo TEST_SRC: ${TEST_SRC}	

clean:
	@echo cleaning
	@rm -f ${NAME} ${OBJ} ${NAME}-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p ${NAME}-${VERSION}
	@cp -R Makefile config.mk LICENSE \
		${SRC} ${NAME}-${VERSION}
	@tar -cf ${NAME}-${VERSION}.tar ${NAME}-${VERSION}
	@gzip ${NAME}-${VERSION}.tar
	@rm -rf ${NAME}-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all options clean dist install uninstall
