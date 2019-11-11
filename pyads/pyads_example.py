import pyads
from pyads.structs import AmsAddr

# x, y, z, a, b, c
cartesian_coordinates = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

pyads.open_port()

netID = '5.46.135.204.1.1'
port = 0x8888

target_addr = AmsAddr(netID, port)

while True:
    cartesian_coordinates[0], cartesian_coordinates[1], cartesian_coordinates[2] = eval(input("x,y,z: "))
    pyads.read_write(target_addr, 2, 1, pyads.PLCTYPE_BOOL, cartesian_coordinates, pyads.PLCTYPE_ARR_LREAL(6))

pyads.close_port()
