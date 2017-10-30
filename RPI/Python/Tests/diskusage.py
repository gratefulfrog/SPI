#!/usr/bin/python3

"""
Return disk usage statistics about the given path as a (total, used, free)
namedtuple.  Values are expressed in bytes.

Memory usage as well
"""

import os, collections,psutil

_ntuple_diskusage = collections.namedtuple('usage', 'total used free')

oneMB = 1000000

def disk_usage(path):
    st = os.statvfs(path)
    free = st.f_bavail * st.f_frsize
    total = st.f_blocks * st.f_frsize
    used = (st.f_blocks - st.f_bfree) * st.f_frsize
    return _ntuple_diskusage(total, used, free)
def disk_freeMB(path):
    """ return disk free in MB
    """
    st = os.statvfs(path)
    free = st.f_bavail * st.f_frsize
    return  '{:.3f}'.format(free/oneMB)

def mem_freeMB():
    return '{:.3f}'.format(psutil.virtual_memory().available/oneMB)

import time
def run():
    while True:
        print('Disk Free :',
              disk_freeMB('.'),
              'MB',
              '\tMemory Free :',
              mem_freeMB(),
              'MB')
        time.sleep(10)
        


        
if __name__ == '__main__':
    run()
    """
    print ('Disk usage :',disk_usage(os.getcwd()))
    print ('Disk free MB :',disk_freeMB(os.getcwd()))
    print ('Percent free % limit 1GB :', round(100*disk_freeMB(os.getcwd())/1000,2), '%')
    """
