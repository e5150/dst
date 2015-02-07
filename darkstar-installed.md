<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="resource-type" content="document">
<title>
DARKSTAR-INSTALLED(1)</title>
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
DARKSTAR-INSTALLED(1)</td>
<td class="head-vol" align="center">
darkstar-tools-14.1</td>
<td class="head-rtitle" align="right">
DARKSTAR-INSTALLED(1)</td>
</tr>
</tbody>
</table>
<div class="section">
<h1 id="x4e414d45">NAME</h1> <b class="name">darkstar-installed</b> &#8212; <span class="desc">querys the package database for installed packages</span></div>
<div class="section">
<h1 id="x53594e4f50534953">SYNOPSIS</h1><table class="synopsis">
<col style="width: 18.00ex;">
<col>
<tbody>
<tr>
<td>
darkstar-installed</td>
<td>
<i class="arg">package name ...</i></td>
</tr>
</tbody>
</table>
</div>
<div class="section">
<h1 id="x4445534352495054494f4e">DESCRIPTION</h1> <b class="name">darkstar-installed</b> looks in /var/log/packages for installed packages and prints the path to the database file for the corresponding package, if found.</div>
<div class="section">
<h1 id="x415247554d454e5453">ARGUMENTS</h1> <i class="arg">package name</i> may be a package filename, e.g. &quot;nc-1.10-x86_64-1.txz&quot; or a package basename, e.g. &quot;xz-5.0.5-x86_64-1&quot;, either optionally with a leading path, or simply a package name, e.g. &quot;xz&quot; or &quot;glibc-solibs&quot;.</div>
<div class="section">
<h1 id="x4558495420535441545553">EXIT STATUS</h1><dl style="margin-top: 0.00em;margin-bottom: 0.00em;" class="list list-tag">
<dt class="list-tag" style="margin-top: 1.00em;">
0</dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
All packages are installed</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
1</dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Any package is not installed, or error.</dd>
</dl>
</div>
<div class="section">
<h1 id="x415554484f52">AUTHOR</h1> Written by Lars Lindqvist.</div>
<div class="section">
<h1 id="x434f50595249474854">COPYRIGHT</h1> Copyright (C) 2012-2014 Lars Lindqvist</div>
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
<h1 id="x53454520414c5345">SEE ALSE</h1> <a class="link-man">darkstar-packname(1)</a>, <a class="link-man">darkstar-packcontent(1)</a>.</div>
<table summary="Document Footer" class="foot" width="100%">
<col width="50%">
<col width="50%">
<tbody>
<tr>
<td class="foot-date">
April 17, 2014</td>
<td class="foot-os" align="right">
</td>
</tr>
</tbody>
</table>
</div>
</body>
</html>

