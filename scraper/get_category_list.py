from selenium import webdriver
from selenium.webdriver.common.keys import Keys

op = webdriver.ChromeOptions()
op.add_argument('headless')

driver = webdriver.Chrome("/home/kunal/chromedriver", options=op)

all_pages = {}

fdmaker = open("./pages.txt", "r")
already_there = fdmaker.read()
to_add = already_there.split("\n")
for i in to_add:
    all_pages[i] = 1
fdmaker.close()

total_pages = 0

fd = open("./pages.txt", "a")

while total_pages <= 5000:
    driver.get("https://en.wikipedia.org/wiki/Special:Random/Category")
    category_name = driver.find_element_by_id("firstHeading")
    subcats = driver.find_elements_by_class_name("mw-category-group")
    cnt = 0
    here = []
    for subcat in subcats:
        pages = subcat.text.split("\n")
        for i in range(1, len(pages)):
            if pages[i][0].isalpha() == False and pages[i][0].isnumeric() == False:
                continue
            if pages[i][0][0:5] == "Talk:" or pages[i][0][0:14] == "Category talk:" or pages[i][0][0:8] == "Template" or pages[i][0][0:5] == "User":
                continue
            if pages[i] in all_pages:
                continue
            here.append(pages[i])
            cnt += 1
    if cnt >= 5:
        for page in here:
            all_pages[page] = 1
            fd.write(page + "\n")
        total_pages += cnt
        print(f"\033[92mAccepted: " + category_name.text + " " + str(cnt))
    else:
        print(f"\033[93mRejected cat: ", end="")
        print(category_name.text + " " + str(cnt))
print(total_pages)
fd.close()
