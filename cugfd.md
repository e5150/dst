<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="resource-type" content="document">
<title>
CUFGF(1)</title>
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
CUFGF(1)</td>
<td class="head-vol" align="center">
darkstar-tools-14.0</td>
<td class="head-rtitle" align="right">
CUFGF(1)</td>
</tr>
</tbody>
</table>
<div class="section">
<h1 id="x4e414d45">NAME</h1> <b class="name">cugfd</b> &#8212; <span class="desc">change user/group/file/directory ownership and permissions</span></div>
<div class="section">
<h1 id="x53594e4f50534953">SYNOPSIS</h1><table class="synopsis">
<col style="width: 5.00ex;">
<col>
<tbody>
<tr>
<td>
cugfd</td>
<td>
&#91;<span class="opt"><b class="flag">&#45;KDq</b></span>&#93; &#91;<span class="opt"><b class="flag">&#45;u</b> <i class="arg">user</i></span>&#93; &#91;<span class="opt"><b class="flag">&#45;g</b> <i class="arg">group</i></span>&#93; &#91;<span class="opt"><b class="flag">&#45;f</b> <i class="arg">file-mode</i></span>&#93; &#91;<span class="opt"><b class="flag">&#45;d</b> <i class="arg">dir-mode</i></span>&#93; <i class="file">path ...</i><div style="height: 1.00em;">
&#160;</div>
</td>
</tr>
</tbody>
</table>
</div>
<div class="section">
<h1 id="x4445534352495054494f4e">DESCRIPTION</h1> <b class="name">cugfd</b> recursively changes ownership and/or permissions on files and directories. At least one of <b class="flag">&#45;u</b>, <b class="flag">&#45;g</b>, <b class="flag">&#45;f</b> or <b class="flag">&#45;d</b> must be given for any changes to be made, if either option is not given, the corresponding owership/permissions will be left unchanged.<div style="height: 1.00em;">
&#160;</div>
By default, the executable bit will be kept for those with read access.</div>
<div class="section">
<h1 id="x4f5054494f4e53">OPTIONS</h1><dl style="margin-top: 0.00em;margin-bottom: 0.00em;" class="list list-tag">
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;u</b> <i class="arg">user</i></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Change user ownership to <i class="arg">user</i>.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;g</b> <i class="arg">group</i></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Change group ownership to <i class="arg">group</i>.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;f</b> <i class="arg">file-mode</i></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Change mode of regular files to the octal <i class="arg">file-mode</i>.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;d</b> <i class="arg">dir-mode</i></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Change mode of directories to the octal <i class="arg">dir-mode</i>.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;K</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Don't keep executable bit on regular files.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;D</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Dry run, don't do any changes.</dd>
<dt class="list-tag" style="margin-top: 1.00em;">
<b class="flag">&#45;q</b></dt>
<dd class="list-tag" style="margin-left: 6.00ex;">
Quiet operation, only print warnings and errors.</dd>
</dl>
</div>
<div class="section">
<h1 id="x415554484f52">AUTHOR</h1> Written by Lars Lindqvist.</div>
<div class="section">
<h1 id="x434f50595249474854">COPYRIGHT</h1> Copyright (C) 2012-2013 Lars Lindqvist</div>
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
<h1 id="x53454520414c5345">SEE ALSE</h1> <a class="link-man">chmod(1)</a>, <a class="link-man">chown(1)</a>,</div>
<table summary="Document Footer" class="foot" width="100%">
<col width="50%">
<col width="50%">
<tbody>
<tr>
<td class="foot-date">
September 9, 2013</td>
<td class="foot-os" align="right">
</td>
</tr>
</tbody>
</table>
</div>
</body>
</html>

