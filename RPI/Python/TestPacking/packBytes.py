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

def packNbytes(bytes):
    """ This will pack unsigned bytes
    """
    return pack('B'*len(bytes),*bytes)

def unpackStruct( format, packed):
    """This will return a list of values
    after unpacking the packed bytes.
    Some formats:
    b/B : signed/unsigned byte 1
    h/H : signed/unsigned short int 2
    i/I : signed/unsigned int 4
    f   : float 4
    d   : double 8
    size and byte order:
    find the system byteorder by examining 
    sys.byteorder
    on Intel and RPI it is 'little'
    @ native order native size, native alignment (needs examination)
    = native order, standard size, no alignment
    </> little/big endian, standard size, no alignment
    !   big endian, standard size, no alignment
    """
    return unpack(format,packed)

"""
Examples (run on intel pc):
# vector of 2 bytes
>>> bytes2 = [7,255]
# unpack as signed bytes
>>> unpackStruct('bb',packNbytes(bytes2))
(7, -1)
# unpack as unsigned bytes
>>> unpackStruct('BB',packNbytes(bytes2))
(7, 255)
# vector of 4 bytes
>>> bytes4 = [1,0,0,0]
# concatenate and unpack as 2 unsigned 4 byte ints
>>> unpackStruct('II',packNbytes(bytes4+bytes4))
(1, 1)
# vector of 4 bytes
>>> bytes4 = [255,255,255,255]
# unpack as unsigned ints
>>> unpackStruct('II',packNbytes(bytes4+bytes4))
(4294967295, 4294967295)
#unpack as signed ints
>>> unpackStruct('ii',packNbytes(bytes4+bytes4))
(-1, -1)
"""

def goL():
    for v in range(pow(2,32)-1):
        byteList  =  list(pack('I',v))
        print('Value :',
              v,
              ' Bytes :',
              byteList,
              ' Reassembled :',
              unpackStruct( 'I',packNbytes(byteList))[0])
        #input()
        
def gof():
    v =0.0
    while (True):
        byteList  =  list(pack('f',v))
        print('Value :',
              v,
              ' Bytes :',
              byteList,
              ' Reassembled :',
              unpackStruct( 'f',packNbytes(byteList))[0])
        #input()
        v+=0.01

              
        
