#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os, sys
from getopt import *
from resource import *
import subprocess

FULL_ANS = {
	"OK":  "Answer correct",
	"ANS": "Wrong answer",
	"TLE": "Timeout",
	"RTE": "Runtime error",
	"SIG": "Sigkill",
	"SYS": "System error"
}

def run_test(exe, wejp):
	wej = os.open(wejp, os.O_RDONLY)
	subprocess.call(["touch", ".out"])
	wyj = os.open(".out", os.O_WRONLY)
	null = os.open("/dev/null", os.O_WRONLY)
	os.ftruncate(wyj, 0)
	used_time = getrusage(RUSAGE_CHILDREN).ru_utime
	try:
		proc = subprocess.Popen(["./"+exe], bufsize=0, stdin=wej, stdout=wyj, stderr=null)
		proc.wait(timeout=2)
	except subprocess.TimeoutExpired:
		proc.kill()
		os.close(wej)
		os.close(wyj)
		return "TLE", 2
	used_time = getrusage(RUSAGE_CHILDREN).ru_utime - used_time
	os.close(wej)
	os.close(wyj)
	info = ""
	if proc.returncode>0:   ans = "RTE"
	elif proc.returncode<0: ans = "SIG"
	else:
		wej = os.open(".out", os.O_RDONLY)
		cmp = subprocess.Popen(["./bin/verifier", wejp], bufsize=0,
			stdin=wej, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		info = cmp.communicate()[0].decode('utf-8')
		os.close(wej)
		ans = "OK" if cmp.returncode==0 else "ANS"
	return ans, used_time, info

def errc(c):
	return ("\x1b[32m" if c == 0 else "\x1b[31m") + "%3d" % c + "\x1b[0m"

def okc(c):
	return "\x1b[32m" + "%4d" % c + "\x1b[0m"

def check(exe, pref):
	cALL = cOK = cTLE = cANS = cRTE = cSIG = cSYS = 0
	wt = 0.0
	mt = 0.0
	tests = sorted(os.listdir('tests/'))
	print("╔═════╦════════════════════╦════════╦════════════════╦═════════════════════════╗")
	for test in tests:
		if test.startswith(pref) and test[-4:] == '.cnf':
			wej = 'tests/' + test

			b = ("%-18s"%test[:-4])
			print("║ %3d ║"%(cALL,), b, "║", end="")
			sys.stdout.flush()

			ans, time, info = run_test(exe, wej)

			c = ("\x1b[32m" if ans=="OK" else "\x1b[31m")
			c += ("%-14s"%FULL_ANS[ans]) + "\x1b[0m"
			info = info[:23].replace("\n", " ")
			print("%6.2fs"%(time,), "║", c, "║", "%-23s"%info, "║")

			wt += time
			if time > mt :
				mt = time
			cALL+=1
			if ans=="OK": cOK+=1
			elif ans=="TLE": cTLE+=1
			elif ans=="ANS": cANS+=1
			elif ans=="RTE": cRTE+=1
			elif ans=="SIG": cSIG+=1
			elif ans=="SYS": cSYS+=1
	cALL = "%4d" % cALL
	cOK = okc(cOK)
	cTLE = errc(cTLE)
	cANS = errc(cANS)
	cRTE = errc(cRTE)
	cSIG = errc(cSIG)
	cSYS = errc(cSYS)
	print("╠═════╩════════╦═══════════╩═══════╦╩═════╦══════╦═══╩═╦═════╦═════╦═════╦═════╣")
	print("║ Total time   ║ %16.2fs ║ ALL  ║ OK   ║ TLE ║ ANS ║ RTE ║ SIG ║ SYS ║" % wt)
	s = "║ Max time     ║ %16.2fs ║" + " %s ║"*7
	print(s % (mt, cALL, cOK, cTLE, cANS, cRTE, cSIG, cSYS))
	print("╚══════════════╩═══════════════════╩══════╩══════╩═════╩═════╩═════╩═════╩═════╝")

def is_exe(f):
	return os.path.isfile(f) and os.access(f, os.X_OK)

def is_dir(f):
	return os.path.isdir(f)

def main():
	if len(sys.argv) != 2 and len(sys.argv) != 3:
		print("Usage: benchmark.py BIN [TEST_PREF]")
		sys.exit(0)
	if not is_exe(sys.argv[1]):
		print("Wrong binary file (check isfile & isexecutable)")
		print("Usage: benchmark.py BIN [TEST_PREF]")
		sys.exit(0)
	if len(sys.argv) < 3:
		check(sys.argv[1], "")
	else:
		check(sys.argv[1], sys.argv[2])

if __name__=="__main__": main()

