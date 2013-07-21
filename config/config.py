#!/usr/bin/env python
# -*- coding:utf-8 -*-
import re
import sys
import json
import urllib

def u(s, encoding):
	if isinstance(s, unicode):
		return s
	else:
		return unicode(s, encoding)

f =  open(sys.argv[1], 'r')
obj = ''.join(f.read().split())
f.close()
d = json.loads(obj)
print len(d),
for k in d.keys():
	print k, len(d[k]),
	for item in d[k]:
		if type(item) is unicode:
			relist = re.findall(ur"[\u4e00-\u9fa5]+", item)
			for r in relist:
				r_new = urllib.quote(r.encode('utf8'))
				item = item.replace(r, r_new)
			print item,
		else:
			print item,


