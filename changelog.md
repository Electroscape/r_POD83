# Changelog

## v0.37

 * localized js libraries 
 * workaround for cleanroom cylinder arming

## v0.36

 * Chat readibility improvements
 * arbiter signal stability improvement
 * changed sweetalert.js source 

## v0.35
 * updated Hamburg dispenser with additional features
 * fixed a dictionary KeyError within wiki page
 * added lockerwall service opening code
 * cleanroom rebuild to Hat stb v0.0.5 Mother
 * added flutter effect to lab 

## v0.34
 * added option to split hint configuration by location

## v0.33
 * added separate sound for anything but message
 * option to override gasriddle 
 * separable start events for the clock on the server interface
 * rework to the analyzer
 * added chromium reset shellscript to be tied in from GM PC
 * removed fullscreen in video player of terminal 1

## v0.32
 * added a testimage to the servicemode to spot if TVs are actually turned on
 * improved the black-screen image for the TVs 


## v0.31
  * renaming and hints for new sample status 
  * rachels upload now should lock the field of upload as completed

## v 0.30

  * replaced sample status page
  * changed notification sound
  * fix to microscope possibly not picking up a very fast change
  * added sound staggering for self checkup so the system can be assessed easier


## v 0.29

  * added hint to the microscope killswitch video
  * fixed permissions being lost on T1 being linked to T3 boot
  * Made laserlock action being arbitrated by the Ter1 and its cable status along with sepereate triggers on the server interface
  * added 15 minute left announcement

## v 0.28

  * **first lauch of rachel tablet (ter3)**
  * filtered the frontend updates out from the server interface
  * refactor airlock to laserlock in TR1 (except cmds used by arb pi)
  * fix upload levels problematic coding and add colors to symbols
  * fix blue usb override return back to homepage
  * green-out lab access on double-auth override (needs to refresh the page)
  * disable auto-complete for server hints
  * mandatory hints are now highlighted
  * changed GM input line to a bigger fields so the whole message is readable
  * added instruction how to block zoom
  * fixed analyzers bug with the killswitch sample position being accepted in wrong position
  * the uploads media override now doesn't require pressing removed first
  * default language is now german
  * fixed "buzz" bell on server_home sending out message field
  * Upload: manual media switch now requires no "no media" before updating. Progress still gets lost
### ST specifc changes v0.28
  * updated lighting in rachel ending
  * added stress atmo after cleanroom opening request

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
