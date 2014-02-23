import math

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