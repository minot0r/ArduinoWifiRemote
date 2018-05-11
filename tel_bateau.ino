#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 

/*       CONFIGURATION      */

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

/* PARAMETRES DE CONNECTION */

const char *ssid = "Telecommande Bateau";
const char *password = "pass";

/* STOCKAGE DES DONNEES */
int sl1 = 127; // SLIDER 1
int sl2 = 127; // SLIDER 2
int sl3 = 90;  // SLIDER 3
bool linked = false; // SLIDER 1 & 2 LINK
bool onoff = false; // LED ON/OFF

ESP8266WebServer server(80);

void handleRoot() {
  String html ="<!DOCTYPE html> <html> <head> <meta charset='UTF-8'> <link href='https://fonts.googleapis.com/css?family=Open+Sans' rel='stylesheet'> <title>Télécommande Bateau</title> <style> body, html{ font-family: 'Open Sans', sans-serif; margin: 0; padding: 0; height: 100%; width: 100%; background-color: #f7f7f7; } div.content{ width: 100%; height: 80%; } div.subcontent{ width: 100%; height: 20%; } div.right{ width: 10%; float: right; height: 100%; } div.left{ width: 10%; height: 100%; float: left; } div.middle{ width: 80%; height: 100%; overflow: hidden; margin-top: 10px; overflow-y: scroll; } .slider { -webkit-appearance: slider-vertical; width: 100%; height: 100%; outline: none; padding: 0; margin: 0; } .horizontal-slider{ width: 80%; height: 100%; outline: none; padding: 0; margin: 0; margin-left: 10%; } .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; border-radius: 50%; background: black; cursor: pointer; } .slider::-moz-range-thumb { width: 25px; height: 25px; border-radius: 50%; background: black; cursor: pointer; } div.box{ background-color: white; border-radius: 5px; -webkit-box-shadow: 0 2px 3px rgba(10, 10, 10, 0.1), 0 0 0 1px rgba(10, 10, 10, 0.1); box-shadow: 0 2px 3px rgba(10, 10, 10, 0.1), 0 0 0 1px rgba(10, 10, 10, 0.1); color: #4a4a4a; display: block; padding: 1.25rem; } button{ -moz-appearance: none; -webkit-appearance: none; -webkit-box-align: center; -ms-flex-align: center; align-items: center; border: 1px solid transparent; border-radius: 3px; -webkit-box-shadow: none; box-shadow: none; display: -webkit-inline-box; display: -ms-inline-flexbox; display: inline-flex; font-size: 1rem; height: 2.25em; -webkit-box-pack: start; -ms-flex-pack: start; justify-content: flex-start; line-height: 1.5; padding-bottom: calc(0.375em - 1px); padding-left: calc(0.625em - 1px); padding-right: calc(0.625em - 1px); padding-top: calc(0.375em - 1px); position: relative; vertical-align: top; -webkit-touch-callout: none; -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: white; border-color: #dbdbdb; color: #363636; cursor: pointer; -webkit-box-pack: center; -ms-flex-pack: center; justify-content: center; padding-left: 0.75em; padding-right: 0.75em; text-align: center; white-space: nowrap; } button:hover{ background-color: #f7f7f7; } </style> <body> <div class='content'> <div class='right'> <input type='range' min='0' max='255' value='{{value2}}' class='slider' id='slide2' orient='vertical'> </div> <div class='left'> <input type='range' min='0' max='255' value='{{value1}}' class='slider' id='slide1' orient='vertical'> </div> <div class='middle'> <div class='box'> <h2>Bienvenue sur la télécommande</h2> <hr> <p>Valeur 1: <span id='o1'></span></p> <p>Valeur 2: <span id='o2'></span></p> <p>Valeur 3: <span id='o3'></span></p> <button id='link'>Relier Slider1 et Slider2</button> <p>Reliés: <span id='o4' value='{{linked}}'>{{linked}}</span></p> <button id='onoff'>On/Off</button> <p>État: <span id='o5' value='{{onoff}}'>{{onoff}}</span></p> <hr> <h4>Développé par Valentin Giorgetti</h4> </div> </div> </div> <div class='subcontent'> <input type='range' min='0' max='180' value='{{value3}}' class='horizontal-slider' id='slide3' orient='vertical'> </div> <script type='text/javascript'> var linked = (document.getElementById('o4').innerHTML == 'true'); var onoff = (document.getElementById('o5').innerHTML == 'true'); var slider1 = document.getElementById('slide1'); var slider2 = document.getElementById('slide2'); var slider3 = document.getElementById('slide3'); var output1 = document.getElementById('o1'); var output2 = document.getElementById('o2'); var output3 = document.getElementById('o3'); var output4 = document.getElementById('o4'); var output5 = document.getElementById('o5'); var btnonoff = document.getElementById('onoff'); var linker = document.getElementById('link'); output1.innerHTML = slider1.value; output2.innerHTML = slider2.value; output3.innerHTML = slider3.value; slider1.oninput = function() { if(linked){ output1.innerHTML = slider1.value; slider2.value = slider1.value; output2.innerHTML = slider2.value; post('/slider1', {val : slider1.value}); post('/slider2', {val : slider2.value}); }else{ output1.innerHTML = slider1.value; post('/slider1', {val : slider1.value}); } }; slider2.oninput = function() { if(linked){ output2.innerHTML = slider2.value; slider1.value = slider2.value; output1.innerHTML = slider1.value; post('/slider1', {val : slider1.value}); post('/slider2', {val : slider2.value}); }else{ output2.innerHTML = slider2.value; post('/slider2', {val : slider2.value}); } }; slider3.oninput = function() { output3.innerHTML = slider3.value; post('/slider3', {val : slider3.value}); }; linker.onclick = function() { linked = !linked; output4.innerHTML = (linked); post('/linked', {val : linked}); }; btnonoff.onclick = function() { onoff = !onoff; output5.innerHTML = (onoff); post('/state', {val : onoff}); }; function post(path, params, method) { method = method || \"post\"; var form = document.createElement(\"form\"); form.setAttribute(\"method\", method); form.setAttribute(\"action\", path); for(var key in params) { if(params.hasOwnProperty(key)) { var hiddenField = document.createElement(\"input\"); hiddenField.setAttribute(\"type\", \"hidden\"); hiddenField.setAttribute(\"name\", key); hiddenField.setAttribute(\"value\", params[key]); form.appendChild(hiddenField); } } document.body.appendChild(form); form.submit(); }; </script> </body> </head> </html>";
  html.replace("{{value1}}", String(sl1, DEC));
  html.replace("{{value2}}", String(sl2, DEC));
  html.replace("{{value3}}", String(sl3, DEC));
  html.replace("{{onoff}}", onoff ? "true" : "false");
  html.replace("{{linked}}", linked ? "true" : "false");

server.send(200, "text/html", html);
}

void handleResponseForSlider1(){
  if(server.hasArg("val")){
    Serial.print("User entered:\t");
    Serial.println(server.arg("val"));
    sl1 = server.arg("val").toInt();
    basicReponse();
  }
}

void handleResponseForSlider2(){
  if(server.hasArg("val")){
    Serial.print("User entered:\t");
    Serial.println(server.arg("val"));
    sl2 = server.arg("val").toInt();
    basicReponse();
  }
}

void handleResponseForSlider3(){
  if(server.hasArg("val")){
    Serial.print("User entered:\t");
    Serial.println(server.arg("val"));
    sl3 = server.arg("val").toInt();
    basicReponse();
  }
}

void handleResponseLinked(){
  if(server.hasArg("val")){
    Serial.print("User entered:\t");
    Serial.println(server.arg("val"));
    linked = !linked;
    basicReponse();
  }
}

void handleResponseOnOff(){
  if(server.hasArg("val")){
    Serial.print("User entered:\t");
    Serial.println(server.arg("val"));
    onoff = !onoff;
    basicReponse();
  }
}

void basicReponse(){
  server.send(200, "text/html", "<meta http-equiv='refresh' content='0; URL=/'>");
}

void configureAP(){
  Serial.println("Configuration du point d'accès: ");
  /* Serial.println("Mise en place de la configuration réseau: ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Réussie" : "Ratée!"); **/

  Serial.println("Mise en place du point d'accès: ");
  Serial.println(WiFi.softAP(ssid) ? "Réussi" : "Raté!");

  Serial.print("Adresse IP du point d'accès: ");
  Serial.println(WiFi.softAPIP());
}


void setup() {
  delay(1000);
  Serial.begin(115200);

  configureAP();
  
  server.on("/", handleRoot);
  server.on("/slider1", HTTP_POST, handleResponseForSlider1); // Slider gauche
  server.on("/slider2", HTTP_POST, handleResponseForSlider2); // Slider droit
  server.on("/slider3", HTTP_POST, handleResponseForSlider3); // Slider en bas
  server.on("/linked", HTTP_POST, handleResponseLinked); // Slider 1 & 2 reliés ou non
  server.on("/state", HTTP_POST, handleResponseOnOff); // Etat de la led
  server.on("/slider1", basicReponse); // Réponse basique
  server.on("/slider2", basicReponse); // Réponse basique
  server.on("/slider3", basicReponse); // Réponse basique
  server.on("/linked", basicReponse); // Réponse basique
  server.on("/state", basicReponse); // Réponse basique
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
