#!/usr/bin/python

import os
import errno
import pyMagic

def test1():
	"""simple test to invoke the syscalls """
	
	pid = os.getpid()

	pyMagic.get_wand(1, "secret")
	
	# Try to attack ourselves
	try:
		pyMagic.attack(pid)
	except OSError, e:
		assert(e.errno == errno.ECONNREFUSED)

	pyMagic.legilimens(pid)
	secrets, remaining = pyMagic.list_secrets(5)
	assert remaining == 0
	assert len(secrets) == 0

if __name__ == "__main__":
	test1()
