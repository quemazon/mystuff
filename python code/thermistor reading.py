
#/*******************
#   Thermistor setup
#**********************

#******* thermistor 1 is SH model
coeff_1a =
coeff_1b =
coeff_1c =
Rref_1 =

#******* thermistor 2 is SH model
beta_2 =
R25_2 =
Rref_2 =

#******** thermistor 3 is GE model

Rref_3 =
R25_3 =


# Main Variables: time, raw1, raw2, raw3, temp1, temp2, temp3

time=raw1=raw2=raw3=temp1=temp2=temp3=[]

#
# tasks
#
# 1. read line from socket
# 2. parse out info into single string, data(time, raw1, raw2, raw3, temp1, temp2, temp3)
# 3. write string to cvs file
# 4. append string elements to individual variables
# 5. plot varibales
# 6. start over


time.append(data(0))
raw1.append(data(1))
raw2.append(data(2))
temp1.append(data(3))
temp2.append(data(4))
temp3.append(data(5))

