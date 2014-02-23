## installing ebay-python. I tried a couple of differnt python ebay packages. this one worked
## I had to do a manual install of something like libxml that wouldn't install by default. I
## had to track-down the windows installer for that library. The output of the program is in
## a format called JSON.






from ebay.finding import getSearchKeywordsRecommendation, findItemsByKeywords, findItemsByCategory, findItemsAdvanced, findItemsByProduct, findItemsIneBayStores, getHistograms
from ebay.shopping import GetSingleItem
import json

items = []

wow = findItemsByKeywords(keywords="gopro hero bacpac")
wow = json.loads(wow)
#items = int(wow['findItemsByKeywordsResponse'][0]['searchResult'][0]['@count'])
for i in wow['findItemsByKeywordsResponse'][0]['searchResult'][0]['item']:
    print i['title'][0] + '     ' + \
    i['sellingStatus'][0]['currentPrice'][0]['__value__'] + '   ' + i['itemId'][0]
    items.append(i['itemId'][0])
for i in items:
    wow = GetSingleItem(i, include_selector='Description')
    print wow
