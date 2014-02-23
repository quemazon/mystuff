# type/draw a text string to the default printer
# needs the win32 extensions package  pywin32-204.win32-py2.4.exe
# from: http://starship.python.net/crew/mhammond/win32/Downloads.html
# make sure the printer is turned on and ready ...
# tested with Python24    vegaseat    14oct2005

import win32ui
import win32print
import win32con

str1 = \
"""W.H.O. influenza chief Albrecht Stohr
is not optimistic that generic producers
would be able tomake Tamiflu, the first

"""

#str1 = str1 + str1 + str1 + str1  # test height of text area

hDC = win32ui.CreateDC()
print win32print.GetDefaultPrinter()  # test
hDC.CreatePrinterDC(win32print.GetDefaultPrinter())
hDC.StartDoc("Test doc")
hDC.StartPage()
hDC.SetMapMode(win32con.MM_TWIPS)

# draws text within a box (assume about 1400 dots per inch for typical HP printer)
ulc_x = 0000    # give a left margin
ulc_y = 0000   # give a top margin
lrc_x = 8000   # width of text area-margin, close to right edge of page
lrc_y = -8000  # height of text area-margin, close to bottom of the page
hDC.DrawText(str1, (ulc_x, ulc_y, lrc_x, lrc_y), win32con.DT_LEFT)

# draws text within a box (assume about 1400 dots per inch for typical HP printer)
ulc_x = 4000    # give a left margin
ulc_y = -3000   # give a top margin
lrc_x = 9000   # width of text area-margin, close to right edge of page
lrc_y = -5000  # height of text area-margin, close to bottom of the page
hDC.DrawText(str1, (ulc_x, ulc_y, lrc_x, lrc_y), win32con.DT_LEFT)

hDC.EndPage()
hDC.EndDoc()