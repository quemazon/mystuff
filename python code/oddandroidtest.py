import androidhelper
droid = androidhelper.Android()
#droid.makeToast('my text to print should be inside the quotes')
droid.vibrate(1000)
#droid.viewMap('681 43rs st., los alamos, nm')
#droid.ttsSpeak('this is cool')
#droid.startLocating()
#data = getDeviceID()
droid.makeToast('coolio')
#droid.generateDtmfTones('5053102085')
#droid.ttsSpeak('5053102085')
data = droid.getNetworkType()
droid.makeToast(data)