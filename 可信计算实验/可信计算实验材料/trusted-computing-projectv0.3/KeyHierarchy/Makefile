#
#  Copyright (c) International Business Machines  Corp., 2004, 2005
#
#  This program is free software;  you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY;  without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
#  the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program;  if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

###########################################################################
# name of file  : Makefile                                                #
# description   : make(1) description file for the testcase directories.  #
###########################################################################
CC = gcc
ifeq ($(WITH_GCOV),1)
	OPTS = -fprofile-arcs -ftest-coverage
else
	OPTS =
endif
ALL = $(shell ls *.c | sed "s/\.c//g")
LIBS = ../common/common.o -ltspi $(LDFLAGS)
CFLAGS += -g -I../include

.c:
	$(CC) $(OPTS) $(CFLAGS) -o $@ $< $(LIBS)

all: $(ALL)

install:
	@set -e; for i in $(ALL); do mv $$i ../../bin/$$i ; done

clean:
	rm -f *.o ../../bin/$(ALL) *~ $(ALL) *.bbg *.bb *.da
