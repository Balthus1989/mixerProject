from datetime import date 
import time
import requests
from astroquery.mast import Observations
from astropy.coordinates import EarthLocation, AltAz, SkyCoord
import astropy.units as u

import pandas as pd 


class Constellation_retrieval():
    
    def __init__(self):
        pass
        
    
    def main_method(self):
        
        def getSkyCoord(ra, dec):
            skyCoord = SkyCoord(ra=ra * u.deg, dec=dec * u.deg, representation_type='spherical')
            return skyCoord

        def getAltAz(skyCoord, observerTime, location):
            altAz = skyCoord.transform_to(AltAz(obstime=observerTime, location=location))
            return altAz

        def getAltAngle(altAz):
            alt = (altAz.alt * u.deg).value
            return alt

        def getAzAngle(altAz):
            az = (altAz.az * u.deg).value
            return az

        def getConstellation(name_flamsteed_full):
            name_split = str(name_flamsteed_full).split('-')
            constellation = name_split[1]
            return constellation
    
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
        
        bright_stars['SkyCoord'] = bright_stars.apply(lambda row : getSkyCoord(row['ra'], row['dec']), axis=1)
        bright_stars['AltAz'] = bright_stars.apply(lambda row: getAltAz(row['SkyCoord'], observerTime, location), axis=1)
        bright_stars['Altitude'] = bright_stars.apply(lambda row: getAltAngle(row['AltAz']), axis=1)
        bright_stars['Azimuth'] = bright_stars.apply(lambda row: getAzAngle(row['AltAz']), axis=1)

        bright_stars = bright_stars.drop(['SkyCoord', 'AltAz'], axis=1)
        bright_stars['Constellation'] = bright_stars.apply(lambda row: getConstellation(row['name_flamsteed_full']), axis=1)
        
        bright_stars = bright_stars[bright_stars['Constellation'] != '']
        bright_stars = bright_stars.reset_index(drop=True)
        
        visible_stars = bright_stars[bright_stars['Altitude'] >= 0]
        not_visible_stars = bright_stars[bright_stars['Altitude'] < 0]
        
        list_of_not_visible_constellations = list(set(not_visible_stars['Constellation'].values))
        visible_stars = visible_stars[~visible_stars['Constellation'].isin(list_of_not_visible_constellations)]
        
        final_visible_stars = list(set(visible_stars['Constellation'].values))
        return "hello world"