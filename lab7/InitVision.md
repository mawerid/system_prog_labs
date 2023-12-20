# Usb Control: Vision

This behind-the-scene page describes ideas and architecture of Usb control app. 
But it doesn't represent final version of it.

# About

It's an app to check and control all USB devices.

Here are the features:
- get info about connect/disconnect USB events
- save all data in file and show it with GUI
- config files as JSON
- enable/disable devices to connect

# Data

Object data: (create linked list)
- serial number
- is connected flag
- enable/disable flag

Config data:
- file path
- blocked devices

# GUI

Main frame:
- list with devices (ID, serial, connected, banned)

Update button to check is there any new devices.
Progress bar while checking.

# Architecture

There must be MVVM architecture.
So here it is!

### Main:
Run viewmodel main process.
Check for proper exit.

#### Functions:
- main

### View:
Generate a main frame.
Handle buttons (to block and to update all)
Generating progress bar while waiting of model response.
Proper exit.

#### Functions:
- main view
- reload button
- ban buttons
- update view
- parse response from model

### Model:
Run as another process.
Start with open file with object data last saved.
Load all previous data.
Update this data with current status.

#### Functions:
- main model (get signals from view and do some things)
- send info view
- get info view
- load from file
- list functions
- save to file
- update statuses
- ban device
- unban device
- load config
- update config

##### Get response from view:

1. Update blocked/unblocked devices.
2. Update all current situation.

Must be:
- Get response from view
- Do some things
- Send response to view with:
    - Return code
    - Other info (everything that changed)

