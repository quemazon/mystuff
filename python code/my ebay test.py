## installing ebay-python. I tried a couple of differnt python ebay packages. this one worked
## I had to do a manual install of something like libxml that wouldn't install by default. I
## had to track-down the windows installer for that library. The output of the program is in
## a format called JSON.






from ebay.finding import getSearchKeywordsRecommendation, findItemsByKeywords, findItemsByCategory, findItemsAdvanced, findItemsByProduct, findItemsIneBayStores, getHistograms
import json

wow = findItemsByKeywords(keywords="sportwerks recoil")
wow = json.loads(wow)
items = int(wow['findItemsByKeywordsResponse'][0]['searchResult'][0]['@count'])
for i in range(items):
    print wow['findItemsByKeywordsResponse'][0]['searchResult'][0]['item'][i]['title'][0] + '     ' + \
    wow['findItemsByKeywordsResponse'][0]['searchResult'][0]['item'][i]['sellingStatus'][0]['currentPrice'][0]['__value__']

