
## in client.py
import socket

HOST = '127.0.0.1'    # The remote host
PORT = 8000              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
import simplekml
#kml = simplekml.Kml()
coordinates = []

import csv
import easygui as eg
filename = eg.enterbox(msg='Enter name for series')
myfile = eg.diropenbox() + '\\data' + '_' + filename + '.kml'



temp = '\x00'*4 + 'm' + '\x00'*31
s.send(temp)
#s.close()
#print 'Received', repr(data)
while True:
    data = s.recv(1024)
    data = data.split('\r/')
    data = data[1]
    data = data.split('/')
    lat = data[0]
    lon = data[1]
    lat = lat.partition('h')
    lat = lat[2]
    lat = lat.split('N')
    lat = lat[0]
    lat = float(lat[:2]) + float(lat[2:])/60.0

    lon = lon.partition('W')
    lon = lon[0]
    lon = -1.0*(float(lon[:3]) + float(lon[3:])/60.0)

    print lat
    print lon

    coordinates.append((lon,lat))
    kml = simplekml.Kml()
    kml.newlinestring(name="Pathway", description="track testing",
                        coords=coordinates)
    #kml.newpoint(name="point", coords=[(lon,lat)])
    kml.save(myfile)
