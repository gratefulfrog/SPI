"""
Some stuff:
>>> list(pack('I', 4294967295))
[255, 255, 255, 255]
>>> list(pack('I', 4294967294))
[254, 255, 255, 255]
>>> int.from_bytes(list(pack('I', 4294967294)),'little')
4294967294

So this should work for uint32_t from arduino...

Now on to floats:
10.25
NbBytes : 4
Byte : 0 : 0
Byte : 1 : 0
Byte : 2 : 100100
Byte : 3 : 1000001

>> v = [0,0,0b100100, 0b1000001]
>> doFloat(V)
10.25

1.90
NbBytes : 4
Byte : 0 : 110011
Byte : 1 : 110011
Byte : 2 : 11110011
Byte : 3 : 111111

>> v = [0b110011,0b110011, 0b11110011, 0b111111]
>> doFloat(v)
1.899999976158142

"""

v1 = [0,0,0b100100, 0b1000001]
v2 = [0b110011,0b110011, 0b11110011, 0b111111]
from struct import pack,unpack

def bytes2unint32(byteVec):
    return int.from_bytes(byteVec,byteorder='little', signed=False)

def bytes2float(byteVec):
    return unpack('f',byteVec)[0]

def floatBytes2ByteVec (bytes):
    return pack('4B', *bytes)

def doFloat(listOfCBytes):
    return bytes2float(floatBytes2ByteVec(listOfCBytes))


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
        

              
        
