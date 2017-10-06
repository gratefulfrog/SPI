#!/usr/bin/python3.6
import queue
import threading


itemCount = 0
nbItems = 10000
num_worker_threads = 20

q = queue.Queue()
threads = []
def getItem():
    global itemCount
    res = [itemCount]
    itemCount +=1
    return res

def do_work(name,thing):
    print('\t\t', name,
          'Dequeue :', thing)

def worker(nm):
    while True:
        item = q.get()
        if item is None:
            break
        do_work(nm,item)
        q.task_done()

def createThreads():
    global threads
    for i in range(num_worker_threads):
        t = threading.Thread(args=["w-" + str(i)], target=worker)
        t.start()
        threads.append(t)

def enq():
    for i in range(nbItems):
        item = getItem()
        print('enqueue :', item)
        q.put(item)

def stopAll():
    # block until all tasks are done
    q.join()

    # stop workers
    for i in range(num_worker_threads):
        q.put(None)
    for t in threads:
        t.join()

def cycle():
    #threading.getIdent()
    #exit()
    if not threads:
        createThreads()
    enq()
    stopAll()


if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    cycle()


    
