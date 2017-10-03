"""
Some stuff:
>>> list(pack('I', 4294967295))
[255, 255, 255, 255]
>>> list(pack('I', 4294967294))
[254, 255, 255, 255]
>>> int.from_bytes(list(pack('I', 4294967294)),'little')
4294967294

So this should work for uint32_t from arduino...

"""
from struct import pack

def bytes2unint32(byteVec):
    return int.from_bytes(byteVec,byteorder='little', signed=False)


def go():
    for v in range(pow(2,32)-1):
        byteList  =  list(pack('I',v))
        print('Value :',
              v,
              ' Bytes :',
              byteList,
              ' Reassembled :',
              bytes2unint32(byteList))
        #input()
        

              
        
