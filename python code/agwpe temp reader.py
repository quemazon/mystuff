import socket
import csv
import easygui as eg
import math
import matplotlib.pyplot as plt


filename = eg.filesavebox(msg=None, title=None, default='*', filetypes=None)

##myfile = csv.writer(open(filename, 'ab'), delimiter=' ',
##    quotechar='|', quoting=csv.QUOTE_MINIMAL)

time = []
therm1 = []
therm2 = []
therm3 = []
therm4 = []

class shtherm:
    def __init__(self, a, b, c, Rr):
        self.a = a
        self.b = b
        self.c = c
        self.Rr = Rr
    def res(self, adc):
        return self.Rr*(1024.0/adc - 1)
    def tempk(self, raw):
        tmp = math.log(self.res(raw))
        #print tmp
        return 1/(self.a + self.b * tmp + self.c * math.pow(tmp, 3))
    def tempc(self, raw):
        return self.tempk(raw) - 273.15
    def tempf(self, raw):
        return self.tempc(raw)*9/5 +32.0

class getherm:
    def __init__(self, Rr, R25):
        self.Rr = Rr
        self.R25 = R25
    def res(self, adc):
        return self.Rr*(1024.0/adc - 1)
    def tempk(self, raw):
        tmp = self.res(raw)/self.R25
        tmpln = math.log(tmp)
        if tmp > 3.195:
            a,b,c,d = 3.3545590e-3, 2.5903082e-4,4.1929419e-6,-7.1497776e-8
        elif tmp > 0.3636:
            a,b,c,d = 3.3540178e-3,2.6021087e-4,3.5946173e-6,-8.5676875e-8
        elif tmp >0.06933:
            a,b,c,d = 3.3531474e-3,2.5743868e-4,1.7022402e-6,-8.8297492e-8
        else:
            a,b,c,d = 3.3547977e-3,2.5879299e-4,1.8964602e-6,-1.1884916e-7
        return 1/(a + b * tmpln + c * math.pow(tmpln, 2) + d * math.pow(tmpln, 3))
    def tempc(self, raw):
        return self.tempk(raw) - 273.15
    def tempf(self, raw):
        return self.tempc(raw)*9/5 +32.0

class coefftherm:
    def __init__(self, Rr, beta, R25):
        self.beta = beta
        self.R25 = R25
        self.Rr= Rr
    def res(self, adc):
        return self.Rr*(1024.0/adc - 1)
    def tempk(self, raw):
        return 1/(1/298.15+math.log(self.res(raw)/self.R25)/self.beta)
    def tempc(self, raw):
        return self.tempk(raw) - 273.15
    def tempf(self, raw):
        return self.tempc(raw)*9/5 +32.0


inside = getherm(33, 30)
outside = shtherm(0.9521120733e-3, 3.056196423e-4, -1.216482627e-7, 10000)
inside2 = shtherm(3.936848245e-3, -2.066927907e-4, 11.84338466e-7, 82000)

HOST = '127.0.0.1'    # The remote host
PORT = 8000              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

temp = '\x00'*4 + 'm' + '\x00'*31
s.send(temp)

plt.ion()

while True:
    data = s.recv(1024)
    temp = data.split('\r/')[1]
    temp = temp.split('\r')[0]
    temp = temp.split(',')
    time.append(float(temp[0]))
    therm1.append(inside.tempf(float(temp[1])))
    therm2.append(outside.tempf(float(temp[3])))
    therm3.append(inside2.tempf(float(temp[4])))

    with open(filename, 'ab') as f:
        writer = csv.writer(f)
        writer.writerow(temp)
    f.close
##    plt.plot(time, therm1, 'b^', time, therm3, 'g>')
    plt.plot(time, therm1, 'b^', time, therm3, 'g>')
    plt.show()
