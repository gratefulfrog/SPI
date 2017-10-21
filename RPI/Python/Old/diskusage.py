#!/usr/bin/python3

"""
Return disk usage statistics about the given path as a (total, used, free)
namedtuple.  Values are expressed in bytes.
"""
# Author: Giampaolo Rodola' <g.rodola [AT] gmail [DOT] com>
# License: MIT

import os, collections

_ntuple_diskusage = collections.namedtuple('usage', 'total used free')

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
    return  round(free/1000000,3)

import time
def run():
    while True:
        print('Disk Free :', disk_freeMB('.'), 'MB')
        time.sleep(10)
        

if __name__ == '__main__':
    print ('Disk usage :',disk_usage(os.getcwd()))
    print ('Disk free MB :',disk_freeMB(os.getcwd()))
    print ('Percent free % limit 1GB :', round(100*disk_freeMB(os.getcwd())/1000,2), '%')
