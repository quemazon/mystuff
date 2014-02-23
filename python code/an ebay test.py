from ebay.shopping import FindProducts, FindHalfProducts , GetSingleItem, GetItemStatus, GetShippingCosts, GetMultipleItems, GetUserProfile, FindPopularSearches, FindPopularItems, FindReviewsandGuides, GetCategoryInfo, GeteBayTime
import json

wow = FindProducts(encoding='JSON', query='ipod', available_items='false', max_entries='10')
wow = json.loads(wow)
for i in wow['Product']:
    print i['Title'] + '     ' + \
    i['ProductID'][0]['Value']

