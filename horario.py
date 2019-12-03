from datetime import datetime, timedelta

from urllib.request import urlopen

date = urlopen('http://just-the-time.appspot.com/').read().strip()
date = str(date)[2:-1]

date = datetime.strptime(date, "%Y-%m-%d %H:%M:%S")

date = date - timedelta(hours=3)

date = datetime.strftime(date,"%a %b %d %H:%M:%S") 

date = date + ' UTC-03 2019' 

import os
cmd = 'date -s \"' + date + '\"'
os.system(cmd)
