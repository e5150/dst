#!/usr/bin/env python
# Copyright (c) 2013-2015 Lars Lindqvist
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import os, sys, string

ADM_PKG = '/var/log/packages'
DEPFILE = '/var/tmp/dst-deps-elfdeps-'

FILE_TO_PKG = {'missing' : 'missing'}

class Node:
	def __init__(self, name):
		self.name = name
		self.deps = set([])
		self.children = set([])
		self.parents = set([])
		self.circular = False
		self.traversed = False
		if name[0] == '/':
			self.short_name = os.path.basename(self.name)
		else:
			self.short_name = string.join(self.name.split('-')[:-3], '-')

	def add_dep(self, dep):
		self.deps.add(dep)

	def add_child(self, node):
		if self == node:
			# libJdbcOdbc.so ;/
			self.circular = True
		else:
			self.children.add(node)
			node.parents.add(self)

	def __repr__(self):
		return "name:\t%s\nshrt:\t%s\ndeps:\t%d\nkids:\t%d\npars:\t%d\ncirc:\t%s\ntrav:\t%s\n" % \
		(self.name, self.short_name, len(self.deps), len(self.children), len(self.parents), self.circular, self.traversed)

	def traverse(self, last = None, level = 0, up = False):
		prefix = level * '  '
		suffix = ''

		recurser = self.parents if up else self.children

#		if self.circular:
#			suffix += " /* CIRCULAR */"

		if not recurser:
			suffix += " /* %s */" % ("ROOT" if up else "LEAF") 
		elif self.traversed:
			suffix += " /* omitting */ "
		else:
			suffix += " {"

		print prefix + self.name + suffix

		if recurser and not self.traversed:
			self.traversed = True

			for node in recurser:
				node.traverse(self, level + 1, up)
			if recurser:
				print prefix + '} /* %s */' % self.short_name



def fix_pkg_caveats(file):
	if file.startswith('lib64/incoming/') \
	or file.startswith('lib/incoming/'):
		file = file.replace('/incoming', '')
	elif file == 'bin/bash4.new':
		file = 'bin/bash'
	elif file == 'sbin/init.new':
		file = 'sbin/init'
	return '/' + file


def fix_dep_caveats(file):
	# These are not actually ELF files:
	if file == '/usr/lib/libc.so':
		file = '/lib/libc.so.6'
	elif file == '/usr/lib64/libc.so':
		file = '/lib64/libc.so.6'

	if os.path.islink(file):
		file = os.path.realpath(file)
	return file


def read_pkgs():
	for pkg in os.listdir(ADM_PKG):
		f = open(os.path.join(ADM_PKG, pkg))
		while not f.readline() == 'FILE LIST:\n':
			pass
		f.readline() # skip "./"
		while f.readline().startswith('/install/'):
			pass
		for line in f.readlines():
			file = fix_pkg_caveats(line.rstrip('\n'))
			FILE_TO_PKG[file] = pkg


def mk_deptree():
	NODES = []
	ROOTS = []
	FILE_TO_NODE = {}

	f = open(DEPFILE, 'r')
	for line in f.readlines():
		(elf_file, dep, dep_file) = line.rstrip('\n').split(':')

		dep_file = fix_dep_caveats(dep_file)

		if not FILE_TO_NODE.has_key(elf_file):
			node = Node(elf_file)
			FILE_TO_NODE[elf_file] = node
			NODES.append(node)

		FILE_TO_NODE[elf_file].add_dep(dep_file)
	f.close()

	for node in NODES:
		for dep in node.deps:
			if FILE_TO_NODE.has_key(dep):
				dep_node = FILE_TO_NODE[dep]
			else:
				dep_node = Node(dep)
				FILE_TO_NODE[dep] = dep_node
				ROOTS.append(dep_node)
			dep_node.add_child(node)
	return (ROOTS, NODES, FILE_TO_NODE)

def mk_pkgtree(ROOTS_LIBS, ELF_NODES):
	NODES = []
	ROOTS = []
	PKG_TO_NODE = { 'missing' : Node('missing')}


	for node in ELF_NODES:
		pkg = FILE_TO_PKG[node.name]
		if not PKG_TO_NODE.has_key(pkg):
			node = Node(pkg)
			PKG_TO_NODE[pkg] = node
			NODES.append(node)
	
	for elf_node in ELF_NODES:
		pkg_name = FILE_TO_PKG[elf_node.name]
		pkg_node = PKG_TO_NODE[pkg_name]
		for elf_dep in elf_node.deps:
			dep_pkg = FILE_TO_PKG[elf_dep]
			dep_node = PKG_TO_NODE[dep_pkg]
			dep_node.add_child(pkg_node)
			

	for node in NODES:
		if len(node.parents) == 0:
			ROOTS.append(node)

	return (ROOTS, NODES, PKG_TO_NODE)


def pack_basename(pkg):
	ret = os.path.basename(pkg)
	for ext in ('.txz', '.tgz', '.tbz', '.tlz'):
		if ret.endswith(ext):
			ret = ret[:-4]
	return ret

if __name__ == '__main__':

	os.system('darkstar-makedep > /dev/null')

	(ROOT_LIBS, ELF_NODES, notused) = mk_deptree()

	# undocumented
	if '--libs' in sys.argv:
		for root in ROOT_LIBS:
			root.traverse()
		exit(0)

	if '--down' in sys.argv:
		pkg_up = False
		sys.argv.pop(sys.argv.index('--down'))
	else:
		pkg_up = True

	read_pkgs()

	(ROOT_PKGS, PKG_NODES, PKG_TO_NODE) = mk_pkgtree(ROOT_LIBS, ELF_NODES);

	if len(sys.argv) == 1:
		for root in ROOT_PKGS:
			root.traverse()
	else:
		for pkg in sys.argv[1:]:
			try:
				pkg = pack_basename(pkg)
				PKG_TO_NODE[pkg].traverse(pkg_up)
			except KeyError:
				sys.stderr.write("ERROR: %s does not look like a package name\n" % pkg)
