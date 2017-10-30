###############################################################################
# file : producer-consumer.py
# date : 2006 06 03
# role : demonstrate producer consumer paradigm
# author: gf_at_gratefulfrog_dot_net
# This file attempts to implement the socket example code found in the
# Python Reference Doc.  
###############################################################################

from sets import Set
from socket import *
import time, os, random

PORT = 50007   # Arbitrary non-privileged server
# change the next variable to the name of the host that will serve the
# panels out:
ServerHostName='UX303LNB' # 'bartbox'

# this is the message sent when there are no more panels:
NoMorePanels   = "EOP"

# this is the number of panels we have, for demo purposes only.
nbPanels  = 20
# this is a local panel id counter, for demo only.
panelId   = nbPanels
 
# this is a list of imaginary hosts on your network
# this is only needed in case you run the demo on one host
# comment out this line and remove comments from line following it
# when running on physically different hosts.
#hostNames = ["HAL", "MIKE", "DEEP-BLUE", "LISPY"]
hostNames = None

def nextPanelNameIterator(init=False):
	""" return the next panel name, or NoMorePanels if no more
	panel names can be found
	"""
	global panelId
	if init:
		panelId = nbPanels
		return
	if panelId > 0:
		res = 'Panel-' + str(panelId)
		panelId = panelId - 1
	else:	
		res = NoMorePanels
	return res
		
def Producer():
	"""
	Server function will listen on the local host's port defined
	by the local 
	variable PORT and accept connections as they are requested by
	any number of 
	local or remote clients.
	The function starts by creating a socket,
	  then binds it to a host and port pair,
	  and starts to listen at the socket thus defined.
        Next an infinite loop starts in which:
	  the server waits to accept a connection on the socket,
	  it prints some info about the connecting process,
	  and a second infinite loop starts in which:
        if data is available, this data is received, printed and
	      returned to sender,
	    if not, then the connection is closed and the internal loop exits.
	  if the data received is the serverr shutdown message, then
	    the external loop exits.
	"""
	nextPanelNameIterator(True)
	s = socket(AF_INET, SOCK_STREAM)
	s.bind((ServerHostName, PORT))
	s.listen(1)
	stop = False
	connected = Set([])
	print "Server starting up..."
	while (not stop) or (len(connected) > 0) :
		conn, addr = s.accept()
		while 1:
			data = conn.recv(1024)
			if not data:
				break
			else:
				print "Server Connected by: ",data
				connected.add(data)
				outgoing = nextPanelNameIterator()
				print "Now sending: " + outgoing
				print "Consumers connected = %s" \
				      % str(connected)[4:-1]
				conn.send(outgoing)
				if outgoing == NoMorePanels:
					stop = True
					connected.remove(data)
					print "Consumers connected = %s" \
					      % str(connected)[4:-1]
					print "Server Shutting down..."
		conn.close()

# number of seconds between client calls for panels.
clientDelay = random.randint(1,5)

def Consumer():
	"""
	Client function will connect to the PORT on the ServerHostName, which should
	already be listening!
	Next a loop starts in which:
	  the client creates a socket,
	  the socket is connected to the server ServerHostName and PORT,
	  send text is assembled consisting of pid and send number,
	  send text is sent on the socket to the server,
	  data is received from the server, then printed,
	  the socket is closed so that the server can receive connections from
	    other clients,
	  the client sleeps for Delay seconds, i.e. no CPU, then loops until
	    counter is reached.
	After exiting the loop, if the client is the master,
	  then a server shutdown message is sent.
	"""
	if not hostNames:
		host =gethostname()
	else:
		host = hostNames[random.randint(0, len(hostNames)) -1] \
		       + ':' + str(os.getpid())

	# counts the number of times I call for a panel!
	counter = 0
	stop = False
	print "Consumer %s starting up..." % host
	while not stop:
		counter = counter+1
		print "Consumer %s calling for panel %d" \
		      % (host, counter)
		s = socket(AF_INET, SOCK_STREAM)
		s.connect((ServerHostName, PORT))
		sendText = host 
		s.send(sendText)
		data = s.recv(1024)
		s.close()
		print 'Received', `data`
		time.sleep(clientDelay)
		if data == NoMorePanels:
			msg = "Consumer %s Shutting down" \
			      " after processing %d panels..."
			print msg % (host, counter-1)
			stop = True
	return



