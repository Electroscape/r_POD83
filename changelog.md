# Changelog

## v 0.28

  * filtered the frontend updates out from the server interface

## v 0.27

 * gas puzzle sequentially dependent 
 * fixed selfcheck triggering rachels files being available

## v 0.26

  * added double auth logout option to override
  * added dropdown/autocomplete to chat interface for GM
  * added rachels personal folders to T1 when rachel is logged in after The truth has been played 
  * more localizations
  * laserlock rfid presentation requirement fix (prviously only covered one reader)

## v 0.25

  * adjusted Dispenser colouring to match sample colours
  * Adjusted gas control layout on T1, now the table is integrated
  * fixed Gas riddle on T2 receiving "sample update" when T1 enters wrong code
  * Increased keypad blocking time to discourage brute forcing
  * added manual ping option in case customers don't notice
  * added german upload page
  * update to rachels personal page on T2
  * locker page images are now handled by ver-config.json

## v 0.24

 * change server kiosk layout to prevent missclicks due to updates
 * fixed selfcheck ending with active camera stream
 * fixed chat page failing to load due to json format receiving escpae characters
 * more localisations 
 * added rachels personal folders on Terminal 2

## v 0.23 (Terminals UI)

* fix progressbar not working when set manually
* fix audio not playing in kiosk mode
* disable laserlock notification when successful lab access
* change Media format from `.mp4` to `.webm`
* fix upload page flashing images when loading first time
* minor bugs and css fixes
* automated startup and continuity of security stream upon first double handshake
* added Biovita advertisement as button on the kiosk interface

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
