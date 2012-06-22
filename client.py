#!/usr/bin/python

import urllib
import time
import sys
import tempfile
import subprocess
import shutil
import os
import threading


url=(sys.argv[1] + '?')

def work_loop(loopname):
    while True:
        getwork= urllib.urlopen((url + 'getwork'), proxies={})
        worku = getwork.readline().strip()
        if worku.startswith('sleep'):
            (junk, stime)=worku.strip().rsplit(':',1)
            print "%s sleeping %s" % (time.asctime(), stime)
            time.sleep(float(stime))
            continue
        if worku.startswith('exit'):
            sys.exit(0)
        (job, mywork) = worku.split(':',1)
        hashes=getwork.readlines()
        getwork.close()

# Make a temp hashfile
        hashfile = tempfile.NamedTemporaryFile()
        for line in hashes:
            hashfile.write(line)
        hashfile.flush()

# Do the work
        mywork = ' '.join(mywork.split(':'))
        mycmd = ('./plugin ' + mywork + ' |./john --stdin --session=' +
        loopname +' ' + hashfile.name)
        print "doing %s" % mycmd
        doing = subprocess.Popen(mycmd, shell=True)
        res = doing.wait()
        if not res == 0:
            print "failed code %s" % res
            sys.exit(1)

# Determine result of chunk
        result='notfound'
        if not os.path.exists('john.oldpot'):
            shutil.copyfile('john.pot', 'john.oldpot')
        if os.stat('john.pot').st_mtime > os.stat('john.oldpot').st_mtime:
            oldpot=open('john.oldpot').readlines()
            newpot=open('john.pot').readlines()
            shutil.copyfile('john.pot', 'john.oldpot')
            for line in newpot:
                if not line in oldpot:
                    result=urllib.urlencode({'hash': line.strip()})

# sent back result
        goodbye = urllib.urlopen((url + 'res:' + worku + ':' + result),
                    proxies={})
        buf = goodbye.readlines()
        goodbye.close()
        hashfile.close()


class workThread(threading.Thread):
    def __init__(self, threadID, tname):
        self.threadID=threadID
        self.tname=tname
        threading.Thread.__init__(self)
    def run(self):
        print "starting thread %s" % self.tname
        work_loop(self.tname)
        

cpucount = int(os.sysconf('SC_NPROCESSORS_ONLN'))

i=0
threads=[]
while i < cpucount:
    thread=workThread(i, ('thread'+str(i)))
    thread.start()
    threads.append(thread)
    i+=1
    time.sleep(5)


for t in threads:
    t.join()

sys.exit(0)
