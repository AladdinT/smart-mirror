import requests ,json
#import datetime as dt

if __name__ == "__main__":
	api_key = "57cb9875a41e9615380e16c8e384e34d"
	base_url = "http://api.openweathermap.org/data/2.5/weather?"

	#TODO : Get Geolocation automatically
	city = "6th of october, Egypt"

	complete_url = base_url+"appid="+api_key+"&q="+city
	response = requests.get(complete_url).json()

	json_object = json.dumps(response, indent=4)
 
	# Writing to sample.json
	with open("weatherData.json", "w") as outfile:
		outfile.write(json_object)
