#!/usr/bin/python

import urllib
import time
import sys
import tempfile
import subprocess
import shutil
import os


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

work_loop('main')
#notreached
sys.exit(0)
