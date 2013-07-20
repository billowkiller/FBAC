#!/usr/bin/env python
# -*- coding:utf-8 -*-

import sys
import json
import urllib

f =  open(sys.argv[1], 'r')
obj = ''.join(f.read().split())
f.close()
d = json.loads(obj)
print len(d),
for k in d.keys():
	print k, len(d[k]),
	for item in d[k]:
		if type(item) is unicode:
			v = urllib.quote(item.encode('utf8'))
			if v.count('%')%3 != 0:
				v = v.replace('%2A', '*')
			print v,
		else:
			print item,
