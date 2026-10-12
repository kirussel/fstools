#
# Copyright (c) 2009 Apple Inc. All rights reserved.
#
# @APPLE_LICENSE_HEADER_START@
# 
# This file contains Original Code and/or Modifications of Original Code
# as defined in and that are subject to the Apple Public Source License
# Version 2.0 (the 'License'). You may not use this file except in
# compliance with the License. Please obtain a copy of the License at
# http://www.opensource.apple.com/apsl/ and read it before using this
# file.
# 
# The Original Code and all software distributed under the License are
# distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
# INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
# Please see the License for the specific language governing rights and
# limitations under the License.
# 
# @APPLE_LICENSE_HEADER_END@
#

ifeq ($(shell uname -s),Darwin)
ARCHS ?= arm64 x86_64
endif
INTERNAL_FRAMEWORKS := /AppleInternal/Library/Frameworks
CFLAGS := $(ARCHS:%=-arch %) -Wall -O3

ifeq ($(wildcard $(INTERNAL_FRAMEWORKS)), $(INTERNAL_FRAMEWORKS))
CPPFLAGS += -DXILOG
CFLAGS += -F/AppleInternal/Library/Frameworks
LIBS += -framework XILog
endif

CLANG_FORMAT ?= clang-format
PKG_CONFIG ?= pkg-config
SRCS := fsx.c parsetime.c parsetime.h tests/check_parsetime.c

# Unit tests link against the system's Check library, which is usually
# built for the host architecture only, so they don't use $(ARCHS).
CHECK_CFLAGS := $(shell $(PKG_CONFIG) --cflags check 2>/dev/null)
CHECK_LIBS := $(shell $(PKG_CONFIG) --libs check 2>/dev/null)
TESTS := tests/check_parsetime

.PHONY: check clean lint format format-diff

fsx: fsx.c parsetime.c parsetime.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ fsx.c parsetime.c $(LIBS)

# Build and run the unit tests (needs Check: https://libcheck.github.io/check/).
check: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

tests/check_parsetime: tests/check_parsetime.c parsetime.c parsetime.h
	@$(PKG_CONFIG) --exists check || { echo "Check not found by $(PKG_CONFIG); install it (e.g. apt install check, brew install check)" >&2; exit 1; }
	$(CC) -Wall -O2 $(CPPFLAGS) $(CHECK_CFLAGS) -o $@ tests/check_parsetime.c parsetime.c $(CHECK_LIBS)

# Fail if any source differs from .clang-format (for CI).
lint:
	$(CLANG_FORMAT) --dry-run --Werror $(SRCS)

# Show what clang-format would change, as a diff, without touching files.
format-diff:
	@for f in $(SRCS); do \
		$(CLANG_FORMAT) $$f | diff -u $$f - || true; \
	done

# Rewrite sources in place.
format:
	$(CLANG_FORMAT) -i $(SRCS)

clean:
	-rm -f *.o fsx $(TESTS)

