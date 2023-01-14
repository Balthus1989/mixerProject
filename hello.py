from datetime import date 
import time
import requests
from astroquery.mast import Observations
from astropy.coordinates import EarthLocation, AltAz, SkyCoord
import astropy.units as u
import pandas as pd 

class Hello:
    def __init__(self):
        pass
    def print_hello(self):
        # Getting the current date
        today = date.today()
        theTime = time.strftime("%H:%M:%S")
        observerTime = str(today) + " " + str(theTime)
        
        # Getting the current location. It is necessary to be connected to
        # the internet
        location = requests.get('http://ipinfo.io/json').json()
        location = EarthLocation.of_address(location['city'] + ", " + location['region'])
        
        bright_stars = pd.read_excel('bright_stars.xlsx', index_col=False)
        bright_stars.drop(['Unnamed: 0'], axis=1, inplace=True) 
        
        return "hello world"