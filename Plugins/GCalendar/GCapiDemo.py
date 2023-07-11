from pprint import pprint
from Google import create_service
from json import dumps
import datetime 


CLIENT_SECRET_FILE = 'client_secret.json'
API_NAME = 'calendar'
API_VERSION = 'v3'
SCOPES = ['https://www.googleapis.com/auth/calendar.events']


service = create_service(CLIENT_SECRET_FILE , API_NAME, API_VERSION , SCOPES )

#print(dir(service))

today = datetime.date.today()
now = datetime.datetime.now()
time = now.strftime("%H:%M:%S")

#weekAgo = datetime.date.today() - datetime.timedelta(days=7)
dayAfter = datetime.date.today() + datetime.timedelta(days=2)

page_token = None
while True:
  events = service.events().list(calendarId='primary', pageToken= page_token,  timeMax= str(dayAfter)+'T00:00:00+02:00',timeMin= str(today)+'T'+str(time)+'+02:00', orderBy= "startTime", singleEvents = True).execute()
  page_token = events.get('nextPageToken')
  if not page_token:
    break
json_object = dumps(events["items"], indent=4)
#print(json_object)
print ( json_object , end='')
# Writing to sample.json
#now = datetime.datetime.now()
#t = now.strftime("%H:%M:%S.%fZ")
#print(t)

with open("myEvents.json", "w") as outfile:
        outfile.write(json_object)

#t1 = now.strftime("%H:%M:%S.%fZ")
#print(t1)
