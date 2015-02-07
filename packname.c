/*
 * Copyright © 2012-2014 Lars Lindqvist
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "util.h"

char *
dst_packname(const char *path, enum SPKG what) {
	const char *file, *p;
	char *ret;
	int sep_idx[3], sep_n, ext_idx;
	int i, start, end, len;


	if((file = strrchr(path, '/'))) {
		++file;
	} else {
		file = path;
	}

	len = strlen(file);
	ext_idx = 0;
	if((p = strrchr(file, '.'))) {
		if(!strncmp(p, ".txz", 4)
		|| !strncmp(p, ".tgz", 4)
		|| !strncmp(p, ".tbz", 4)
		|| !strncmp(p, ".tlz", 4)) {
			ext_idx = p - file + 1;
		}
	}

	for(sep_n = 0, i = strlen(file); sep_n < 3 && i > 0; --i) {
		if(file[i] == '-') {
			sep_idx[sep_n++] = i;
		}
	}

	end = len;
	start = 0;

	if(sep_n < 3) {
		/* Old style, or out of spec */
		switch(what) {
		case SPKG_NAME:
			start = 0;
			end = ext_idx ? ext_idx - 1 : len;
			break;
		case SPKG_EXT:
			start = ext_idx ? ext_idx : len;
			end = len;
			break;
		case SPKG_BASE:
			start = 0;
			end = ext_idx ? ext_idx - 1 : len;
			break;
		default:
			return NULL;
		}
	} else {
		switch(what) {
		case SPKG_NAME:
			start = 0;
			end = sep_idx[2];
			break;
		case SPKG_VERSION:
			start = sep_idx[2] + 1;
			end = sep_idx[1];
			break;
		case SPKG_ARCH:
			start = sep_idx[1] + 1;
			end = sep_idx[0];
			break;
		case SPKG_BUILD:
			start = sep_idx[0] + 1;
			for(p = file + start; isdigit(*p); ++p);
			end = p - file;
			break;
		case SPKG_TAG:
			for(p = file + sep_idx[0] + 1; isdigit(*p); ++p);
			start = p - file;
			end = ext_idx ? ext_idx - 1 : len;
			break;
		case SPKG_EXT:
			start = ext_idx ? ext_idx : len;
			end = len;
			break;
		case SPKG_BASE:
			start = 0;
			end = ext_idx ? ext_idx - 1 : len;
			break;
		case SPKG_NULL:
			return NULL;
		}
	}

	ret = NULL;

	if((start || end) && end >= start && start < len && end <= len) {
		ret = calloc(end - start + 1, sizeof(char));
		strncpy(ret, file + start, end - start);
	}

	return ret;
}
