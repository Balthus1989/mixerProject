from datetime import date 
import time
import requests
import json
from astropy import coordinates
from astroquery.mast import Observations
from astropy.coordinates import EarthLocation, AltAz, SkyCoord
import astropy.units as u
from astropy.io import fits
import pandas as pd
import visible_stars_data
import numpy as np
from sklearn.preprocessing import MinMaxScaler

def get_stars():

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
    db = visible_stars_data.data
    bright_stars = pd.DataFrame.from_dict(db, orient='index')

    # Getting the current date
    today = date.today()
    theTime = time.strftime("%H:%M:%S")
    observerTime = str(today) + " " + str(theTime)
    
    # Getting the current location. It is necessary to be connected to
    # the internet
    location = requests.get('http://ipinfo.io/json').json()
    location = EarthLocation.of_address(location['city'] + ", " + location['region'])
    
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
    
    final_visible_stars = str(final_visible_stars)
    final_visible_stars = final_visible_stars.replace("[", "")
    final_visible_stars = final_visible_stars.replace("]", "")
    inal_visible_stars = final_visible_stars.replace("'", "")
    final_visible_stars = final_visible_stars.replace("'", "")
    
    print(final_visible_stars)
    return final_visible_stars

def get_sonification(constellation_name="no_constellation"):
    if constellation_name == "no_constellation":
        constellation_name = "UMa"
    
    db = visible_stars_data.data
    bright_stars = pd.DataFrame.from_dict(db, orient='index')
    
    constellation_target = bright_stars[bright_stars["name_flamsteed_full"].str.contains(constellation_name, na=False)]
    a_star = constellation_target.sample()
    skyCoord = coordinates.SkyCoord(ra=float(a_star['ra']) * u.deg, dec=float(a_star['dec']) * u.deg, representation_type='spherical')
    
    radius = 0.5 * u.arcminute
    observed_object = Observations.query_object(skyCoord.to_string(), radius=radius)
    
    timeseries_obs_object = observed_object[observed_object['dataproduct_type']==['timeseries']]
    optical_timeseries = timeseries_obs_object[timeseries_obs_object['wavelength_region'] == 'Optical']
    filename = optical_timeseries[0]['dataURL'].split('/')[2]
    
    first_URL_part = 'https://mast.stsci.edu/api/v0.1/Download/file?uri=mast:TESS/product/'
    URL_complete = first_URL_part + filename
    
    #data = fits.getdata(URL_complete)
    myFile = requests.get(URL_complete)
    
    #with fits.open(URL_complete) as data:
    #    print('hello')
    
    
    with open('data.fits', 'wb') as f:
        f.write(myFile.content)
    
    data = fits.getdata('.\data.fits')
    
    flux_data = data['SAP_FLUX'][~np.isnan(data['SAP_FLUX'])]
    
    myScaler = MinMaxScaler((48, 120))
    myScaler.fit(np.array(flux_data).reshape(-1, 1))
    
    results = myScaler.transform(np.array(flux_data).reshape(-1, 1))
    results = results.squeeze()
    results = [int(i) for i in results]
    #fits_data = fits.open('data.fits')
    
    #flux_data = fits_data[1].data['SAP_FLUX'][~np.isnan(fits_data[1].data['SAP_FLUX'])]
    #print(type(results))
    
    results = list(results)
    results = str(results)
    # prova = ['Ada', 'Ipa', 'Apa']
    # prova = list(prova)
    # prova = str(prova)
    results = results.replace("[", "")
    results = results.replace("]", "")
    results = results.replace("'", "")
    results = results.replace("'", "")
    return results
