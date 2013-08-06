from urllib2 import urlopen
from json import loads

def fetchID(id):
	content = loads(urlopen('http://graph.facebook.com/'+id).read())
	return content['username']+' '+content['id']
