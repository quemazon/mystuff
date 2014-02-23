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


import win32ui
import win32print
import win32con

hDC = win32ui.CreateDC()
print win32print.GetDefaultPrinter()  # test
hDC.CreatePrinterDC(win32print.GetDefaultPrinter())
hDC.StartDoc("Test doc")

def PrintEnvelop(toaddress):
    hDC.StartPage()
    hDC.SetMapMode(win32con.MM_TWIPS)


    str1 = "Burnside Family" + "\r\n" + "681 43rd st" + "\r\n" + "Los Alamos, NM 87544"
    str2 = names[0] + "\r\n" + street[0] + "\r\n" + city[0]

    # draws text within a box (assume about 1400 dots per inch for typical HP printer)
    ulc_x = 0000    # give a left margin
    ulc_y = 0000   # give a top margin
    lrc_x = 2800   # width of text area-margin, close to right edge of page
    lrc_y = -1400  # height of text area-margin, close to bottom of the page
    hDC.DrawText(str1, (ulc_x, ulc_y, lrc_x, lrc_y), win32con.DT_LEFT)

    # draws text within a box (assume about 1400 dots per inch for typical HP printer)
    ulc_x = 4000    # give a left margin
    ulc_y = -3000   # give a top margin
    lrc_x = 8000   # width of text area-margin, close to right edge of page
    lrc_y = -5000  # height of text area-margin, close to bottom of the page
    hDC.DrawText(toaddress, (ulc_x, ulc_y, lrc_x, lrc_y), win32con.DT_LEFT)

    hDC.EndPage()

for i in range(numaddr):
    toaddress = names[i] + "\r\n" + street[i] + "\r\n" + city[i]
    #toaddress = unicode(toaddress)
    PrintEnvelop(toaddress)

hDC.EndDoc()
