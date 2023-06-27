# Changelog

## v 0.23 (Terminals UI)

* fix progressbar not working when set manually
* fix audio not playing in kiosk mode
* disable laserlock notification when successful lab access
* change Media format from `.mp4` to `.webm`
* fix upload page flashing images when loading first time
* minor bugs and css fixes

## v 0.22.1 (Terminals UI)

* add `static/json/ver_config` for versions differences
* add sounds for bootup, airlock boot, active laser and notifications
* add colors for Elancell HQ and Rachel in chat
* change Wiki language handling
* add personell pics
* fix usb upload bug

## v 0.22

* servicemode triggered from lockerwall
* added config flie listing
* fixed minor startup problems in T2
* queued security stream after "the truth"
* added eventlogging to the Arbiter
* fixed laserlock problems with card locaality and magnet

## v 0.21

* Arbiter handles events in a non blocking way
* Dispenser revision and fixes, also update of m_servo brains
* small fix to lablight not containing a dooropen on service
* Terminal
  * German error code translation
  * added security system stream as button
  * added selfcheck button
* Pi_TVs: improved reconnect handling

## V 0.2

* STB parts updated: Laserlock, Lablight
* automatic endings
* added safety to usb_boot now can be removed post startup without triggering
* Laserlock -> binary instead of pinbased, permanently active eventscanning along with changes to arbiter evenmapping
* added light options to Laserlock and lablight -> Service mode added endings are still WIP
* added inverting of events along with laserlock cable update in both ways
* removed workaround of labDoorOpenEvent and made it functional
* changes to terminal python-socketio across the system, reconnection loops and ping timings, flask startups -> SIO start
* adjustments for HH: cleanroom duration and outer door for laserlock
* new setup desription for microscope, removed socketio connection to server
* added logging throughout python system

## V 0.1 initial version
