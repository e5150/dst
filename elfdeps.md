<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="resource-type" content="document">
<title>
ELFDEPS(1)</title>
</head>
<body>
<div class="mandoc">
<table summary="Document Header" class="head" width="100%">
<col width="30%">
<col width="30%">
<col width="30%">
<tbody>
<tr>
<td class="head-ltitle">
ELFDEPS(1)</td>
<td class="head-vol" align="center">
darkstar-tools-14.1</td>
<td class="head-rtitle" align="right">
ELFDEPS(1)</td>
</tr>
</tbody>
</table>
<div class="section">
<h1 id="x4e414d45">NAME</h1> <b class="name">elfdeps</b> &#8212; <span class="desc">querys ELF files for NEEDED libraries.</span></div>
<div class="section">
<h1 id="x53594e4f50534953">SYNOPSIS</h1><table class="synopsis">
<col style="width: 7.00ex;">
<col>
<tbody>
<tr>
<td>
elfdeps</td>
<td>
&#91;<span class="opt"><b class="flag">&#45;vvvv</b></span>&#93; <i class="file">file ...</i></td>
</tr>
</tbody>
</table>
</div>
<div class="section">
<h1 id="x4445534352495054494f4e">DESCRIPTION</h1> <b class="name">elfdeps</b> scans the dynamic section of an ELF file for DT_NEEDED libraries, and does a query for the required libraries in the /lib, /usr/lib, /usr/local/lib and /lib64, /usr/lib64, /usr/local/lib64 directories for 32 and 64 bit ELF files, respectively. Any DT_RPATH found in the dynamic section is also scanned for the libraries. Output is formatted as &quot;&lt;file&gt;:&lt;lib&gt;:&lt;path&gt;&quot;, where <i class="file">file</i> is the path given as argument, lib is the needed library and path is either the path to required library or &quot;missing&quot;.</div>
<div class="section">
<h1 id="x415247554d454e5453">ARGUMENTS</h1> <b class="flag">&#45;v</b> Increase verbosity, may be given multiple times, only for debugging.<dl style="margin-top: 0.00em;margin-bottom: 0.00em;" class="list list-tag">
</dl>
</div>
<div class="section">
<h1 id="x43415645415453">CAVEATS</h1> <b class="name">elfdeps</b> does not actually resolve library dependencies, it ignores /etc/ld.so.cache, /etc/ld.so.conf and $LD_LIBRARY_PATH. It is designed to sacrifice accuracy for speed. For proper dependency resolution use <a class="link-man">ldd(1)</a>. The program scans each directory only once, multiple <i class="file">file s</i> sharing DT_RPATH, or DT_RPATH containing e.g. /usr/lib64, will not incur rescanning of the given directory. Files in a scanned directory is regarded as a potential library iff its name contains the string &quot;.so&quot;, they are never <a class="link-man">stat(2)'ed</a>, so they might be dangling symlinks, sockets, directories, or any non-shared-library file possible. The fact that <b class="name">elfdeps</b> claims that a library is either missing or found at some particular path does not answer wheter or not a library dependency is actually satisfied, but hopefully it should give an accrurate picture in most cases.</div>
<div class="section">
<h1 id="x4558414d504c45">EXAMPLE</h1> Find files with unsatified dependencies (with aforementioned caveats)<p>
<pre style="margin-left: 5.00ex;" class="lit display">
$ find / -xdev -type f -perm -100 -exec elfdeps {} + 2&gt;/dev/null | grep :missing$</pre>
</div>
<div class="section">
<h1 id="x42554753">BUGS</h1> DT_RPATH is (allegedly) deprecated, DT_RUNPATH should be queried as well, but isn't.</div>
<div class="section">
<h1 id="x415554484f52">AUTHOR</h1> Written by Lars Lindqvist.</div>
<div class="section">
<h1 id="x434f50595249474854">COPYRIGHT</h1> Copyright (C) 2013-2014 Lars Lindqvist</div>
<div class="section">
<h1 id="x4c4943454e5345">LICENSE</h1> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the &quot;Software&quot;), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:<div style="height: 1.00em;">
&#160;</div>
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.<div style="height: 1.00em;">
&#160;</div>
</div>
<div class="section">
<h1 id="x57415252414e5459">WARRANTY</h1> THE SOFTWARE IS PROVIDED &quot;AS IS&quot;, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.<div style="height: 1.00em;">
&#160;</div>
</div>
<div class="section">
<h1 id="x53454520414c5345">SEE ALSE</h1> <a class="link-man">ldd(1)</a>, <a class="link-man">readelf(1)</a>, <a class="link-man">objdump(1)</a>, <a class="link-man">elf(5)</a>, <a class="link-man">ld.so(8)</a>,</div>
<table summary="Document Footer" class="foot" width="100%">
<col width="50%">
<col width="50%">
<tbody>
<tr>
<td class="foot-date">
February 7, 2015</td>
<td class="foot-os" align="right">
</td>
</tr>
</tbody>
</table>
</div>
</body>
</html>

