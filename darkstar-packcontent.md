<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="resource-type" content="document">
<title>
DARKSTAR-PACKCONTENT(1)</title>
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
DARKSTAR-PACKCONTENT(1)</td>
<td class="head-vol" align="center">
darkstar-tools-14.1</td>
<td class="head-rtitle" align="right">
DARKSTAR-PACKCONTENT(1)</td>
</tr>
</tbody>
</table>
<div class="section">
<h1 id="x4e414d45">NAME</h1> <b class="name">darkstar-packcontent</b> &#8212; <span class="desc">prints content of installed package(s)</span></div>
<div class="section">
<h1 id="x53594e4f50534953">SYNOPSIS</h1><table class="synopsis">
<col style="width: 20.00ex;">
<col>
<tbody>
<tr>
<td>
darkstar-packcontent</td>
<td>
&#91;<span class="opt"><b class="flag">&#45;vd</b></span>&#93; &#91;<span class="opt"><b class="flag">&#45;p</b> | <b class="flag">&#45;c</b> | <b class="flag">&#45;e</b> | <b class="flag">&#45;m</b></span>&#93; <i class="file">package-database-file ...</i></td>
</tr>
</tbody>
</table>
<br>
<table class="synopsis">
<col style="width: 20.00ex;">
<col>
<tbody>
<tr>
<td>
darkstar-packcontent -s</td>
<td>
<i class="file">package-database-file ...</i></td>
</tr>
</tbody>
</table>
</div>
<div class="section">
<h1 id="x4445534352495054494f4e">DESCRIPTION</h1> <b class="name">darkstar-packcontent</b> prints information about the files belonging to an installed package. It reads the the &quot;FILE LIST:&quot; section of a packge, and can find out if a file or directory is still present on the system or not, and compute the actual installed size of a package.</div>
<div class="section">
<h1 id="x4f5054494f4e53">OPTIONS</h1><dl style="margin-top: 0.00em;margin-bottom: 0.00em;" class="list list-tag">
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;v</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Verbose output, prepend lines with &quot;&lt;package name&gt;:&quot;</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;d</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Ignore directories.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;p</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Only print the file list (default behaviour).</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;c</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Check for files, appends output with &quot;:present&quot; or &quot;:missing&quot;.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;e</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print only existing files.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;m</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print only missing files.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;s</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print the total size of the files that are present on the system.</dd>
</dl>
</div>
<div class="section">
<h1 id="x4e4f544553">NOTES</h1> The /install directory is always ignored, since it should never actually stay on the file system when a package has been properly installed.<div style="height: 1.00em;">
&#160;</div>
The &quot;.new&quot; suffix gets stripped from filenames, since install/doinst.sh will rename such files on a clean system, might cause false results.<div style="height: 1.00em;">
&#160;</div>
To accommodate glibc{,-solibs} lib/incoming/ and lib64/incoming/ are truncated to lib/ and lib64/, respectively.</div>
<div class="section">
<h1 id="x415554484f52">AUTHOR</h1> Written by Lars Lindqvist.</div>
<div class="section">
<h1 id="x434f50595249474854">COPYRIGHT</h1> Copyright (C) 2012-2015 Lars Lindqvist</div>
<div class="section">
<h1 id="x4c4943454e5345">LICENSE</h1> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the &quot;Software&quot;), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:<div style="height: 1.00em;">
&#160;</div>
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.</div>
<div class="section">
<h1 id="x57415252414e5459">WARRANTY</h1> THE SOFTWARE IS PROVIDED &quot;AS IS&quot;, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.<div style="height: 1.00em;">
&#160;</div>
</div>
<div class="section">
<h1 id="x53454520414c5345">SEE ALSE</h1> <a class="link-man">darkstar-installed(1)</a>,</div>
<table summary="Document Footer" class="foot" width="100%">
<col width="50%">
<col width="50%">
<tbody>
<tr>
<td class="foot-date">
April 16, 2014</td>
<td class="foot-os" align="right">
</td>
</tr>
</tbody>
</table>
</div>
</body>
</html>

