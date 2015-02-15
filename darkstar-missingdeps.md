<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="resource-type" content="document">
<title>
DARKSTAR-MISSINGDEPS(1)</title>
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
DARKSTAR-MISSINGDEPS(1)</td>
<td class="head-vol" align="center">
darkstar-tools-14.1</td>
<td class="head-rtitle" align="right">
DARKSTAR-MISSINGDEPS(1)</td>
</tr>
</tbody>
</table>
<div class="section">
<h1 id="x4e414d45">NAME</h1> <b class="name">darkstar-missingdeps</b> &#8212; <span class="desc">Prints files with unresolved dependencies.</span></div>
<div class="section">
<h1 id="x53594e4f50534953">SYNOPSIS</h1><table class="synopsis">
<col style="width: 20.00ex;">
<col>
<tbody>
<tr>
<td>
darkstar-missingdeps</td>
<td>
&#91;<span class="opt"><b class="flag">&#45;l</b></span>&#93; &#91;<span class="opt"><b class="flag">&#45;f</b></span>&#93;</td>
</tr>
</tbody>
</table>
</div>
<div class="section">
<h1 id="x4445534352495054494f4e">DESCRIPTION</h1> <b class="name">darkstar-missingdeps</b> prints a list of files found on the system who are dynamically linked to some library that was not found, it useses <a class="link-man">darkstar-makedep(1)</a> to do this.</div>
<div class="section">
<h1 id="x4f5054494f4e53">OPTIONS</h1><dl style="margin-top: 0.00em;margin-bottom: 0.00em;" class="list list-tag">
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;f</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Force <a class="link-man">darkstar-makedep(1)</a> to rebuild the dependency database.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;l</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Tell <a class="link-man">darkstar-makedep(1)</a> to use <a class="link-man">ldd(1)</a>, rather than <a class="link-man">elfdeps(1)</a>, to build the database</dd>
</dl>
</div>
<div class="section">
<h1 id="x415554484f52">AUTHOR</h1> Written by Lars Lindqvist.</div>
<div class="section">
<h1 id="x434f50595249474854">COPYRIGHT</h1> Copyright (C) 2011 Lars Lindqvist</div>
<div class="section">
<h1 id="x4c4943454e5345">LICENSE</h1> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the &quot;Software&quot;), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:<div style="height: 1.00em;">
&#160;</div>
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.</div>
<div class="section">
<h1 id="x57415252414e5459">WARRANTY</h1> THE SOFTWARE IS PROVIDED &quot;AS IS&quot;, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.<div style="height: 1.00em;">
&#160;</div>
</div>
<div class="section">
<h1 id="x53454520414c5345">SEE ALSE</h1> <a class="link-man">darkstar-makedep(1)</a>, <a class="link-man">elfdeps(1)</a>, <a class="link-man">ldd(1)</a>.</div>
<table summary="Document Footer" class="foot" width="100%">
<col width="50%">
<col width="50%">
<tbody>
<tr>
<td class="foot-date">
February 15, 2015</td>
<td class="foot-os" align="right">
</td>
</tr>
</tbody>
</table>
</div>
</body>
</html>

