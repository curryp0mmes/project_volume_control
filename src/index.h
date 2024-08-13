#pragma once
#include <Arduino.h>
const char mainPage[] PROGMEM = R"=====(
<HTML>
    <HEAD>
        <TITLE>My first web page</TITLE>
    </HEAD>
    <BODY>
        <CENTER>
            <B>Hello World.... </B>
            <a href="https://accounts.spotify.com/authorize?response_type=code&client_id=%s&redirect_uri=%s&scope=user-modify-playback-state user-read-currently-playing user-read-playback-state user-library-modify user-library-read">Log in to spotify</a>
        </CENTER>
    </BODY>
</HTML>
)=====";

const char errorPage[] PROGMEM = R"=====(
<HTML>
    <HEAD>
        <TITLE>My first web page</TITLE>
    </HEAD>
    <BODY>
        <CENTER>
            <B>Hello World.... </B>
            <a href="https://accounts.spotify.com/authorize?response_type=code&client_id=%s&redirect_uri=%s&scope=user-modify-playback-state user-read-currently-playing user-read-playback-state user-library-modify user-library-read">Log in to spotify</a>
        </CENTER>
    </BODY>
</HTML>
)=====";

const char pidPage[] PROGMEM = R"=====(
<HTML><HEAD>
    <TITLE>PID</TITLE>
    <script>let d=document;function u(s,i){d.getElementById(i).value=s.value}function w(){let p=new URLSearchParams(window.location.search);let sr=d.querySelectorAll('input[type="range"]');let sn=d.querySelectorAll('input[type="number"]');for(i in sn){let n=sn[i].name;let v=p.get(n);if(v){sr[i].value=v;sn[i].value=v}}}window.onload = w;</script>
</HEAD><BODY><B>PID Tuner</B><FORM action="pid" method="get">
    <p>P: <input type="range" name="s1" min="0" max="1" step=".001" oninput="u(this,'s1val')"><input type="number" id="s1val" name="s1val" step=".001"></p>
    <p>I: <input type="range" name="s2" min="0" max="1" step=".001" oninput="u(this,'s2val')"><input type="number" id="s2val" name="s2val" step=".001"></p>
    <p>D: <input type="range" name="s3" min="0" max="1" step=".001" oninput="u(this,'s3val')"><input type="number" id="s3val" name="s3val" step=".001"></p>
    <p>T: <input type="range" name="s4" min="0" max="1" step=".001" oninput="u(this,'s4val')"><input type="number" id="s4val" name="s4val" step=".001"></p>
    <p>F: <input type="range" name="s5" min="0" max="1" step=".001" oninput="u(this,'s5val')"><input type="number" id="s5val" name="s5val" step=".001"></p>
    <button type="submit">Submit</button>
</FORM></BODY></HTML>
)=====";