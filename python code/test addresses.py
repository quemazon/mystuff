import easygui as eg

filename = eg.fileopenbox(msg=None, title=None, default='*', filetypes=None)

names = []
street = []
city = []

myfile = open(filename, 'r')

addresses = myfile.read()
addresses = addresses.split("\n")
numaddr = len(addresses)/4
for i in range(numaddr):
    names.append(addresses[i*4].strip("\r\n"))
    street.append(addresses[i*4+1].strip("\r\n"))
    city.append(addresses[i*4+2].strip("\r\n"))

myfile.close()


