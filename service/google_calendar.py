#!/usr/bin/env python3
import calendar
import datetime
import json

from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials

from PyQt5.QtCore import QSettings
from PyQt5.QtCore import QCoreApplication

import common

# If modifying these scopes, delete the file google_token.json.
SCOPES = ['https://www.googleapis.com/auth/calendar.readonly']

def read_isodate(event, fieldname):
    s = event[fieldname].get('dateTime', event[fieldname].get('date'))
    return datetime.datetime.fromisoformat(s)

def get_end_of_current_meeting():
    creds = None
    # The file google_token.json stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    settings = QSettings()
    google_token = settings.value('google_token')
    if google_token:
        creds = Credentials.from_authorized_user_info(json.loads(google_token), SCOPES)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'google_credentials.json', SCOPES)
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        settings = QSettings()
        settings.setValue('google_token', creds.to_json())
        del settings

    service = build('calendar', 'v3', credentials=creds)

    # Call the Calendar API
    now = datetime.datetime.now(datetime.timezone.utc)
    now_iso = now.isoformat()
    events_result = service.events().list(calendarId='primary', timeMin=now_iso,
                                        maxResults=1, singleEvents=True,
                                        orderBy='startTime').execute()
    events = events_result.get('items', [])

    if not events:
        return None
    for event in events:
        start = read_isodate(event, 'start')
        end = read_isodate(event, 'end')
        if start < now and now < end:
            return end


def get_end_of_current_meeting_utctimestamp():
    t = get_end_of_current_meeting()
    return calendar.timegm(t.utctimetuple()) if t is not None else None


if __name__ == '__main__':
    QCoreApplication.setOrganizationName(common.ORGANIZATION_NAME)
    QCoreApplication.setApplicationName(common.APPLICATION_NAME)
    print(get_end_of_current_meeting_utctimestamp())
