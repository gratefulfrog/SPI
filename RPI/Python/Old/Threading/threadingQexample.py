#!/usr/bin/python3

import queue
import threading
import time, sys

q = queue.Queue()

def getItem(itemCount):
    return [itemCount]


def do_work(name,thing):
    pass
    #print('\t\t', name,'Dequeue :', thing)

def worker(nm):
    while True:
        item = q.get()
        if item is None:
            break
        do_work(nm,item)
        q.task_done()

def createThreads(num):
    thrs = []
    for i in range(num):
        t = threading.Thread(args=["w-" + str(i)], target=worker)
        t.start()
        thrs.append(t)
    return thrs

def enq(nbItems):
    for i in range(nbItems):
        item = getItem(i)
        #print('enqueue :', item)
        q.put(item)

def stopAll(num,thr):
    # block until all tasks are done
    q.join()

    # stop workers
    for i in range(num):
        q.put(None)
    for t in thr:
        t.join()

def cycle(num,nbItems):
    #threading.getIdent()
    #exit()
    threads =  createThreads(num)
    t = time.time()
    enq(nbItems)
    stopAll(num,threads)
    print(num, time.time() -t)

if __name__ == '__main__':
    cycle(int(sys.argv[1]),10000)

    


    
