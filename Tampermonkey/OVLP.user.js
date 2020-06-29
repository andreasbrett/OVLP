// ==UserScript==
// @name         OVLP
// @version      0.45
// @description  handsfree looping in Youtube and Vimeo
// @author       Andreas Brett
// @match        *://*.youtube.com/*
// @match        *://*.vimeo.com/*
// @exclude      *://*.youtube.com/subscribe_embed?*
// @run-at       document-idle
// ==/UserScript==



/* ========================================================================================
   CUSTOMIZE KEY MAPPINGS
    - key: any character (a-z) or digit (0-9)
    - possible modifiers: "ctrl", "shift", "alt"
    ----> combine modifiers by concatenating them (e.g. "shift,alt")
    ----> note: combining ctrl and shift won't work unfortunately
======================================================================================== */

var button_Speed = {
    "key": "e",
    "modifiers": "shift,alt"
}
var button_Loop = {
    "key" : "w",
    "modifiers": "shift,alt"
}
var button_Back = {
    "key": "a",
    "modifiers": "shift,alt"
}
var button_PlayPause = {
    "key": "s",
    "modifiers": "shift,alt"
}
var button_Forward = {
    "key": "d",
    "modifiers": "shift,alt"
}
var button_Reset = {
    "key": "q",
    "modifiers": "shift,alt"
}
var button_Rewind = {
    "key": "r",
    "modifiers": "shift,alt"
}
var button_Fullscreen = {
    "key": "f",
    "modifiers": "shift,alt"
}





/* ========================================================================================
   DO NOT EDIT BELOW
======================================================================================== */

var ovlp_player = null;
var ovlp_log = null;
var ovlp_progress = null;
var ovlp_timer = null;
var ovlp_loopStart = null;
var ovlp_loopEnd = null;
var ovlp_speed = 1.0;


function log(s) {
    // log to developer console
    console.log("%cOnline Video Looper Pedal: %c" + s, "font-weight: bold; color: #990099", "color: gray");

    // create log pane at first call
    if (ovlp_log == null) createLogPane();

    // show text to user
    ovlp_log.innerHTML = s;
    ovlp_log.style.display = "block";

    // hide after 850msec
    setTimeout(function() {
        ovlp_log.innerHTML = "";
        ovlp_log.style.display = "none";
    }, 850);
}

function modifyButtonVar(b) {
    // only use first character
    b.key = b.key[0];

    // check if letter or digit
    if (/^[a-zA-Z]$/.test(b.key)) {
        b.code = "Key" + b.key.toUpperCase();
    } else if (/^[0-9]$/.test(b.key)) {
        b.code = "Digit" + b.key();
    } else {
        b.code = null;
    }

    // split modifiers into array
    b.modifiers = b.modifiers.split(",");
    for (var i = 0; i < b.modifiers.length; i++) {
        b.modifiers[i] = b.modifiers[i].toLowerCase() + "Key";
    }
}

function checkInput(k, b) {
    var bReturn = false;

    // check if key has been depressed
    if (k.code == b.code) {
        bReturn = true;

        // check if modifiers have also been depressed
        for (var i = 0; i < b.modifiers.length; i++) {
            bReturn = bReturn & k[b.modifiers[i]];
        }
    }

    // fetch player element if not already loaded
    if (bReturn && ovlp_player == null) {
        ovlp_player = document.getElementsByTagName("video")[0];
    }

    return bReturn;
}

function handleKeyInput(k) {
    // button "Speed"
    if (checkInput(k, button_Speed)) {
        k.preventDefault();
        togglePlaybackRate();
    }
    // button "Loop"
    else if (checkInput(k, button_Loop)) {
        k.preventDefault();
        setLoopMarkers();
    }
    // button "Back"
    else if (checkInput(k, button_Back)) {
        k.preventDefault();
        seekBack();
    }
    // button "Play/Pause"
    else if (checkInput(k, button_PlayPause)) {
        k.preventDefault();
        togglePlayPause();
    }
    // button "Forward"
    else if (checkInput(k, button_Forward)) {
        k.preventDefault();
        seekForward();
    }
    // button "Reset"
    else if (checkInput(k, button_Reset)) {
        k.preventDefault();
        clearSettings();
    }

    // button "Rewind"
    else if (checkInput(k, button_Rewind)) {
        k.preventDefault();
        seekRewind();
    }

    // button "Fullscreen"
    else if (checkInput(k, button_Fullscreen)) {
        k.preventDefault();
        toggleFullscreen();
    }
}

function seekRewind() {
    // seek back to beginning (of loop when in loop mode; of video otherwise)
    if (ovlp_loopStart) {
        log("Rewinding Loop Start");
        ovlp_player.currentTime = ovlp_loopStart;
    } else {
        log("Rewinding");
        ovlp_player.currentTime = 0;
    }
}

function seekBack() {
    // seek back 5sec (respecting loop)
    if (ovlp_loopStart && ovlp_player.currentTime - 5 < ovlp_loopStart) {
        ovlp_player.currentTime = ovlp_loopStart;
    } else{
        ovlp_player.currentTime -= 5;
    }
    log("Rewinding 5sec");
}

function seekForward() {
    // seek forward 5sec (respecting loop)
    if (ovlp_loopEnd && ovlp_player.currentTime + 5 > ovlp_loopEnd) {
        ovlp_player.currentTime = ovlp_loopEnd;
    } else{
        ovlp_player.currentTime += 5;
    }
    log("Forwarding 5sec");
}

function togglePlaybackRate() {
    // go through playback rate chain
    if (ovlp_speed == 1) {
        ovlp_speed = 0.75;
    } else if (ovlp_speed == 0.75) {
        ovlp_speed = 0.5;
    } else if (ovlp_speed == 0.5) {
        ovlp_speed = 0.35;
    } else if (ovlp_speed == 0.5) {
        ovlp_speed = 0.35;
    } else if (ovlp_speed == 0.35) {
        ovlp_speed = 0.25
    } else {
        ovlp_speed = 1.0;
    }

    // set playback rate
    ovlp_player.playbackRate = ovlp_speed;
    log("Playback rate = " + (ovlp_speed*100) + "%");
}

function toggleFullscreen() {
    // toggle full screen or normal screen
    ovlp_player.webkitDisplayingFullscreen ? ovlp_player.webkitExitFullScreen() : ovlp_player.webkitEnterFullScreen();
}

function togglePlayPause() {
    // toggle play state
    if (ovlp_player.paused) {
        log("Play");
        ovlp_player.play();
    } else {
        log("Pause");
        ovlp_player.pause();
    }
}

function setLoopMarkers() {
    if (ovlp_loopStart == null) {
        // set loop start marker
        ovlp_loopStart = ovlp_player.currentTime;
        log("Set loop start");
    } else if (ovlp_loopEnd == null ) {
        // set loop end marker
        ovlp_loopEnd = ovlp_player.currentTime;
        log("Set loop end");

        // create progress bar at first call
        if (ovlp_progress == null) createProgressBar();

        // start loop checking
        ovlp_progress.style.display = "block";
        ovlp_timer = setInterval(checkPlayState, 50);
    } else {
        // clear loop markers
        ovlp_loopStart = null;
        ovlp_loopEnd = null;
        log("Loop cleared");

        // stop loop checking
        ovlp_progress.style.display = "none";
        clearInterval(ovlp_timer);
    }
}

function clearSettings() {
    log("Settings cleared");

    // clear loop markers
    ovlp_progress.style.display = "none";
    ovlp_loopStart = null;
    ovlp_loopEnd = null;

    // reset playback rate to 100%
    ovlp_speed = 1.0;
    ovlp_player.playbackRate = 1.0;
}

function checkPlayState() {
    if (ovlp_loopStart != null && ovlp_loopEnd != null) {
        // rewind to loop start
        if (ovlp_player.currentTime > ovlp_loopEnd) {
            ovlp_player.currentTime = ovlp_loopStart;
        }

        // update progress bar
        var progress = 100*(ovlp_player.currentTime - ovlp_loopStart)/(ovlp_loopEnd - ovlp_loopStart);
        ovlp_progress.innerHTML = Math.floor(progress) + "%";
        ovlp_progress.style.backgroundSize = progress + "%";
    }
}

function createLogPane() {
    // calculate position
    var rect = ovlp_player.getBoundingClientRect();
    var width = 350;
    var height = 35;
    var left = rect.left + (rect.width - width)/2;
    var top = rect.bottom - 2*height;

    // create div element
    var logDiv = document.createElement("div");
    logDiv.id = "ovlp_log";
    logDiv.innerHTML = "";
    logDiv.style.cssText = "position: absolute; top: " + top + "px; left: " + left + "px; width: " + width + "px; height: " + height + "px; border-radius: 5px; margin: 0; padding: 0; background-color: #fff; font-weight: bold; font-size: " + (height*0.5) + "px; color: #333; text-align: center; line-height: " + height + "px; opacity: 0.8; display: none;";

    // attach element to body
    document.getElementsByTagName("body")[0].appendChild(logDiv);

    // store reference to log pane
    ovlp_log = document.getElementById("ovlp_log");
}

function createProgressBar() {
    // calculate position
    var rect = ovlp_player.getBoundingClientRect();
    var width = 100;
    var height = 75;
    var left = rect.left + 5;
    var top = rect.top + 5;

    // create div element
    var progressDiv = document.createElement("div");
    progressDiv.id = "ovlp_progress";
    progressDiv.innerHTML = "";
    progressDiv.style.cssText = "position: absolute; top: " + top + "px; left: " + left + "px; width: " + width + "px; height: " + height + "px; border-radius: 5px; margin: 0; padding: 0; background-color: #fff; background-repeat: no-repeat; background-image: linear-gradient(90deg, rgb(51, 153, 102, 50) 0%, rgb(51, 153, 102, 50) 100%); background-size: 0%; font-weight: bold; font-size: " + (height*0.5) + "px; color: #333; text-align: center; line-height: " + height + "px; text-shadow: 0 0 8px #fff; opacity: 0.8; display: none;";

    // attach element to body
    document.getElementsByTagName("body")[0].appendChild(progressDiv);

    // store reference to log pane
    ovlp_progress = document.getElementById("ovlp_progress");
}

(function() {
    'use strict';

    // define key handlers
    document.addEventListener("keyup", handleKeyInput);

    // modify button vars
    modifyButtonVar(button_Loop);
    modifyButtonVar(button_Speed);
    modifyButtonVar(button_Back);
    modifyButtonVar(button_Forward);
    modifyButtonVar(button_PlayPause);
    modifyButtonVar(button_Reset);
    modifyButtonVar(button_Rewind);
    modifyButtonVar(button_Fullscreen);
})();
