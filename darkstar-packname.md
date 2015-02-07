<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="resource-type" content="document">
<title>
DARKSTAR-PACKNAME(1)</title>
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
DARKSTAR-PACKNAME(1)</td>
<td class="head-vol" align="center">
darkstar-tools-14.1</td>
<td class="head-rtitle" align="right">
DARKSTAR-PACKNAME(1)</td>
</tr>
</tbody>
</table>
<div class="section">
<h1 id="x4e414d45">NAME</h1> <b class="name">darkstar-packname</b> &#8212; <span class="desc">extract package information from filename</span></div>
<div class="section">
<h1 id="x53594e4f50534953">SYNOPSIS</h1><table class="synopsis">
<col style="width: 17.00ex;">
<col>
<tbody>
<tr>
<td>
darkstar-packname</td>
<td>
&#91;<span class="opt"><b class="flag">&#45;n</b> | <b class="flag">&#45;V</b> | <b class="flag">&#45;a</b> | <b class="flag">&#45;t</b> | <b class="flag">&#45;B</b> | <b class="flag">&#45;e</b> | <b class="flag">&#45;b</b> | <b class="flag">&#45;v</b></span>&#93; <i class="arg">filename ...</i></td>
</tr>
</tbody>
</table>
</div>
<div class="section">
<h1 id="x4445534352495054494f4e">DESCRIPTION</h1> <b class="name">darkstar-packname</b> extracts package information from slackware package filenames that adheres to the &lt;name&gt;-&lt;version&gt;-&lt;arch&gt;-&lt;build&gt;&lt;tag&gt;[.ext] format, where ext is one of tgz, txz, tbz or tlz, and only the name part may contain a '-'. Any leading path is ignored.</div>
<div class="section">
<h1 id="x4f5054494f4e53">OPTIONS</h1><dl style="margin-top: 0.00em;margin-bottom: 0.00em;" class="list list-tag">
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;n</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print package name (default operation). Is (designed to be) equivalent to package_name() function in installpkg(8).</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;V</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print package version.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;a</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print package arch.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;t</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print package tag.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;B</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print package build (numeric only, non-numeric strings are regarded as &quot;tag&quot;).</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;e</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print package extension.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;b</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Print package basename, without extension. Equivalent to pkgbase() in installpkg(8).</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;v</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Verbose operation, print all available information in the comma separated format &quot;&lt;arg&gt;:&lt;key&gt;:&lt;val&gt;&quot;, where arg is the <i class="arg">filename</i>, key is one of &quot;NAME&quot;, &quot;VERSION&quot;, &quot;ARCH&quot;, &quot;BUILD&quot;, &quot;TAG&quot;, &quot;EXT&quot; and &quot;BASE&quot;, and val is the corresponding (potentially empty) string for the given package.</dd>
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

