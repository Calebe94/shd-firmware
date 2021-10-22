# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

* Added  `CORE_DEBUG_LEVEL` to `build_flags` on `platformio.ini`;
* Added `SPIFFS` initialization;
* Ported `settings` to `Arduino` framework;
* Added `DEBUG` flag to interact with `sim7070g` module;
* Ported `devices` to `Arduino` framework;
* Ported `rs485` module to `Arduino` framework;
* Ported `protocol` module to `Arduino` framework;
* Ported `message_process` module to `Arduino` framework;
* Added `WiFi` Access Point (`AP`) initialization;
* Added `ESP Async WebServer` library as dependency;
* Added `AsyncTCP` library as dependency;
* Ported `webserver` module to `Arduino` framework;
* Added `reading` module to save and load water reading;
* Added a `Ticker reading_autosave` to save water reading every 5 minutes;
* Added `send_address_by_sms` to send the `address` to every phone number;
* Added `sim7070g_utils` as a helper to parse sms messages;
* Added `sim7070g_read_sms_by_id` function to get sms stored on device by `ID`;
* Added `sim7070g_list_all_sms` function to get all sms stored on device;
* Added `sim7070g_clear_sms_list` function delete all sms stored on device;
* Added `commands` module to parse SMS commands;
* Added `commands_utils` module to help with the commands;

### Changed

* Upgraded `flowsensor` to Arduino framework;
* Changed `settings` debug level to `DEBUG`;
* Changed `sim7070g` debug level to `DEBUG`;
* Changed `char *` arguments to `const char *` on `settings` module to better integrate with `cpp`;

### Fixed

* Fixed `test` target;

### Removed

## [0.17.0]

### Added

* Added `MAX_URI_HANDLERS` config macro to `tiny_webservice` to chose how many URIs will be used;
* Added a simple runtime event handler, to handle events such as `sigterm`, `system restart` and others `#WIP`;
* Added `/restart` route to simulator;
* Added `restart` form to webpage;
* Added `/restart` route to `settings_api`;
* Added `sim7070g_event_handler_task` function to handle `sim7070g` responses;
* Added `sim7070g_suspend_event_handler` and `sim7070g_resume_event_handler` to control the `sim7070g` event handle task;

### Changed

### Fixed

* Fixed `partition` target. Now it calls `webpage` target before;

### Removed

## [0.16.0]

### Added

* Added phones list;
* Added phones list on webpage;

### Changed

### Fixed

* Fixed route typo;

### Removed

## [0.15.0]

### Added

* Added a flask simulator to test webpage and API;

### Changed

### Fixed

### Removed

## [0.14.1]

### Added

### Changed

* Changed interrupt type from falling(`GPIO_INTR_NEGEDGE`) to rising edge(`GPIO_INTR_POSEDGE`);

### Fixed

### Removed

* Removed additional interrupt function registration;

## [0.14.0]

### Added

* Enabled sending controller readings via sms;

### Changed

* Updated developer info formatation on `README.md`

### Fixed

### Removed

## [0.13.2]

### Added

* Added function `sim7070g_read()` to get buffered data from `SIM7070G` module;
* Added function `sim7070g_flush()` to flush buffer from `SIM7070G` module;

### Changed

* Changed create and parse message verification;
* Changed controller and peripheral communication;
* Changed `get_readings_timer_callback()` to check if any data is received by the `peripherals`;
* Moved function to send SMS to `on_message_event_handler`;

### Fixed

### Removed

## [0.13.1]

### Added

* Added `compile`, `flash` and `monitor` Makefile targets;
* Added `partition` Makefile target to upload `data` folder to flash memory;

### Changed

### Fixed

### Removed

## [0.13.0]

### Added

* Added interval from settings to get readings task;
* Added send sms function to get readings process;

### Changed

### Fixed

### Removed

## [0.12.0]

### Added

* Added interval `set` and `get` settings helpers;
* Added local address section on settings page;
* Added settings set and get interval routes;

### Changed

### Fixed

### Removed

## [0.11.0]

### Added

* Added `utils` library with some helpers such as `str_replace()`, `str_split()` and `url_decode()`;
* Added settings `set` and `get` address routes;
* Added address section on settings page;
* Added address `set` and `get` settings helpers;
 
### Changed

### Fixed

### Removed

## [0.10.0]

### Added

* Added rs485 wait transmission
* Added rs485 flush function;
* Added macro condition to initialize rs485 module. Now only the peripheral firmware will have RX queue;
* Added response box classes and ids to `modest.css`;
* Added functions to create`success` and `failure` responses;

### Changed

* Updated wifi ssid name depending on the mode and settings;
* Changed SIM7070G queues sizes;
* Changed get readings function to send request and read the response;
* Moved css from `_header.html` to `modest.css` file;
* Chaged `POST` routes to return the new response page with `success` or `failure` messages;

### Fixed

* Fixed typo in main;
* Fixed index page links;

### Removed

* Removed SIM7070G TX queue;
* Removed `message_process_handler()` from controller firmware;
* Removed `style css` from `_header.html` file;

## [0.9.0]

### Added

* Added phone to settings handler;
* Added get and set phone routes;
* Added phone section to settings page;
* Added dynamic phone placeholder;

### Changed

* Device mode section is now hide when device is controller;

### Fixed

### Removed

## [0.8.0]

### Added

* Added support to **SIM7070G** module;
* Added function to send **SMS**;

### Changed

### Fixed

### Removed

## [0.7.0]

### Added

* Added devices communication algorithm;
* Added `init_routes()` function to `web_api.h` file;

### Fixed

### Changed

* Renamed terms associeted with slavery to its alternatives;
* Changed flow sensor gpio from `4` to `15`;

### Removed

* Removed function `init_routes()` from main file;

## [0.6.0]

### Added

* Added [MinUnit](https://jera.com/techinfo/jtns/jtn002) test to test.c file;
* Added unit tests to `protocol_create_message` and `protocol_message_parse` from protocol lib;
* Added a `test` task pipeline;
* Added environment for the ESP32 WROVER board;

### Fixed

* Fixed a bug from `protocol_message_parse` where a null terminator was not beign added;

### Changed

* Changed `test` target to compile the `TEST_SRCS`;
* Updated README with ESP32 WROVER board infos;

### Removed

## [0.5.0]

### Added

* Added [MinUnit](https://jera.com/techinfo/jtns/jtn002) test to test.c file;
* Added unit tests to `protocol_create_message` and `protocol_message_parse` from protocol lib;
* Added a `test` task pipeline;

### Fixed

* Fixed a bug from `protocol_message_parse` where a null terminator was not beign added;

### Changed

* Changed `test` target to compile the `TEST_SRCS`;

### Removed

## [0.4.0]

### Added

* Added device id file handler(will be used by the main device);
* Added devices uri handlers;

### Fixed

### Changed

* Now when the device is `controler`, the form ID is hidden;
* When the device is `controller`, the form to register peripherals `ids` is shown;

### Removed

* Removed unecessary references to files and comments;

## [0.3.1]

### Added

### Fixed

### Changed

* Renamed peer to mode in the entire project;
* Updated README with latest changes;

### Removed

## [0.3.0]

### Added 

* Added protocol to parse RS485 messages with pretty simple Master-Slave support;
* Added rs485 event handler;
* Added wifi AP module;
* Added flash partition support;
* Added webserver to enter as device settings;
* Added settings data persistence;
* Added javascript file to manipulate html elements;
* Added webpage folder with the web static content;

### Fixed

### Changed

* Changed settings variables and functions names;

### Removed

* Removed html, js and css files from data folder. They'll be generated by **ssg5** script from `webpage` folder.

## [0.2.0]

### Added 

* Added max485 initialization;
* Added rs485 module interface;
* Added rs485 echo example to project;
* Added serial console module interface;

### Fixed

### Changed

* Updated README with usefull info and pipeline badge;

### Removed

## [0.1.0]

### Added 

* Added function calls to setup flowsensor
* Added install isr service call
* Added flowsensor module
* Added flow sensor math functions
* Added gitlab-ci file
* Added can receiver program to main
* Added mcp can lib as submodule
* Added platformio project
* Added artifacts

### Fixed

### Changed

* Updated flowsensor function calls
* Updated flow variables and equations
* Updated functions name
* Updated README with function info
* Updated firmware path in pipeline
* Updated README and removed readme.rst
* Changed project microcontroller to esp32
* Update .gitlab-ci.yml file
* Updated docker image
* Changed pins
* Updated README usefull info

### Removed

* Removed old .cpp main file

[unreleased]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.17.0...main
[0.17.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.16.0...0.17.0
[0.16.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.15.0...0.16.0
[0.15.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.14.1...0.15.0
[0.14.1]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.14.0...0.14.1
[0.14.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.13.2...0.14.0
[0.13.2]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.13.1...0.13.2
[0.13.1]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.13.0...0.13.1
[0.13.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.12.0...0.13.0
[0.12.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.11.0...0.12.0
[0.11.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.10.0...0.11.0
[0.10.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.9.0...0.10.0
[0.9.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.8.0...0.9.0
[0.8.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.7.0...0.8.0
[0.7.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.6.0...0.7.0
[0.6.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.5.0...0.6.0
[0.5.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.4.0...0.5.0
[0.4.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.3.1...0.4.0
[0.3.1]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.3.0...0.3.1
[0.3.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.2.0...0.3.0
[0.2.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/compare/0.1.0...0.2.0
[0.1.0]: https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/-/tags/0.1.0

