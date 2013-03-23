from sysv_ipc import *
from os import fork, _exit, wait, getpid
import sys, signal

mutex = Semaphore(None, flags=IPC_CREX, initial_value=1)
H = Semaphore(None, flags=IPC_CREX, initial_value=0)
O = Semaphore(None, flags=IPC_CREX, initial_value=0)
B = Semaphore(None, flags=IPC_CREX, initial_value=1)

HCount = SharedMemory(None, flags=IPC_CREX)
OCount = SharedMemory(None, flags=IPC_CREX)
BCount = SharedMemory(None, flags=IPC_CREX)
HCount.write('0')
OCount.write('0')
BCount.write('0')

def increment(var):
    var.write(str(int(var.read()) + 1))
def decrement(var):
    var.write(str(int(var.read()) - 1))
def val(var):
    return int(var.read())

def oxygen_process():
    mutex.acquire()
    if val(HCount) > 2:
        BCount.write(str(2))
        H.release()
        H.release()
    else:
        increment(OCount)
        mutex.release()
        O.acquire()
        B.acquire()
        decrement(OCount)
        decrement(BCount)
        if val(BCount) != 0:
            B.release()
        else:
            B.release()
            mutex.release()
    print "BOND"

def hydrogen_process():
    mutex.acquire()
    if val(OCount) >= 1 and val(HCount) >= 1:
        BCount.write('2')
        O.release()
        H.release()
    else:
        increment(HCount)
        mutex.release()
        H.acquire()
        B.acquire()
        decrement(HCount)
        decrement(BCount)
        if val(BCount) != 0:
            B.release()
        else:
            B.release()
            mutex.release()

def remove_memory():
    HCount.remove()
    OCount.remove()
    BCount.remove()
    H.remove()
    O.remove()
    B.remove()
    mutex.remove()

def signal_handler(signal, frame):
    remove_memory()
    sys.exit(1)

signal.signal(signal.SIGINT, signal_handler)

if __name__ == "__main__":
    atoms = sys.argv[1]
    for atom in atoms:
        pid = fork()
        if pid == 0:
            if atom == 'o':
                oxygen_process()
                _exit(0)
            elif atom == 'h':
                hydrogen_process()
                _exit(0)
            else:
                print "ERROR: Invalid inputs"
                _exit(0)
    for atom in atoms:
        print "wait"
        wait()
    print "DONE!"
    remove_memory()
