

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

from struct import pack,unpack

def packNbytes(bytes):
    """ 
    This will return a pack of unsigned bytes, suitable for unpacking into their
    original values.
    the argument: bytes : is a list of any length of 
    unsigned byte values.
    [255, 0, 1, ...]
    returns packed bytes of the form:
    >>> packNbytes(sss)
    b'\x01\n\x00\x00\x00\x00\x00\xc0?'
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
    the correct choice is '<' to get little endian, which is what is needed 
    for Arduino and RPI and PC/Intel
    Will return a list of things corresponding to format spec.
    Ex:
    if we have an unsigned byte, unsigned long, float = 9bytes
    >>> unpack('<BIf' packNbytes([<9 numbers that are on [0,255]])
    [uint8_t, uint32_t, f]
    """
    return unpack(format,packed)


"""
**************** experimantal stuff follows ********************

v1 = [0,0,0b100100, 0b1000001]
v2 = [0b110011,0b110011, 0b11110011, 0b111111]


typeDict = {0b1000:[1,'<B'],   # type 1000, 1 byte, i.e. one unsigned byte  i.e. uint8_t
            0b1001:[4,'<I'],   # type 1001, 4 bytes, i.e. one unsigned Long i.e. uint32_t
            0b1010:[4,'<f'],   # type 1010, 4 bytes, i.e. one float         i.e. float
            0b1011:[9,'<BIf']} # type 1011, 9 bytes struct,                 i.e. uint8_t,uint32_t float


"""

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

>>> unpackStruct('<bIf',packNbytes(sss))
(1, 10, 1.5)

It fucking works!!!

"""

sss = [1,  
       10, 
       0,  
       0,  
       0,  
       0,  
       0,  
       192,
       63]

def goL():
    for v in range(pow(2,32)-1):
        byteList  =  list(pack('I',v))
        print('Value :',
              v,
              ' Bytes :',
              byteList,
              ' Reassembled :',
              unpackStruct( '<I',packNbytes(byteList))[0])
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
              unpackStruct( '<f',packNbytes(byteList))[0])
        #input()
        v+=0.01

def goS():
    try:
        v=[1,1,1.0]
        lims = [256, pow(2,32)-1, 5.0]
        while (True):
            byteList  =  list(pack('<BIf',*v))
            res =  unpackStruct( '<BIf',packNbytes(byteList))
            ok =  all(map(lambda x,y:abs(x-y)<0.05,v,res))
            print('OK:' if ok else 'NOK:',
                  'Value :',
                  [round(x,2) for x in v],
                  'Bytes :',
                  byteList,
                  ' Reassembled :',
                  [round(x,2) for x in res])
            if (not ok):
                break
            v = list(map(lambda x,l:(x+1)%l,v,lims))
    except KeyboardInterrupt:
        print('bye...')
              
""" 
the timeValStruct used in the AEM board is defined as:
* byte, 
* uin32_t, 
* float
which looks like 1 + 4 + 4 = 9 bytes.

But lets see what the arduino says about that!
Yes!!

byteUint32Float: 1, 10, 1.50
NbBytes : 9
Byte : 0 : 1
Byte : 1 : 10
Byte : 2 : 0
Byte : 3 : 0
Byte : 4 : 0
Byte : 5 : 0
Byte : 6 : 0
Byte : 7 : 192
Byte : 8 : 63 


"""


"""  obsolete

def bytes2unint32(byteVec):
    return int.from_bytes(byteVec,byteorder='little', signed=False)

def bytes2float(byteVec):
    return unpack('f',byteVec)[0]

def floatBytes2ByteVec (bytes):
    return pack('4B', *bytes)

def doFloat(listOfCBytes):
    #take a list of bytes of length 4 
    #and return the corresponding float value
    return bytes2float(floatBytes2ByteVec(listOfCBytes))
"""
"""
#!/usr/local/bin/python3
import sys

toto = 27
if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    print(eval(sys.argv[1]))
"""
