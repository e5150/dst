/* Copyright © 2010,2013 Lars Lindqvist 
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

char *
hr(off_t val) {
    char *ret;
    int i;
    double x;
    const char prefix[10] = "bkMGTPEZY";

    for(i = 0, x = val; x > 1000.0; x /= 1000.0, i++);

    if(i > 9)
        return NULL;

    ret = calloc(6, sizeof(char));
    if(x < 10.0)
        sprintf(ret, "%.2f%c", x, prefix[i]);
    else if(x < 100.0)
        sprintf(ret, "%.1f%c", x, prefix[i]);
    else if(x < 1000.0)
        sprintf(ret, "%.0f%c", x, prefix[i]);

    return ret;
}
